/*
* Kevin Scrivnor
* Copyright (C) 2025
* COMP 362 - Operating Systems
* 
* Modified by Jose Morales Hilario
* Simulates a library to open/read/write/close a block device on a character device
*/

#include "ciblk.h"
#include "libciblk.h"
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>

ci_blk_dev_t*
ci_blk_create(ci_blk_dev_attrs_t* attrs) //a
{
    ci_blk_dev_t* defBS = malloc(sizeof(ci_blk_dev_t));
    defBS->block_size = CI_BLK_SIZE;


    if(attrs){ //if attrs is exist, otherwise don't do it
        attrs->block_size = CI_BLK_SIZE;
    }

    return defBS;
}

void 
ci_blk_destroy(ci_blk_dev_t* dev)
{
    if(dev){
        free(dev);
    }
    return;
}

int         //device and path to the .img
ci_blk_open(ci_blk_dev_t* dev, const char* path) //Opens path as the device and sets the device structure appropriately
{
    if(!dev){ //if dev doesn't exist
        return -CI_BLK_NULL_DEV;
    }

    if(!path || !*path){ //if path or what path points to doesn't exist
        return -CI_BLK_NO_FILE_PROVIDED;
    }

    struct stat st; //stat struct
    if(stat(path, &st) == -1){ //if stat method doesn't work
        return -CI_BLK_CANNOT_STAT;
    }

    if(!S_ISREG(st.st_mode)){ //struct is not a regualr file
        return -CI_BLK_NOT_REGULAR_FILE;
    }
    
    //step 2: open and get file descriptor (fd)
    int fd = open(path, O_RDWR);
    if(fd == -1){
        return -CI_BLK_OPEN_FILE;
    }
    dev->fd = fd;

    //st.st_size is the file size in bytes
    //dev->block_size is bytes per block
    //dev->blocks is number of blocks that the file contains
    dev->blocks = (int)((long long)st.st_size/dev->block_size); // (int)((long long)8200/16)

    return CI_BLK_SUCCESS;
}

int
ci_blk_close(ci_blk_dev_t* dev)
{
    if(!dev){
        return -CI_BLK_NULL_DEV;
    }

    if(dev->fd >= 0){ //checking if fd is closed or not. (fd is -1 if it already closed)
        // if not closed, ...
        int closeFile; 
        do{
            closeFile = close(dev->fd); //keep trying to close until closeFile turns into 0(success), gets -1 when it fails
        } while (closeFile == -1 && errno == EINTR);
        
        dev->fd = -1; //change file descriptor to closed
    }

    return CI_BLK_SUCCESS;
}

int //block_num = the starting block number, buffer = array of data bytes to write, count = the size of the buffer array
ci_blk_write(ci_blk_dev_t* dev, int block_num, const void* buffer, ssize_t count)
{
    if(!dev){ //dev doesn't exist
        return -CI_BLK_NULL_DEV;
    }

    if(count == 0){ //rare case where if we get zero bytes
        return CI_BLK_SUCCESS;
    }

    //buffer is the memory containing the data to write
    if(!buffer || count <= 0){  //if buffer is NULL or count is 0 or less
        return -CI_BLK_NOT_ENOUGH_SPACE;
    }

    if(block_num < 0 || block_num >= dev->blocks){ 
        return -CI_BLK_INVALID_BLOCK_NUM;
    } 
    
    long long needed_blocks = ((long long)count + dev->block_size - 1)/ dev->block_size;
    if((long long)block_num + needed_blocks >= dev->blocks){
        return -CI_BLK_NOT_ENOUGH_SPACE;
    }


    long long device_size = (long long)dev->blocks * dev->block_size; //total number of bytes our device can hold
    long long offset = (long long)block_num * dev->block_size; //the byte offset where the write should start at


    //EX: (80) + 126 bytes > 1024
    if(offset + (long long)count > device_size){ //checking if there is enough space
        return -CI_BLK_NOT_ENOUGH_SPACE;
    }

    char* data = (char*)buffer;
    ssize_t remaining = count;

    while(remaining > 0){

        //lseek args: file descriptor, offset(how far to move)
        // Move the file cursor to the correct byte position for this block
        if(lseek(dev->fd, offset, SEEK_SET) == (off_t)-1){ //moves to the correct block/byte before each write, updates fd's internal file offset to where 'offset' states
            return -CI_BLK_OFFSET;
        }

        
        size_t currBlockSize; //the current block size in bytes
        if(remaining < dev->block_size){ //what remains in the block(in bytes), partial block
            currBlockSize = remaining;
        } else {
            currBlockSize = dev->block_size; //the whole block(in bytes), full block
        }

        ssize_t w = write(dev->fd, data, currBlockSize);

        if(w <= 0){
            return -CI_BLK_NOT_ENOUGH_SPACE;
        }


        data += w;          //advance the buffer pointe
        offset += w;        //advance file position
        remaining -= w;     //reduce how much left to write
    }


    return CI_BLK_SUCCESS;
}

