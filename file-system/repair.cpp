#include "fs.h"

void FileSystem::restoreOriginalFileSize()
{
    if (transaction_log.file_idx != -1 && transaction_log.last_valid_block != -1) {
        fd_table[transaction_log.file_idx].size =
            (transaction_log.last_valid_block + 1) * BLOCK_SIZE;
    }
}

void FileSystem::clearNewAllocatedBlocks()
{
    if (transaction_log.first_new_block != -1) {
        int block_idx = transaction_log.first_new_block;
        while (block_idx != -1 && block_idx != END_OF_CHAIN) {
            int next_block = fat[block_idx];
            fat[block_idx] = FREE_BLOCK;
            block_idx = next_block;
        }
    }
}

void FileSystem::clearTransactionLog()
{
    transaction_log.in_progress = false;
    transaction_log.file_idx = -1;
    transaction_log.new_size = 0;
    transaction_log.last_valid_block = -1;
    transaction_log.first_new_block = -1;
}

void FileSystem::transactionRollback()
{
    if (!transaction_log.in_progress)
        return;

    // restore original file size
    restoreOriginalFileSize();

    // clear new allocated blocks
    clearNewAllocatedBlocks();

    // restore end of chain for the last valid block
    if (transaction_log.last_valid_block != -1) {
        fat[transaction_log.last_valid_block] = END_OF_CHAIN;
    }

    clearTransactionLog();
}

void FileSystem::repair()
{
    if (transaction_log.in_progress) {
        std::cout << "Repairing file system...\n";
        std::cout << "Transaction in progress, attempting rollback...\n";

        transactionRollback();
        std::cout << "File system repair completed.\n";
    }
}
