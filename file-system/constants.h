#ifndef FS_CONSTANTS
#define FS_CONSTANTS

#include <cstdint>

constexpr int BLOCK_SIZE = 32;
constexpr int MEMORY_SIZE = 2048;

constexpr int NUM_BLOCKS = MEMORY_SIZE / BLOCK_SIZE;
constexpr int NUM_FILES = 16;
constexpr int NUM_OPENED_FILES = 8;

constexpr int16_t FREE_BLOCK = -1;
constexpr int16_t NO_OPENED_FILE = -1;
constexpr int16_t END_OF_CHAIN = -2;

constexpr int MAX_NAME_SIZE = 10;
constexpr int MIN_NAME_SIZE = 1;

struct FileDescriptor {
    std::string name;
    int16_t starting_block;
    uint16_t size;
};

struct OpenedFile {
    int16_t idx;        // idx of opened file descriptor
    uint16_t offset;     // bytes of read file contents
};

struct TransactionLog {
    bool in_progress;
    uint8_t file_idx;
    uint8_t last_block;
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
    FILE_WRITE_EXCEEDS_FILE_SIZE,
    FILE_WRITE_NO_SPACE
} FileWriteStatus;

typedef enum {
    FILE_DELETE_SUCCESS,
    FILE_DELETE_DOESNT_EXIST,
} FileDeleteStatus;

#endif