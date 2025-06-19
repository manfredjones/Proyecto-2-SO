#ifndef BWFS_H
#define BWFS_H

#include <stdint.h>

#define BWFS_MAGIC 0xBEEFCAFE
#define MAX_INODES 128
#define MAX_FILENAME 32
#define BLOCK_SIZE 125000
#define MAX_BLOCKS 128

typedef struct {
    uint32_t magic;
    uint32_t total_blocks;
    uint32_t total_inodes;
    uint32_t free_block_bitmap_offset;
    uint32_t inode_table_offset;
    uint32_t data_block_offset;
    char signature[32];
} Superblock;

typedef struct {
    uint8_t valid;
    char filename[MAX_FILENAME];
    uint32_t size;
    uint32_t blocks[MAX_BLOCKS];
} Inode;

extern uint8_t block_bitmap[MAX_BLOCKS];

#endif // BWFS_H