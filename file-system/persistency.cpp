#include "fs.h"

void FileSystem::saveToDisk()
{
    std::ofstream mem_file("./data/memory.txt");
    if (!mem_file.is_open()) {
        throw std::runtime_error("Failed to open 'memory.txt'. File does not exist or cannot be accessed.");
    }

    for (int i = 0; i < MEMORY_SIZE; ++i) {
        mem_file << "Memory[" << i << "]: " << static_cast<int>(memory[i]) << std::endl;
    }
    mem_file.close();

    saveFATToDisk();
    saveFileDescriptorsToDisk();
    saveTransactionLogsToDisk();
    saveFileCountToDisk();
}

void FileSystem::restoreFromDisk()
{
    std::ifstream mem_file("./data/memory.txt");
    if (!mem_file.is_open()) {
        throw std::runtime_error("Failed to open 'memory.txt'. File does not exist or cannot be accessed.");
    }

    std::string line;
    while (std::getline(mem_file, line)) {
        size_t pos = line.find(":");
        if (pos != std::string::npos) {
            std::string index_str = line.substr(7, pos - 7);
            int idx = std::stoi(index_str);
            int value = std::stoi(line.substr(pos + 2));
            memory[idx] = static_cast<char>(value);
        }
    }
    mem_file.close();

    restoreFATFromDisk();
    restoreFileDescriptorsFromDisk();
    restoreTransactionLogsFromDisk();
    restoreFileCountFromDisk();
}

void FileSystem::saveFATToDisk() {
    std::ofstream fat_file("./data/fat.txt");
    if (!fat_file.is_open()) {
        throw std::runtime_error("Failed to open 'fat.txt'. File does not exist or cannot be accessed.");
    }

    for (int i = 0; i < NUM_BLOCKS; ++i) {
        fat_file << "FAT[" << i << "]: " << fat[i] << std::endl;
    }

    fat_file.close();
}

void FileSystem::saveFileDescriptorsToDisk() {
    std::ofstream file_desc_file("./data/file_descriptors.txt");
    if (!file_desc_file.is_open()) {
        throw std::runtime_error("Failed to open 'file_descriptors.txt'. File does not exist or cannot be accessed.");
    }

    for (const auto& fd : fd_table) {
        file_desc_file << "Name: " << fd.name << std::endl;
        file_desc_file << "Starting Block: " << fd.starting_block << std::endl;
        file_desc_file << "Size: " << fd.size << std::endl;
        file_desc_file << "----" << std::endl;
    }

    file_desc_file.close();
}

void FileSystem::saveTransactionLogsToDisk() {
    std::ofstream log_file("./data/transaction_log.txt");
    if (!log_file.is_open()) {
        throw std::runtime_error("Failed to open 'transaction_log.txt'. File does not exist or cannot be accessed.");
    }

    log_file << "In Progress: " << (transaction_log.in_progress ? "true" : "false") << std::endl;
    log_file << "File Index: " << static_cast<int>(transaction_log.file_idx) << std::endl;
    log_file << "New Size: " << transaction_log.new_size << std::endl;
    log_file << "Last Valid Block: " << static_cast<int>(transaction_log.last_valid_block) << std::endl;
    log_file << "First New Block: " << static_cast<int>(transaction_log.first_new_block) << std::endl;

    log_file.close();
}

void FileSystem::saveFileCountToDisk() {
    std::ofstream file_count_file("./data/file_count.txt");
    if (!file_count_file.is_open()) {
        throw std::runtime_error("Failed to open 'file_count.txt'. File does not exist or cannot be accessed.");
    }

    file_count_file << "File Count: " << file_count << std::endl;

    file_count_file.close();
}

void FileSystem::restoreFATFromDisk() {
    std::ifstream fat_file("./data/fat.txt");
    if (!fat_file.is_open()) {
        throw std::runtime_error("Failed to open 'fat.txt'. File does not exist or cannot be accessed.");
    }

    std::string line;
    while (std::getline(fat_file, line)) {
        size_t start_bracket = line.find("[");
        size_t end_bracket = line.find("]");
        size_t colon_pos = line.find(":");

        if (start_bracket != std::string::npos && end_bracket != std::string::npos && colon_pos != std::string::npos) {
            std::string index_str = line.substr(start_bracket + 1, end_bracket - start_bracket - 1);
            std::string value_str = line.substr(colon_pos + 2);

            try {
                int idx = std::stoi(index_str);
                int value = std::stoi(value_str);

                fat[idx] = value;
            } catch (const std::exception& e) {
            }
        } else {
        }
    }
    fat_file.close();
}

void FileSystem::restoreFileDescriptorsFromDisk() {
    std::ifstream file_desc_file("./data/file_descriptors.txt");
    if (!file_desc_file.is_open()) {
        throw std::runtime_error("Failed to open 'file_descriptors.txt'. File does not exist or cannot be accessed.");
    }

    std::string line;
    FileDescriptor fd = {};
    int i = 0;
    while (std::getline(file_desc_file, line)) {
        if (line.find("Name:") != std::string::npos) {
            std::string name = line.substr(6);
            if (!name.empty()) {
                fd.name = name;
            } else {
                fd.name = "";
            }
        }
        else if (line.find("Starting Block:") != std::string::npos) {
            fd.starting_block = std::stoi(line.substr(15));
        }
        else if (line.find("Size:") != std::string::npos) {
            fd.size = std::stoi(line.substr(6));
        }
        else if (line == "----" && i != NUM_FILES) {
            fd_table[i++] = fd;
        }
    }

    file_desc_file.close();
}

void FileSystem::restoreTransactionLogsFromDisk() {
    std::ifstream log_file("./data/transaction_log.txt");
    if (!log_file.is_open()) {
        throw std::runtime_error("Failed to open 'transaction_log.txt'. File does not exist or cannot be accessed.");
    }

    std::string line;
    while (std::getline(log_file, line)) {
        if (line.find("In Progress:") != std::string::npos) {
            transaction_log.in_progress = (line.substr(13) == "true");
        }
        else if (line.find("File Index:") != std::string::npos) {
            transaction_log.file_idx = std::stoi(line.substr(12));
        }
        else if (line.find("New Size:") != std::string::npos) {
            transaction_log.new_size = std::stoi(line.substr(9));
        }
        else if (line.find("Last Valid Block:") != std::string::npos) {
            transaction_log.last_valid_block = std::stoi(line.substr(17));
        }
        else if (line.find("First New Block:") != std::string::npos) {
            transaction_log.first_new_block = std::stoi(line.substr(17));
        }
    }

    log_file.close();
}

void FileSystem::restoreFileCountFromDisk() {
    std::ifstream file_count_file("./data/file_count.txt");
    if (!file_count_file.is_open()) {
        throw std::runtime_error("Failed to open 'file_count.txt'. File does not exist or cannot be accessed.");
    }

    std::string line;
    while (std::getline(file_count_file, line)) {
        if (line.find("File Count:") != std::string::npos) {
            file_count = std::stoi(line.substr(12));
        }
    }

    file_count_file.close();
}