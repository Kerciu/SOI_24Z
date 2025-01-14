#include <iostream>
#include <cstring>
#include "constants.h"

class FileSystem {

    private:
        char memory[MEMORY_SIZE];
        int fat[NUM_BLOCKS];
        
        FileDescriptor fd_table[NUM_FILES];
        int file_count;

        OpenedFile open_file_fd_table[NUM_FILES];      // idx of opened file
        int opened_file_count;

    public:
        FileSystem();
        void displayState();

        bool create(const std::string& name, int size);
        void open(int ptr_idx);
        void close(int ptr_idx);
        void read(int ptr_idx);
        void write(int ptr_idx, const char * data, int size);
};