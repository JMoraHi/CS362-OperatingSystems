#pragma once
#include <unistd.h>

enum exit_codes {
    CI_BLK_SUCCESS,
    CI_BLK_NO_FILE_PROVIDED,   // folder is technically a file
    CI_BLK_CANNOT_STAT,
    CI_BLK_NOT_REGULAR_FILE,   // not used anymore
    CI_BLK_OPEN_FILE,
    CI_BLK_NULL_DEV,
    CI_BLK_INVALID_BLOCK_NUM,
    CI_BLK_NOT_ENOUGH_SPACE,
    CI_BLK_OFFSET,
    CI_BLK_READ_ERROR,
    CI_BLK_NOT_IMPLEMENTED,
    CI_BLK_NO_DISK_ATTRS,     // new errors
    CI_BLK_INVALID_ATTRS,
    CI_BLK_NOT_DIRECTORY
};

// forward definition
// user of library can use ci_blk_dev_t but cannot access members
typedef struct ci_blk_dev ci_blk_dev_t;

// Define a logical block address type (as a long)
typedef long lba_t;

// Define a Cylinder, Track, Sector type to hold the "physical" location of the block
typedef struct cts {
    int cyl;
    int trk;
    int sec;
} cts_t;

typedef struct ci_blk_dev_attrs {
    int block_size; // # of bytes in a sector
    // new features!
    int cylinders;  // # of platters
    int tracks;     // # of tracks per platter
    int sectors;    // # of sectors per track
} ci_blk_dev_attrs_t;

void test_conv_funcs(void); // new testing function

int ci_blk_create(ci_blk_dev_t** dev, const ci_blk_dev_attrs_t* attrs); // two return values now
void ci_blk_destroy(ci_blk_dev_t* dev);

int ci_blk_open(ci_blk_dev_t* dev, const char* path);
int ci_blk_close(ci_blk_dev_t* dev);

int ci_blk_write(ci_blk_dev_t* dev, lba_t block_num, const void* buffer, ssize_t count);
int ci_blk_read(ci_blk_dev_t* dev, lba_t block_num, void* buffer, ssize_t count);

void ci_blk_print_error(int error);