#include "fs.h"

FileSystem::FileSystem()
{
    std::memset(memory, 0, sizeof(memory));
    std::fill(fat, fat + NUM_BLOCKS, FREE_BLOCK);
    for (int i = 0; i < NUM_FILES; ++i) {
        fd_table[i] = {"", -1, 0};
        open_file_fd_table[i] = {NO_OPENED_FILE, 0};
    }
    file_count = 0;
    opened_file_count = 0;

    fd_table[0] = {"TestFile", 0, 16};
    file_count ++;

}

void FileSystem::displayState() {
    std::cout << "File System State:\n";

    std::cout << "FAT Table:\n";
    for (int i = 0; i < NUM_BLOCKS; ++i) {
        std::cout << "Block " << i << ": " << fat[i] << "\n";
    }

    std::cout << "\nFile Descriptors:\n";
    for (int i = 0; i < file_count; ++i) {
        std::cout << " - " << fd_table[i].name
                  << ": Start=" << fd_table[i].starting_block
                  << ", Size=" << fd_table[i].size << "\n";
    }

    std::cout << "\nOpened Files:\n";
    for (int i = 0; i < NUM_FILES; ++i) {
        if (open_file_fd_table[i].idx != NO_OPENED_FILE) {
            int idx = open_file_fd_table[i].idx;
            if (idx >= 0 && idx < file_count) {
                FileDescriptor& fd = fd_table[idx];
                std::cout << " - " << fd.name << ": Offset=" << open_file_fd_table[i].offset << "\n";
            }
        }
    }
}

bool FileSystem::duplicateName(const std::string& name)
{
    for (int i = 0; i < file_count; ++i) {
        if (fd_table[i].name == name) {
            return true;
        }
    }
    return false;
}

int FileSystem::countFreeBlocks()
{
    int free_blocks = 0;
    for (int i = 0; i < NUM_BLOCKS; ++i) {
        if (fat[i] == FREE_BLOCK) {
            ++free_blocks;
        }
    }
    return free_blocks;
}

int FileSystem::firstFreeBlock()
{
    for (int i = 0; i < NUM_BLOCKS; ++i) {
        if (fat[i] == FREE_BLOCK) {
            return i;
        }
    }
    return -1;
}

void FileSystem::markBlockAsUsed(int block_idx)
{
    fat[block_idx] = END_OF_CHAIN;
}

int FileSystem::findFileIndexByName(const std::string& name)
{
    for (int i = 0; i < file_count; ++i) {
        if (fd_table[i].name == name) {
            return i;
        }
    }

    return -1;
}

int FileSystem::findOpenFileIndexByName(const std::string& name)
{
    for (int i = 0; i < NUM_FILES; ++i) {
        if (open_file_fd_table[i].idx != NO_OPENED_FILE) {
            int file_index = open_file_fd_table[i].idx;
            if (file_index >= 0 && file_index < file_count && fd_table[file_index].name == name) {
                return i;
            }
        }
    }
    return -1;
}

int FileSystem::findOpenFileFreeSlot(int file_idx)
{
    for (int i = 0; i < NUM_FILES; ++ i)
    {
        if (open_file_fd_table[i].idx == NO_OPENED_FILE)
            return i;
    }

    return -1;
}

FileCreateStatus FileSystem::create(const std::string &name, int size)
{
    if (size > MEMORY_SIZE || size <= 0)
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
    int starting_block = firstFreeBlock();
    if (starting_block == -1) {
        return FILE_CREATE_NO_SPACE;
    }

    int current_block = starting_block;

    for (int i = 1; i <= blocks_needed; ++i)
    {
        markBlockAsUsed(current_block);
        int next_block = firstFreeBlock();
        if (i == blocks_needed - 1) {
            fat[current_block] = END_OF_CHAIN; // last block in chain
        } else {
            fat[current_block] = next_block; // link next block
        }

        current_block = next_block;

    }

    // add new file to file descriptor table
    fd_table[file_count++] = {name, starting_block, size};

    return FILE_CREATE_SUCCESS;
}



FileOpenStatus FileSystem::open(const std::string& name)
{
    if (opened_file_count >= NUM_FILES)
        return FILE_OPEN_TOO_MANY_FILES_OPENED;
    
    int file_idx = findFileIndexByName(name);

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

FileReadStatus FileSystem::read(const std::string& name)
{

}

FileWriteStatus FileSystem::write(const std::string& name, const std::string& data, int size)
{

}

FileDeleteStatus FileSystem::delete_(const std::string& name)
{

}
