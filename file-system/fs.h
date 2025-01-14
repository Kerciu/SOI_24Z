#include <iostream>
#include <cstring>
#include "constants.h"
#include "fd.h"

class FileSystem {

    private:

    char memory[MEMORY_SIZE];
    int fat[NUM_BLOCKS];
    FileDescriptor fd_table[NUM_FILES];
    int file_count;

    public:

    FileSystem();

    void displayState();
};