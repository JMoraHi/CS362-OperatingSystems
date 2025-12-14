/*
* Kevin Scrivnor
* AJ Bieszczad
* Copyright (C) 2025
* COMP 362 - Operating Systems
*
* Modified by: Jose Morales Hilario
*
* CI File System
* Implements a simple file system
*/

#include "cifs.h"
#include <time.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/stat.h>

struct cifs_context* cifs_context = NULL;
FILE* cifs_volume = NULL;
struct cifs_user* fuser = NULL;

int
cifs_make_filesystem(char* path)
{
    // Open the "block" volume first, no point if no "disk"
    cifs_volume = fopen(path, "w+");
    if(cifs_volume == NULL) {
        return -CIFS_CANNOT_OPEN;
    }

    int ret = CIFS_SUCCESS;

    // Pretend to be a normal user
    fuser = malloc(sizeof(struct cifs_user));
    fuser->uid = 1000;
    fuser->gid = 1000;

    // Initialize the in-memory data structures
    cifs_context = malloc(sizeof(struct cifs_context));
    if(cifs_context == NULL) {
        ret = -CIFS_CANNOT_ALLOC;
        goto mkfs_close_out;
    }

    cifs_context->bitvector = calloc(CIFS_NUMBER_OF_BLOCKS / 8, sizeof(char));
    if(cifs_context->bitvector == NULL) {
        ret = -CIFS_CANNOT_ALLOC;
        goto mkfs_free_context_out;
    }
    for(int i = 0; i < CIFS_SUPERBLOCK_INDEX; i++) {
        cifs_set_bit(cifs_context->bitvector, i);
    }

    cifs_context->superblock = malloc(CIFS_BLOCK_SIZE);
    if(cifs_context->superblock == NULL) {
        ret = -CIFS_CANNOT_ALLOC;
        goto mkfs_free_bv_out;
    }
    cifs_context->superblock->magic[0] = 'S'; cifs_context->superblock->magic[1] = 'P'; cifs_context->superblock->magic[2] = 'R';
    cifs_context->superblock->inode_count = 0;
    cifs_context->superblock->block_size = CIFS_BLOCK_SIZE;
    cifs_context->superblock->num_of_blocks = CIFS_NUMBER_OF_BLOCKS;
    cifs_context->superblock->root_block_inode = CIFS_SUPERBLOCK_INDEX + 1;
    cifs_set_bit(cifs_context->bitvector, CIFS_SUPERBLOCK_INDEX);

    // Create the root folder 
    // Note: the root folder is a special case and many things are hard coded, do not blindly copy
    struct cifs_block root;
    cifs_add_magic(&root);
    root.type = CIFS_FILE_CONTENT_TYPE;
    root.content.inode.id = cifs_context->superblock->inode_count++;
    root.content.inode.file_type = CIFS_FILE_DIRECTORY_TYPE;
    strcpy(root.content.inode.name, "/");
    root.content.inode.size = 0;
    root.content.inode.links = 1;
    root.content.inode.parent_block_inode = cifs_context->superblock->root_block_inode;
    root.content.inode.indirect_block_inode = cifs_context->superblock->root_block_inode + 1;
    root.content.inode.uid = fuser->uid;
    root.content.inode.gid = fuser->gid;
    root.content.inode.mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH; // 755
    time_t curr = time(NULL);
    root.content.inode.atime = curr;
    root.content.inode.ctime = curr;
    root.content.inode.mtime = curr;
    cifs_set_bit(cifs_context->bitvector, cifs_context->superblock->root_block_inode);

    // Create the root folder index block
    struct cifs_block rdx;
    cifs_add_magic(&rdx);
    rdx.type = CIFS_INDEX_CONTENT_TYPE;
    for(int i = 0; i < CIFS_INDEX_SIZE; i++) {
        rdx.content.index[i] = CIFS_INVALID_INDEX;
    }
    cifs_set_bit(cifs_context->bitvector, cifs_context->superblock->root_block_inode + 1);

    // Write the bitvector blocks
    unsigned char block[CIFS_BLOCK_SIZE]; //blocks w
    for(int i = 0; i < CIFS_SUPERBLOCK_INDEX; i++) { //blocks one by one
        for(int j = 0; j < CIFS_BLOCK_SIZE; j++) { //each byte in the block
            block[j] = cifs_context->bitvector[i * CIFS_BLOCK_SIZE + j]; //at N byte, copy bit vector byte into the block
        }
        cifs_write_block(block, i); //write the current bit vector block into volume
    }

    // Write the superblock block
    cifs_write_block((unsigned char*) cifs_context->superblock, CIFS_SUPERBLOCK_INDEX);

    // Write the root folder block
    cifs_write_block((unsigned char*) &root, cifs_context->superblock->root_block_inode);

    // Write the root folder index block
    cifs_write_block((unsigned char*) &rdx, cifs_context->superblock->root_block_inode+1);

    // Expand the volume
    memset(block, 0x00, CIFS_BLOCK_SIZE);
    // Write a block at the end of the file
    for(int i = cifs_context->superblock->root_block_inode+2; i < CIFS_NUMBER_OF_BLOCKS; i++) {
        cifs_write_block(block, i);
    }

    // Sanity check, read root folder and check magic
    struct cifs_block* rblk;
    rblk = (struct cifs_block*) cifs_read_block(cifs_context->superblock->root_block_inode);
    if(cifs_check_magic(rblk) == CIFS_SUCCESS) {
        printf("Magic sanity check passed\n");
    } else {
        printf("Magic sanity check failed...\n");
        ret = -CIFS_BAD_MAGIC;
        goto mkfs_free_sb_out;
    }

    // Some info cause why not?
    printf("CIFS Info:\n");
    printf("Block size: %d\n", cifs_context->superblock->block_size);
    printf("CIFS struct block size: %lu\n", sizeof(struct cifs_block));
    printf("# of blocks: %d\n", cifs_context->superblock->num_of_blocks);

    free(cifs_context->superblock);
    free(cifs_context->bitvector);
    free(cifs_context);
    free(rblk);

    fflush(cifs_volume);
    fclose(cifs_volume);

    return ret;

    mkfs_free_sb_out:
        free(rblk);
        free(cifs_context->superblock);
    mkfs_free_bv_out:
        free(cifs_context->bitvector);
    mkfs_free_context_out:
        free(cifs_context);
    mkfs_close_out:
        fclose(cifs_volume);
        return ret;
}

