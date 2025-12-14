/*
* Kevin Scrivnor
* AJ Bieszczad
* Copyright (C) 2025
* COMP 362 - Operating Systems
*
* Modified by: Jose Morales Hilario
* NOTE: No modifications are required for PROJ02-1. 
* Any modifications to this file will be explained in the PROJ02-*.html files
*/

#pragma once

#include <time.h>
#include <sys/types.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
enum errors {
    CIFS_SUCCESS,
    CIFS_CANNOT_ALLOC,
    CIFS_CANNOT_OPEN,
    CIFS_NO_BLK,
    CIFS_BAD_MAGIC,
    CIFS_NOT_MOUNTED,
    CIFS_ALREADY_MOUNTED,
    CIFS_NAME_TOO_LONG,
    CIFS_INVALID_TYPE,
    CIFS_NOT_UNMOUNTED,
    CIFS_FSCK_ERROR,
    CIFS_BITVECTOR_UNMARKED,
    CIFS_BITVECTOR_CONSISTENCY
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Definition of the sizes for the volumes
*/
#define CIFS_BLOCK_SIZE         256
#define CIFS_NUMBER_OF_BLOCKS   65536
#define CIFS_MAX_NAME_LENGTH    128
#define CIFS_INDEX_SIZE         125
#define CIFS_DATA_SIZE          250
#define CIFS_INVALID_INDEX      65535

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
superblock - meta information about the disk structure
*/
#define CIFS_SUPERBLOCK_INDEX   CIFS_NUMBER_OF_BLOCKS/8/CIFS_BLOCK_SIZE
struct cifs_superblock {
    char magic[3];
    unsigned long inode_count;
    unsigned int num_of_blocks;
    unsigned short block_size;
    unsigned long root_block_inode;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
enum block_type {
    CIFS_INVALID_BLOCK_TYPE,
    CIFS_FILE_CONTENT_TYPE,
    CIFS_INDEX_CONTENT_TYPE//,
    //CIFS_DATA_CONTENT_TYPE
};

enum file_type {
    CIFS_FILE_REGULAR_TYPE,
    CIFS_FILE_DIRECTORY_TYPE
};

/*
File system block types
    - inode blocks
        - file information
    - index blocks
        - inode numbers of other blocks
    - data blocks
        - the data of the file
*/
/*
inode structure
    contains the meta information for a file
*/
struct __attribute__((packed)) cifs_inode {
    unsigned short id;
    unsigned short file_type;
    char name[CIFS_MAX_NAME_LENGTH];
    size_t size;
    unsigned short links;
    unsigned short parent_block_inode;
    unsigned short indirect_block_inode;
    mode_t mode;
    unsigned short uid;
    unsigned short gid;
    time_t atime;
    time_t ctime;
    time_t mtime;
};

// must be 256 bytes
// 4 bytes for magic, 2 for type, 250 left for content
struct __attribute__((packed)) cifs_block {
    char bmagic[2];
    unsigned short type;
    union {
        struct cifs_inode inode;
        unsigned short index[CIFS_INDEX_SIZE];
        char data[CIFS_DATA_SIZE];
    } content;
    char emagic[2];
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* 
context
    in memory data structure for faster access
*/
struct cifs_context {
    struct cifs_superblock* superblock;
    unsigned char* bitvector;
};

/*
user
    pretend to be a user in a group with a process ID
*/
struct cifs_user {
    uid_t uid;
    gid_t gid;
    pid_t pid;
};
extern struct cifs_user* fuser;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////// file system functions
int cifs_make_filesystem(char* path);
int cifs_mount_filesystem(char* path);
int cifs_umount_filesystem(void);
int cifs_create_file(char* name, unsigned short type);
int cifs_filesystem_check(char* path);
int cifs_fsck_traverse(unsigned char* bitvector, struct cifs_block* idx, int size);
int cifs_fsck_block(unsigned char* bitvector);

////////////////// general helper functions
void cifs_add_magic(struct cifs_block* blk);
int cifs_check_magic(struct cifs_block* blk);

////////////////// bit vector helper functions
void cifs_set_bit(unsigned char* bitvector, unsigned short bdx);
unsigned short cifs_find_free_block(unsigned char* bitvector);
int cifs_check_bit(unsigned char* bitvector, unsigned short bdx);

////////////////// block device helper functions
size_t cifs_write_block(const unsigned char* block, short block_num);
unsigned char* cifs_read_block(short block_num);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////// useful macros
#define CIFS_REQUIRE_MOUNTED()                 \
    do {                                       \
        if (!cifs_volume || !cifs_context)     \
            return -CIFS_NOT_MOUNTED;          \
    } while (0)

#define CIFS_REQUIRE_UNMOUNTED()               \
    do {                                       \
        if (cifs_volume || cifs_context)       \
            return -CIFS_NOT_UNMOUNTED;        \
    } while (0)

#define CIFS_BLK_TO_BVBLK(blk) (blk / (CIFS_BLOCK_SIZE * 8))
