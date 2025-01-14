#ifndef FS_CONSTANTS
#define FS_CONSTANTS

constexpr int BLOCK_SIZE = 16;
constexpr int MEMORY_SIZE = 4096;

constexpr int NUM_BLOCKS = MEMORY_SIZE / BLOCK_SIZE;
constexpr int NUM_FILES = NUM_BLOCKS;

constexpr int FREE_BLOCK = -1;
constexpr int NO_OPENED_FILE = -1;

constexpr int MAX_NAME_SIZE = 16;
constexpr int MIN_NAME_SIZE = 1;

struct FileDescriptor {
    std::string name;
    int starting_block;
    int size;
};

struct OpenedFile {
    int idx;        // idx of opened file descriptor
    int offset;     // bytes of read file contents
};

#endif