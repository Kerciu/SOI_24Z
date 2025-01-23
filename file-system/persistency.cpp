#include "fs.h"

void FileSystem::saveToDisk()
{
    std::ofstream mem_file("./data/mem.dat", std::ios::binary);
    if (!mem_file.is_open()) {
        throw std::runtime_error("Failed to open 'fat.mem'. File does not exist or cannot be accessed.");
    }
    mem_file.write(reinterpret_cast<char*>(memory), sizeof(memory));
    mem_file.close();
    saveFATToDisk();
    saveFileDescriptorsToDisk();
    saveTransactionLogsToDisk();
    saveFileCountToDisk();
}

void FileSystem::restoreFromDisk()
{
    std::ifstream mem_file("./data/mem.dat", std::ios::binary);
    if (!mem_file.is_open()) {
        throw std::runtime_error("Failed to open 'fat.mem'. File does not exist or cannot be accessed.");
    }
    mem_file.read(reinterpret_cast<char*>(memory), sizeof(memory));
    mem_file.close();
    restoreFATFromDisk();
    restoreFileDescriptorsFromDisk();
    restoreTransactionLogsFromDisk();
    restoreFileCountFromDisk();
}

void FileSystem::saveFATToDisk() {
    std::ofstream fat_file("./data/fat.dat", std::ios::binary);
    fat_file.write(reinterpret_cast<char*>(fat), sizeof(fat));
    fat_file.close();
}

void FileSystem::saveFileDescriptorsToDisk() {
    std::ofstream file_desc_file("./data/file_descriptors.dat", std::ios::binary);
    file_desc_file.write(reinterpret_cast<char*>(&fd_table), sizeof(fd_table));
    file_desc_file.close();
}

void FileSystem::saveTransactionLogsToDisk() {
    std::ofstream log_file("./data/transaction_log.dat", std::ios::binary);
    log_file.write(reinterpret_cast<char*>(&transaction_log), sizeof(transaction_log));
    log_file.close();
}

void FileSystem::saveFileCountToDisk() {
    std::ofstream file_count_file("./data/file_count.dat", std::ios::binary);
    file_count_file.write(reinterpret_cast<char*>(&file_count), sizeof(file_count));
    file_count_file.close();
}

void FileSystem::restoreFATFromDisk() {
    std::ifstream fat_file("./data/fat.dat", std::ios::binary);
    if (!fat_file.is_open()) {
        throw std::runtime_error("Failed to open 'fat.dat'. File does not exist or cannot be accessed.");
    }
    fat_file.read(reinterpret_cast<char*>(fat), sizeof(fat));
    fat_file.close();
}

void FileSystem::restoreFileDescriptorsFromDisk() {
    std::ifstream file_desc_file("./data/file_descriptors.dat", std::ios::binary);
    if (!file_desc_file.is_open()) {
        throw std::runtime_error("Failed to open 'file_desc_file.dat'. File does not exist or cannot be accessed.");
    }
    file_desc_file.read(reinterpret_cast<char*>(&fd_table), sizeof(fd_table));
    file_desc_file.close();
}

void FileSystem::restoreTransactionLogsFromDisk() {
    std::ifstream log_file("./data/transaction_log.dat", std::ios::binary);
    if (!log_file.is_open()) {
        throw std::runtime_error("Failed to open 'log_file.dat'. File does not exist or cannot be accessed.");
    }
    log_file.read(reinterpret_cast<char*>(&transaction_log), sizeof(transaction_log));
    log_file.close();
}

void FileSystem::restoreFileCountFromDisk() {
    std::ifstream file_count_file("./data/file_count.dat", std::ios::binary);
    if (!file_count_file.is_open()) {
        throw std::runtime_error("Failed to open 'file_count_file.dat'. File does not exist or cannot be accessed.");
    }
    file_count_file.read(reinterpret_cast<char*>(&file_count), sizeof(file_count));
    file_count_file.close();
}
