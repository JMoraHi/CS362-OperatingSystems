#pragma once

// Naming pattern for the platters
#define PLATTER "/platter_"
// Make sure our paths don't get too long
#define MAX_PATH 256

struct ci_blk_dev {
    int* fd;            // array of platter file descriptors
    int blocks;
    int block_size;
    int cylinders;      // number of cylinders
    int tracks;         // number of tracks per cylinder
    int sectors;        // number of sectors per track
};

// private functions
static lba_t cts_to_lba(struct ci_blk_dev* dev, cts_t* cts);
static void lba_to_cts(struct ci_blk_dev* dev, lba_t lba, cts_t* cts);

// Moves the head on the device to a cylinder, track, sector
static int ci_blk_move_head(ci_blk_dev_t* dev, cts_t* cts);