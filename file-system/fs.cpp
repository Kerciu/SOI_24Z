#include "fs.h"

FileDescriptor emptyFileDescriptor() {
    FileDescriptor fd;
    std::memset(fd.name, 0, MAX_NAME_SIZE);
    fd.starting_block = -1;
    fd.size = 0;
    return fd;
}


FileSystem::FileSystem()
{
    try {
        restoreFromDisk();
        repair();
    } catch (const std::exception& e)
    {
        std::cout << "Error while restoring data from disk: " << e.what() << ", setting default values\n";
        std::memset(memory, 0, sizeof(memory));
        std::fill(fat, fat + NUM_BLOCKS, FREE_BLOCK);
        for (int i = 0; i < NUM_FILES; ++i) {
            fd_table[i] = emptyFileDescriptor();
        }
        file_count = 0;
    }

    for (int i = 0; i < NUM_OPENED_FILES; ++i) {
        open_file_fd_table[i] = {NO_OPENED_FILE, 0};
    }

    opened_file_count = 0;
    transaction_log.in_progress = false;
    saveToDisk();
    displayInitialVals();
}

FileCreateStatus FileSystem::create(const std::string &name, uint16_t size = 0)
{
    if (size > MEMORY_SIZE || size < 0)
        return FILE_CREATE_INVALID_SIZE;

    if (file_count >= NUM_FILES)
        return FILE_CREATE_TOO_MANY_FILES;

    if (name.length() > MAX_NAME_SIZE || name.length() < MIN_NAME_SIZE)
    {
        return FILE_CREATE_INVALID_NAME;
    }

    if (duplicateName(name))
        return FILE_CREATE_DUPLICATE_FILE;

    int blocks_needed = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;

    if (countFreeBlocks() < blocks_needed)
        return FILE_CREATE_NO_SPACE;

    // allocate blocks in fat
    int16_t starting_block = firstFreeBlock();
    if (starting_block == -1) {
        return FILE_CREATE_NO_SPACE;
    }

    transaction_log.in_progress = true;
    transaction_log.file_idx = file_count;
    transaction_log.last_valid_block = -1;

    int current_block = starting_block;

    for (int i = 1; i <= blocks_needed; ++i)
    {
        markBlockAsUsed(current_block);
        transaction_log.last_valid_block = current_block;

        int next_block = firstFreeBlock();
        if (i == blocks_needed) {
            fat[current_block] = END_OF_CHAIN; // last block in chain
        } else {
            fat[current_block] = next_block; // link next block
        }

        current_block = next_block;

    }

    // add new file to file descriptor table
    strncpy(fd_table[file_count].name, name.c_str(), MAX_NAME_SIZE - 1);
    fd_table[file_count].name[MAX_NAME_SIZE - 1] = '\0';
    fd_table[file_count].starting_block = starting_block;
    fd_table[file_count].size = size;
    file_count++;

    transaction_log.in_progress = false;

    return FILE_CREATE_SUCCESS;
}



FileOpenStatus FileSystem::open(const std::string& name)
{
    if (opened_file_count >= NUM_FILES)
        return FILE_OPEN_TOO_MANY_FILES_OPENED;

    int16_t file_idx = findFileIndexByName(name);

    if (file_idx == -1)
        return FILE_OPEN_DOESNT_EXIST;

    int free_slot = findOpenFileFreeSlot(file_idx);

    if (free_slot == -1)
        return FILE_OPEN_NO_SPACE_FOR_OPEN_FILES;

    open_file_fd_table[free_slot] = {file_idx, 0};
    ++opened_file_count;

    return FILE_OPEN_SUCCESS;
}

FileCloseStatus FileSystem::close(const std::string& name)
{
    int file_index = findOpenFileIndexByName(name);

    if (file_index == -1) {
        return FILE_CLOSE_INVALID_INDEX;
    }

    if (open_file_fd_table[file_index].idx == NO_OPENED_FILE) {
        return FILE_CLOSE_NOT_OPENED;
    }

    open_file_fd_table[file_index] = {NO_OPENED_FILE, 0};
    --opened_file_count;

    return FILE_CLOSE_SUCCESS;
}