int
ci_blk_read(ci_blk_dev_t* dev, int block_num, void* buffer, ssize_t count)
{
    if(!dev){ //dev doesn't exist
        return -CI_BLK_NULL_DEV;
    }

    if(count == 0){ //rare case where if we get zero bytes
        return CI_BLK_SUCCESS;
    }

    //buffer is the memory containing the data to write
    if(!buffer || count <= 0){  //if buffer is NULL or count is 0 or less
        return -CI_BLK_NOT_ENOUGH_SPACE;
    }

    if(block_num < 0 || block_num >= dev->blocks){ 
        return -CI_BLK_INVALID_BLOCK_NUM;
    }   

    long long device_size = (long long)dev->blocks * dev->block_size; //total number of bytes our device can hold
    long long offset = (long long)block_num * dev->block_size; //the byte offset where the write should start at


    //EX: (80) + 126 bytes > 1024
    if(offset + (long long)count > device_size){ //checking if there is enough space
        return -CI_BLK_NOT_ENOUGH_SPACE;
    }

    char* data = (char*)buffer;
    ssize_t remaining = count;

    while(remaining > 0){

        //lseek args: file descriptor, offset(how far to move)
        //Move the file cursor to the correct byte position for this block
        if(lseek(dev->fd, offset, SEEK_SET) == (off_t)-1){ //moves to the correct block/byte before each write, updates fd's internal file offset to where 'offset' states
            return -CI_BLK_OFFSET;
        }

        
        size_t currBlockSize; //the current block size in bytes
        if(remaining < dev->block_size){ //what remains in the block(in bytes), partial block
            currBlockSize = remaining;
        } else {
            currBlockSize = dev->block_size; //the whole block(in bytes), full block
        }

        ssize_t r = read(dev->fd, data, currBlockSize);
        if(r == -1){
            return -CI_BLK_READ_ERROR;
        } else if(r == 0){
            return -CI_BLK_NOT_ENOUGH_SPACE;
        }


        data += r;          //advance the buffer pointe
        offset += r;        //advance file position
        remaining -= r;     //reduce how much left to write
    }


    return CI_BLK_SUCCESS;
}

void
ci_blk_print_error(int error)
{
    switch(error) {
        case CI_BLK_SUCCESS:
            fprintf(stderr, "No error\n");
            break;
        case -CI_BLK_NO_FILE_PROVIDED:
            fprintf(stderr, "Error %d: No file provided\n", error);
            break;
        case -CI_BLK_CANNOT_STAT:
            fprintf(stderr, "Error %d: Cannot stat file\n", error);
            break;
        case -CI_BLK_NOT_REGULAR_FILE:
            fprintf(stderr, "Error %d: File is not a regular file type\n", error);
            break;
        case -CI_BLK_OPEN_FILE:
            fprintf(stderr, "Error %d: Cannot open file\n", error);
            break;
        case -CI_BLK_NULL_DEV:
            fprintf(stderr, "Error %d: Device was not created\n", error);
            break;
        case -CI_BLK_INVALID_BLOCK_NUM:
            fprintf(stderr, "Error %d: Invalid block number\n", error);
            break;
        case -CI_BLK_NOT_ENOUGH_SPACE:
            fprintf(stderr, "Error %d: Not enough space\n", error);
            break;
        case -CI_BLK_OFFSET:
            fprintf(stderr, "Error %d: Cannot seek to offset\n", error);
            break;
        case -CI_BLK_READ_ERROR:
            fprintf(stderr, "Error %d: I/O error with read\n", error);
            break;
        default:
            fprintf(stderr, "Unknown error code %dn", error);
            break;
    }
}
