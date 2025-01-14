#ifndef FS_CONSTANTS
#define FS_CONSTANTS

constexpr int BLOCK_SIZE = 16;
constexpr int MEMORY_SIZE = 4096;

constexpr int NUM_BLOCKS = MEMORY_SIZE / BLOCK_SIZE;
constexpr int NUM_FILES = NUM_BLOCKS;

constexpr int FREE_BLOCK = -1;
constexpr int NO_OPENED_FILE = -1;
constexpr int END_OF_CHAIN = -2;

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

typedef enum {
    FILE_CREATE_SUCCESS,
    FILE_CREATE_INVALID_SIZE,
    FILE_CREATE_INVALID_NAME,
    FILE_CREATE_TOO_MANY_FILES,
    FILE_CREATE_DUPLICATE_FILE,
    FILE_CREATE_NO_SPACE
} FileCreateStatus;

typedef enum {
    FILE_OPEN_SUCCESS,
    FILE_OPEN_TOO_MANY_FILES_OPENED,
    FILE_OPEN_DOESNT_EXIST,
    FILE_OPEN_NO_SPACE_FOR_OPEN_FILES
} FileOpenStatus;

typedef enum {
    FILE_CLOSE_SUCCESS,
    FILE_CLOSE_INVALID_INDEX,
    FILE_CLOSE_NOT_OPENED
} FileCloseStatus;

typedef enum {
    FILE_READ_SUCCESS,
    FILE_READ_INVALID_INDEX,
    FILE_READ_NOT_OPEN
} FileReadStatus;

typedef enum {
    FILE_WRITE_SUCCESS,
    FILE_WRITE_INVALID_INDEX,
    FILE_WRITE_NOT_OPENED,
    FILE_WRITE_EXCEEDS_FILE_SIZE
} FileWriteStatus;

typedef enum {
    FILE_DELETE_SUCCESS,
    FILE_DELETE_DOESNT_EXIST,
} FileDeleteStatus;

#endif