FileReadStatus FileSystem::read(const std::string& name, char* buffer, uint16_t size)
{
    int file_idx = findFileIndexByName(name);

    if (file_idx == -1)
        return FILE_READ_INVALID_INDEX;

    if (open_file_fd_table[file_idx].idx == NO_OPENED_FILE)
        return FILE_READ_NOT_OPEN;

    int fd_index = open_file_fd_table[file_idx].idx;
    FileDescriptor& file = fd_table[fd_index];

    int offset = open_file_fd_table[file_idx].offset;
    if (offset > file.size) {
        return FILE_READ_EOF;
    }

    if (offset + size > file.size)
    {
        size = file.size - offset;  // adjust
    }

    int bytes_to_read = size;
    int current_offset = offset;
    int buffer_idx = 0;

    int block_idx = file.starting_block;
    int block_offset = 0;

    while (current_offset >= BLOCK_SIZE)
    {
        block_idx = fat[block_idx];     // next block in fat
        current_offset -= BLOCK_SIZE;
    }
    block_offset = current_offset;

    while (bytes_to_read > 0)
    {
        int bytes_in_block = std::min(BLOCK_SIZE - block_offset, bytes_to_read);

        std::memcpy(buffer + buffer_idx, memory + (block_idx * BLOCK_SIZE) + block_offset, bytes_in_block);

        buffer_idx += bytes_in_block;
        bytes_to_read -= bytes_in_block;
        block_offset = 0;

         // go to the next block if there are any data left to read
        if (bytes_to_read > 0) {
            block_idx = fat[block_idx];
            if (block_idx == END_OF_CHAIN) {
                break; // unexcpected end of file in chain
            }
        }
    }

    open_file_fd_table[file_idx].offset += size;

    return FILE_READ_SUCCESS;
}

FileWriteStatus FileSystem::write(const std::string& name, char* buffer, uint16_t size)
{
    int file_idx = findFileIndexByName(name);

    if (file_idx == -1)
        return FILE_WRITE_INVALID_INDEX;

    if (open_file_fd_table[file_idx].idx == NO_OPENED_FILE)
        return FILE_WRITE_NOT_OPENED;

    int fd_index = open_file_fd_table[file_idx].idx;
    FileDescriptor& file = fd_table[fd_index];

    int offset = open_file_fd_table[file_idx].offset;
    int new_size = offset + size;
    if (new_size > file.size)
    {
        transaction_log.in_progress = true;
        transaction_log.file_idx = file_idx;
        transaction_log.new_size = new_size;
        transaction_log.last_valid_block = file.starting_block;
        transaction_log.first_new_block = -1;

        // dodac do logow nowy rozmiar, indeks ostatniej j.a. i index nowej j.a.
        int additional_blocks_needed = (new_size + BLOCK_SIZE - 1) / BLOCK_SIZE -
                                       (file.size + BLOCK_SIZE - 1) / BLOCK_SIZE;

        for (int i = 0; i < additional_blocks_needed; ++i)
        {
            int new_block = firstFreeBlock();
            if (new_block == -1) {
                transactionRollback();
                return FILE_WRITE_NO_SPACE;
            }

            int last_block = file.starting_block;
            while (fat[last_block] != END_OF_CHAIN)
                last_block = fat[last_block];

            if (transaction_log.first_new_block == -1)
                transaction_log.first_new_block = new_block;

            fat[last_block] = new_block;
            fat[new_block] = END_OF_CHAIN;
        }

        file.size = new_size;
        transaction_log.in_progress = false;
    }

    int bytes_to_write = size;
    int current_offset = offset;
    int buffer_idx = 0;

    int block_idx = file.starting_block;
    int block_offset = 0;

    while (current_offset >= BLOCK_SIZE)
    {
        block_idx = fat[block_idx];     // next block in fat
        if (block_idx == END_OF_CHAIN)
            return FILE_WRITE_EXCEEDS_FILE_SIZE; // unexpected end of chain
        current_offset -= BLOCK_SIZE;
    }

    block_offset = current_offset;

    while (bytes_to_write > 0)
    {
        int bytes_in_block = std::min(BLOCK_SIZE - block_offset, bytes_to_write);

        std::memcpy(memory + (block_idx * BLOCK_SIZE) + block_offset, buffer + buffer_idx, bytes_in_block);

        buffer_idx += bytes_in_block;
        bytes_to_write -= bytes_in_block;
        block_offset = 0;

         // go to the next block if there are any data left to read
        if (bytes_to_write > 0) {
            block_idx = fat[block_idx];
            if (block_idx == END_OF_CHAIN) {
                break; // unexcpected end of file in chain
            }
        }
    }

    open_file_fd_table[file_idx].offset += size;

    return FILE_WRITE_SUCCESS;
}

FileDeleteStatus FileSystem::delete_(const std::string& name)
{
    int file_idx = findFileIndexByName(name);
    if (file_idx == -1)
        return FILE_DELETE_DOESNT_EXIST;

    transaction_log.in_progress = true;
    transaction_log.file_idx = file_idx;
    transaction_log.last_valid_block = fd_table[file_idx].starting_block;

    int next_block;
    int start_block = fd_table[file_idx].starting_block;
    while (start_block != END_OF_CHAIN) {
        next_block = fat[start_block];
        fat[start_block] = FREE_BLOCK;
        start_block = next_block;
    }

    for (int i = 0; i < file_count; ++i) {
        fd_table[i] = fd_table[i + 1];
    }
    --file_count;

    fd_table[file_count] = emptyFileDescriptor();

    for (int i = 0; i < opened_file_count; i++) {
        if (open_file_fd_table[i].idx == file_idx) {
            open_file_fd_table[i] = {NO_OPENED_FILE, 0}; // if opened set as closed
            --opened_file_count;
        }
    }

    transaction_log.in_progress = false;

    return FILE_DELETE_SUCCESS;
}
