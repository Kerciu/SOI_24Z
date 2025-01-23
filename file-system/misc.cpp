#include "fs.h"


bool FileSystem::duplicateName(const std::string& name)
{
    for (int i = 0; i < NUM_FILES; ++i) {
        if (fd_table[i].name == name) {
            return true;
        }
    }
    return false;
}

int FileSystem::countFreeBlocks()
{
    int free_blocks = 0;
    for (int i = 0; i < NUM_BLOCKS; ++i) {
        if (fat[i] == FREE_BLOCK) {
            ++free_blocks;
        }
    }
    return free_blocks;
}

int FileSystem::firstFreeBlock()
{
    for (int i = 0; i < NUM_BLOCKS; ++i) {
        if (fat[i] == FREE_BLOCK) {
            return i;
        }
    }
    return -1;
}

void FileSystem::markBlockAsUsed(int block_idx)
{
    fat[block_idx] = END_OF_CHAIN;
}

int FileSystem::findFileIndexByName(const std::string& name)
{
    for (int i = 0; i < file_count; ++i) {
        if (fd_table[i].name == name) {
            return i;
        }
    }

    return -1;
}

int FileSystem::findOpenFileIndexByName(const std::string& name)
{
    for (int i = 0; i < NUM_FILES; ++i) {
        if (open_file_fd_table[i].idx != NO_OPENED_FILE) {
            int file_index = open_file_fd_table[i].idx;
            if (file_index >= 0 && file_index < file_count && fd_table[file_index].name == name) {
                return i;
            }
        }
    }
    return -1;
}

int FileSystem::findOpenFileFreeSlot(int file_idx)
{
    for (int i = 0; i < NUM_FILES; ++ i)
    {
        if (open_file_fd_table[i].idx == NO_OPENED_FILE)
            return i;
    }

    return -1;
}