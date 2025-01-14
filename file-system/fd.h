#ifndef FS_FILE_DESCRIPTOR
#define FS_FILE_DESCRIPTOR


#include <string>

struct FileDescriptor {
    std::string name;
    int starting_block;
    int size;
};

#endif