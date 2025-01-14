#ifndef FS_CONSTANTS
#define FS_CONSTANTS

constexpr int BLOCK_SIZE = 16;

constexpr int MEMORY_SIZE = 4096;

constexpr int NUM_BLOCKS = MEMORY_SIZE / BLOCK_SIZE;

constexpr int NUM_FILES = NUM_BLOCKS;

constexpr int FREE_BLOCK = -1;

#endif