#include "fs.h"

void FileSystem::displayInitialVals()
{
    std::cout << "Memory State: ";
    for (int i = 0; i < MEMORY_SIZE; ++i) {
        std::cout << static_cast<int>(memory[i]) << " ";
    }
    std::cout << std::endl;

    std::cout << "FAT Table: ";
    for (int i = 0; i < NUM_BLOCKS; ++i) {
        std::cout << fat[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "File Descriptors: " << std::endl;
    for (int i = 0; i < NUM_FILES; ++i) {
        std::cout << "File " << i << ": " 
                  << "Starting Block: " << fd_table[i].starting_block << ", "
                  << "Size: " << fd_table[i].size << ", "
                  << "Name: " << fd_table[i].name << std::endl;
    }

    std::cout << "File Count: " << file_count << std::endl;

    std::cout << "Opened File Descriptors: " << std::endl;
    for (int i = 0; i < NUM_OPENED_FILES; ++i) {
        std::cout << "Open File " << i << ": "
                  << "File Index: " << open_file_fd_table[i].idx << ", "
                  << "Offset: " << open_file_fd_table[i].offset << std::endl;
    }

    std::cout << "Opened File Count: " << opened_file_count << std::endl;

    std::cout << "Transaction Log: " 
              << "In Progress: " << transaction_log.in_progress << ", "
              << "File Index: " << transaction_log.file_idx << ", "
              << "New Size: " << transaction_log.new_size << ", "
              << "Last Valid Block: " << transaction_log.last_valid_block << ", "
              << "First New Block: " << transaction_log.first_new_block << std::endl;
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
    for (int i = 0; i < NUM_OPENED_FILES; ++i) {
        if (open_file_fd_table[i].idx != NO_OPENED_FILE) {
            int idx = open_file_fd_table[i].idx;
            if (idx >= 0 && idx < file_count) {
                FileDescriptor& fd = fd_table[idx];
                std::cout << " - " << fd.name << ": Offset=" << open_file_fd_table[i].offset << "\n";
            }
        }
    }
}