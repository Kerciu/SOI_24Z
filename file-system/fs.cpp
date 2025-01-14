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

bool FileSystem::create(const std::string &name, int size)
{
    return false;
}

void FileSystem::open(int ptr_idx)
{
}

void FileSystem::close(int ptr_idx)
{
}

void FileSystem::read(int ptr_idx)
{
}

void FileSystem::write(int ptr_idx, const char *data, int size)
{
}
