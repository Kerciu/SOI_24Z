#include <iostream>
#include <cstring>
#include <cmath>
#include "constants.h"

class FileSystem {

    private:
        char memory[MEMORY_SIZE];
        int fat[NUM_BLOCKS];
        
        FileDescriptor fd_table[NUM_FILES];
        int file_count;

        OpenedFile open_file_fd_table[NUM_FILES];      // idx of opened file
        int opened_file_count;

        bool duplicateName(const std::string &name);
        int countFreeBlocks();
        int firstFreeBlock();
        void markBlockAsUsed(int block_idx);

    public:
        FileSystem();
        void displayState();


        FileCreateStatus create(const std::string& name, int size);
        FileOpenStatus open(int ptr_idx);
        FileCloseStatus close(int ptr_idx);
        FileReadStatus read(int ptr_idx);
        FileWriteStatus write(int ptr_idx, const char * data, int size);
        FileDeleteStatus delete_(int ptr_idx);     // delete is a c++ keyword
};