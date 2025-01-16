#include <iostream>
#include <cstring>
#include <cstdint>
#include <cmath>
#include "constants.h"

class FileSystem {

    private:
        char memory[MEMORY_SIZE];
        int16_t fat[NUM_BLOCKS];
        
        FileDescriptor fd_table[NUM_FILES];
        uint8_t file_count;

        OpenedFile open_file_fd_table[NUM_OPENED_FILES];      // idx of opened file
        uint8_t opened_file_count;

        TransactionLog transaction_log;

        bool duplicateName(const std::string &name);
        int countFreeBlocks();
        int firstFreeBlock();
        void markBlockAsUsed(int block_idx);

        int findOpenFileFreeSlot(int file_idx);
        int findFileIndexByName(const std::string &name);

        int findOpenFileIndexByName(const std::string &name);

    public:
        FileSystem();
        void displayState();

        FileCreateStatus create(const std::string &name, uint16_t size);
        FileOpenStatus open(const std::string& name);
        FileCloseStatus close(const std::string& name);
        FileReadStatus read(const std::string& name, char* buffer, uint16_t size);
        FileWriteStatus write(const std::string& name, char* buffer, uint16_t size);
        FileDeleteStatus delete_(const std::string& name);     // delete is a c++ keyword
};