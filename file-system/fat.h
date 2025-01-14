#ifndef FS_FAT
#define FS_FAT


#include <string>

struct FAT {
    std::string name;
    int starting_block;
    int size;
};

#endif