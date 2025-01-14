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

        int findOpenFileFreeSlot(int file_idx);
        int findFileIndexByName(const std::string &name);

    public:
        FileSystem();
        void displayState();


        FileCreateStatus create(const std::string &name, int size);
        FileOpenStatus open(const std::string& name);
        FileCloseStatus close(const std::string& name);
        FileReadStatus read(const std::string& name);
        FileWriteStatus write(const std::string& name, const std::string& data, int size);
        FileDeleteStatus delete_(const std::string& name);     // delete is a c++ keyword
};