#include "fs.h"

FileSystem::FileSystem()
{
    std::memset(memory, 0, sizeof(memory));
    std::fill(fat, fat + NUM_BLOCKS, FREE_BLOCK);
    std::memset(fd_table, 0, sizeof(fd_table));
    file_count = 0;
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
}