int
cifs_mount_filesystem(char* path)
{
    if(cifs_volume || cifs_context) {
        return -CIFS_ALREADY_MOUNTED;
    }

    cifs_volume = fopen(path, "rw+");
    if(cifs_volume == NULL) {
        return -CIFS_CANNOT_OPEN;
    }

     // Pretend to be a normal user
    fuser = malloc(sizeof(struct cifs_user));
    fuser->uid = 1000;
    fuser->gid = 1000;

    int ret = CIFS_SUCCESS;

    // intialize the in-memory data structures
    cifs_context = malloc(sizeof(struct cifs_context));
    if(cifs_context == NULL) {
        ret = -CIFS_CANNOT_ALLOC;
        goto mount_close_out;
    }

    cifs_context->bitvector = calloc(CIFS_NUMBER_OF_BLOCKS / 8, sizeof(char));
    if(cifs_context->bitvector == NULL) {
        ret = -CIFS_CANNOT_ALLOC;
        goto mount_free_context_out;
    }
    // TODO: Read the bitvector off of the disk, one block at a time
    // 1 = allocated, 0 = not allocated


    //Assume bit vector is empty
    for(int i = 0; i < CIFS_SUPERBLOCK_INDEX; i++){ //superblock index is right after bit vectors
        unsigned char* blk = cifs_read_block(i); //blk will point to a malloc'ed 256-byte buffer after function call
        for(int j = 0; j < CIFS_BLOCK_SIZE; j++){
            cifs_context->bitvector[i*CIFS_BLOCK_SIZE + j] = blk[j]; //at N byte, copy bit vector byte into the block
        }
        free(blk); //free after to reuse for following malloc'ed blocks
    }  // NOTE: Don't forget to free each malloc() of a block in your loop

   
    cifs_context->superblock = (struct cifs_superblock*) cifs_read_block(CIFS_SUPERBLOCK_INDEX);

    //Check SuperBlock Magic Bytes
    //Sanity check, read root folder and check magic
    if(cifs_context->superblock->magic[0] == 'S' && cifs_context->superblock->magic[1] == 'P' && cifs_context->superblock->magic[2] == 'R'){
        printf("SB Magic sanity check passed\n");
    } else {
        printf("SB Magic sanity check failed...\n");
        ret = -CIFS_BAD_MAGIC;
    }    


    //TODO FINISHED

    return ret;

    mount_free_context_out:
        free(cifs_context);
    mount_close_out:
        fclose(cifs_volume);
        return ret;
}

int
cifs_umount_filesystem(void)
{
    CIFS_REQUIRE_MOUNTED();

    unsigned char block[CIFS_BLOCK_SIZE];
    for(int i = 0; i < CIFS_SUPERBLOCK_INDEX; i++) {
        for(int j = 0; j < CIFS_BLOCK_SIZE; j++) {
            block[j] = cifs_context->bitvector[i * CIFS_BLOCK_SIZE + j];
        }
        cifs_write_block(block, i);
    }

    cifs_write_block((unsigned char*) cifs_context->superblock, CIFS_SUPERBLOCK_INDEX);

    free(cifs_context->superblock);
    free(cifs_context->bitvector);
    free(cifs_context);
    cifs_context = NULL;

    fflush(cifs_volume);
    fclose(cifs_volume);
    cifs_volume = NULL;

    return CIFS_SUCCESS;
}

int
cifs_create_file(char* name, unsigned short type)
{
    //TODO: implement
    if(strlen(name) > CIFS_MAX_NAME_LENGTH){
        return -CIFS_NAME_TOO_LONG;
    }

    // Create the root folder
    struct cifs_block root; //create file, then fill  in parameters correctly
    unsigned short freeBlockIdFolder = cifs_find_free_block(cifs_context->bitvector); //find block to put stuff root inode in
    cifs_set_bit(cifs_context->bitvector, freeBlockIdFolder); //mark the block we got as allocated now
    
    // Create the root folder index block
    struct cifs_block rdx;
    unsigned short freeBlockIdIndex = cifs_find_free_block(cifs_context->bitvector); //find block to put stuff root inode in
    cifs_set_bit(cifs_context->bitvector, freeBlockIdIndex); //mark the block we got as allocated now
    

    //Root Folder Stuff
    cifs_add_magic(&root);
    root.type = CIFS_FILE_CONTENT_TYPE;
    root.content.inode.file_type = type;
    root.content.inode.id = freeBlockIdFolder; //give the root folder the blockId
    strcpy(root.content.inode.name, name); //use 'name' from arguments
    root.content.inode.size = 0; //size of folder
    root.content.inode.links = 1; //Professor said don't worry about it for now
    root.content.inode.parent_block_inode = cifs_context->superblock->root_block_inode; //root folder after sb
    root.content.inode.indirect_block_inode = freeBlockIdIndex; //root index
    root.content.inode.uid = fuser->uid;
    root.content.inode.gid = fuser->gid;
    root.content.inode.mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH; // 755
    time_t curr = time(NULL);
    root.content.inode.atime = curr;
    root.content.inode.ctime = curr;
    root.content.inode.mtime = curr;
    
    
    

    //Root Folder Index Stuff
    cifs_add_magic(&rdx);
    rdx.type = CIFS_INDEX_CONTENT_TYPE;
    for(int i = 0; i < CIFS_INDEX_SIZE; i++){
        rdx.content.index[i] = CIFS_INVALID_INDEX;
    }
    
    //6 Blocks Change
    //Bit Vector
    cifs_context->superblock->inode_count++; //SuperBlock
    

    struct cifs_inode* rootBlockInode = (struct cifs_inode*) cifs_read_block(cifs_context->superblock->root_block_inode);
    rootBlockInode->size++;
    



    unsigned short bitVectorblock = CIFS_BLK_TO_BVBLK(freeBlockIdFolder); //should be 0
    unsigned short bitVectorblock1 = CIFS_BLK_TO_BVBLK(freeBlockIdIndex);
    
    cifs_write_block((unsigned char*) &root, freeBlockIdFolder);//write root folder block into volume
    cifs_write_block((unsigned char*) &rdx, freeBlockIdIndex);//write root folder index block into volume
    cifs_write_block((unsigned char*) &cifs_context->superblock, CIFS_SUPERBLOCK_INDEX); //write changed superblock back into volume
    cifs_write_block((unsigned char*) &rootBlockInode, cifs_context->superblock->root_block_inode); //write changed superblock's inode block into volume


    unsigned char block[CIFS_BLOCK_SIZE];
    for(int j = 0; j < CIFS_BLOCK_SIZE; j++) {
            block[j] = cifs_context->bitvector[bitVectorblock * CIFS_BLOCK_SIZE + j];
    }
    cifs_write_block(block, bitVectorblock); //write changed bit vector block into volume

    for(int j = 0; j < CIFS_BLOCK_SIZE; j++) {
            block[j] = cifs_context->bitvector[bitVectorblock1 * CIFS_BLOCK_SIZE + j];
    }
    cifs_write_block(block, bitVectorblock1); //write changed bit vector block into volume
    
    //65536 blocks are in cifs.vol
    //Each block is 256 bytes = 2048 bits
    //(Bit Vector Only) : Each block has represents 2048 blocks' status of whether they are allocated or not.
    //Each pair of a hex is 1 byte(8 bits) (FF), there are 256 pairs of hexes 
    //32(0-31) blocks are bit vectors that hold data

    return CIFS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// fsck functions
// Reads the bitvecotr, superblock and the rest of the blocks and checks for inconsistencies 
int
cifs_filesystem_check(char* path)
{
    CIFS_REQUIRE_UNMOUNTED();
    int ret = CIFS_SUCCESS;

    // TODO: Implement
    cifs_volume = fopen(path, "r"); //Open volume in read mode only
    if(!cifs_volume){
        return -CIFS_CANNOT_OPEN;
    }

    cifs_context = malloc(sizeof(struct cifs_context)); //malloc context
    if(cifs_context == NULL) {
        ret = -CIFS_CANNOT_ALLOC;
        goto fsck_cleanup;
    }

    cifs_context->bitvector = calloc(CIFS_NUMBER_OF_BLOCKS / 8, sizeof(char)); //calloc bit vector
    if(cifs_context->bitvector == NULL) {
        ret = -CIFS_CANNOT_ALLOC;
        goto fsck_cleanup;
    }
    //fill context's bit vector with vol's bit vector
    for(int i = 0; i < CIFS_SUPERBLOCK_INDEX; i++){ //superblock index is right after bit vectors
        unsigned char* blk = cifs_read_block(i); //blk will point to a malloc'ed 256-byte buffer after function call

        if (!blk) {
            ret = -CIFS_CANNOT_ALLOC;
            fclose(cifs_volume);
            cifs_volume = NULL;
            goto fsck_cleanup;
        }

        for(int j = 0; j < CIFS_BLOCK_SIZE; j++){
            cifs_context->bitvector[i*CIFS_BLOCK_SIZE + j] = blk[j]; //at N byte, copy bit vector byte into the block
        }
        free(blk); //free after to reuse for following malloc'ed blocks
    }  // NOTE: Don't forget to free each malloc() of a block in your loop


    
    cifs_context->superblock = (struct superblock*) cifs_read_block(CIFS_SUPERBLOCK_INDEX); //read from the vol to get sb then cast as a sb struct
    if(cifs_context->superblock == NULL) {
        ret = -CIFS_CANNOT_ALLOC;
        goto fsck_cleanup;
    }


    struct cifs_block* root_inode = (struct cifs_block*) cifs_read_block(cifs_context->superblock->root_block_inode); 
    struct cifs_block* root_index = (struct cifs_block*) cifs_read_block(root_inode->content.inode.indirect_block_inode);

    ret = cifs_fsck_traverse(cifs_context->bitvector, root_index, root_inode->content.inode.size);

    return ret;


    fsck_cleanup:
        free(cifs_context);
        fclose(cifs_volume);
        cifs_volume = NULL;

        return ret;
}

int
cifs_fsck_traverse(unsigned char* bitvector, struct cifs_block* idx, int size) 
{
    int ret = CIFS_SUCCESS;

    // TODO: Implement

    for(int i = 0; i < size; i++){

    }

   


    return ret;
}

int
cifs_fsck_block(unsigned char* bitvector)
{
    int ret = CIFS_SUCCESS;
    
    // TODO: Implement
    // Scan whole bitvector

    return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// general helper functions
void
cifs_add_magic(struct cifs_block* blk)
{
    if(blk) {
        blk->bmagic[0] = 'C'; blk->bmagic[1] = 'I';
        blk->emagic[0] = 'F'; blk->emagic[1] = 'S';
    }
}

int
cifs_check_magic(struct cifs_block* blk)
{
    if(!blk) return -CIFS_NO_BLK;
    if(blk->bmagic[0] == 'C' && blk->bmagic[1] == 'I' && blk->emagic[0] == 'F' && blk->emagic[1] == 'S') {
        return CIFS_SUCCESS;
    } else {
        return -CIFS_BAD_MAGIC;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// bit vector helper functions
void
cifs_set_bit(unsigned char* bitvector, unsigned short bdx)
{
    unsigned short vdx = bdx / 8;      // which 8 byte block in vector
    unsigned short shift = bdx % 8;    // which of the 8 bits to set

    unsigned char mask = 0x80;         // 1000 0000
    /*
        1000 0000 >> shift - move the 1 to the correct bit slot
        OR shifted bit with that the bit vector already as
            keep the bits there, turn the one bit on
    */
    bitvector[vdx] |= (mask >> shift); 
}

unsigned short
cifs_find_free_block(unsigned char* bitvector)
{
    int i = 0;
    while(bitvector[i] == 0xFF && i < CIFS_NUMBER_OF_BLOCKS / 8) i++;    // skip bitvectors that are all 1's
    int j = 0;
    unsigned char mask = 0x80;          // 1000 0000
    while(bitvector[i] & mask) {        // search for the first zero
        mask >>= 1;                     // moves the 1 in the mask one over to the right
        j++;
    }
    return (i * 8) + j;
}

int
cifs_check_bit(unsigned char* bitvector, unsigned short bdx)
{
    unsigned short vdx = bdx / 8;      // which 8 byte block in vector
    unsigned short shift = bdx % 8;    // which of the 8 bits to set

    unsigned char mask = 0x80;
    mask >>= shift;

    if(((bitvector[vdx] & mask) == mask)) {
        return CIFS_SUCCESS;
    } else {
        return -CIFS_BITVECTOR_UNMARKED;   
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// block device functions
size_t
cifs_write_block(const unsigned char* block, short block_num) //block data, num of block to go to
{
    fseek(cifs_volume, block_num * CIFS_BLOCK_SIZE, SEEK_SET); //set cursor to that block num
    size_t wrote = fwrite((const void *) block, sizeof(unsigned char), CIFS_BLOCK_SIZE, cifs_volume); // write block into that block
    return wrote; //returns how bytes were successfully written
}

unsigned char*
cifs_read_block(short block_num)
{
    unsigned char* content = malloc(CIFS_BLOCK_SIZE);

    fseek(cifs_volume, block_num * CIFS_BLOCK_SIZE, SEEK_SET);
    fread((void * restrict) content, sizeof(unsigned char), CIFS_BLOCK_SIZE, cifs_volume);

    return content;
}
