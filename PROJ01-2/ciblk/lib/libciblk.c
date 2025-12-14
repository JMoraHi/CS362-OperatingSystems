/*
* Kevin Scrivnor
* Copyright (C) 2025
* COMP 362 - Operating Systems
* 
* Modified by Jose Morales Hilario
* Simulates a library to open/read/write/close a block device on a character device
*/

#include "libciblk.h" //needs to be first so the compiler see this first
#include "ciblk.h"

#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>

//Define a logical block address type (as a long) : typedef long lba_t;


//a physical disk with spinning platters and a head that moves along the tracks to read sectors.
static int ci_blk_move_head(ci_blk_dev_t* dev, cts_t* cts){ 

}

static lba_t  //cylinder, track, and sector to logical block address
cts_to_lba(struct ci_blk_dev* dev, cts_t* cts){ //This function returns the "head" (file descriptor of the relevant platter) with cursor (offset) repositioned to the right track/sector. 
    lba_t currLBA = (((cts->cyl * dev->tracks) + cts->trk) * dev->sectors) + (cts->sec);
    return currLBA; 
    
}

static void //logical block address to cylinder, track, and sector
lba_to_cts(struct ci_blk_dev* dev, lba_t lba, cts_t* cts){
    lba_t secs_per_cyl = dev->tracks * dev->sectors;

    cts->cyl = lba/(secs_per_cyl);
    cts->trk = (lba % secs_per_cyl)/dev->sectors;
    cts->sec = lba % dev->sectors;  
}


void // new testing function
test_conv_funcs(void){
    struct ci_blk_dev* fakeDev = malloc(sizeof(struct ci_blk_dev)); 
    fakeDev->block_size = 16;
    fakeDev->cylinders = 4; // # of platters(# of pages)
    fakeDev->tracks = 16; // # of tracks per platter(# of rows)
    fakeDev->sectors = 32; // # of sectors per track(# of cells per row)

    cts_t* ctsTest = malloc(sizeof(cts_t));
    lba_t currLBA; 

    ctsTest->cyl = 0;
    ctsTest->trk = 0;
    ctsTest->sec = 0;
    currLBA = cts_to_lba(fakeDev, ctsTest);
    printf("(0,0,0) -> 0\n");

    printf("CTS to LBA: ");
    if(currLBA == 0){
        printf("PASSED\n");
    } else {
        printf("FAILED\n");
    }

    lba_to_cts(fakeDev, currLBA, ctsTest);
    printf("LBA to CTS: ");
    if(ctsTest->cyl == 0 && ctsTest->trk == 0 && ctsTest->sec == 0){
        printf("PASSED\n");
    } else {
        printf("FAILED\n");
    }


    ctsTest->cyl = 0;
    ctsTest->trk = 1;
    ctsTest->sec = 0;
    currLBA = cts_to_lba(fakeDev, ctsTest);
    printf("(0,1,0) -> 32\n");
    
    printf("CTS to LBA: ");
    if(currLBA == 32){
        printf("PASSED\n");
    } else {
        printf("FAILED\n");
    }

    lba_to_cts(fakeDev, currLBA, ctsTest);
    printf("LBA to CTS: ");
    if(ctsTest->cyl == 0 && ctsTest->trk == 1 && ctsTest->sec == 0){
        printf("PASSED\n");
    } else {
        printf("FAILED\n");
    }


    ctsTest->cyl = 0;
    ctsTest->trk = 2;
    ctsTest->sec = 6;
    currLBA = cts_to_lba(fakeDev, ctsTest);
    printf("(0,2,6) -> 70\n");
    
    printf("CTS to LBA: ");
    if(currLBA == 70){
        printf("PASSED\n");
    } else {
        printf("FAILED\n");
    }
    
    lba_to_cts(fakeDev, currLBA, ctsTest);
    printf("LBA to CTS: ");
    if(ctsTest->cyl == 0 && ctsTest->trk == 2 && ctsTest->sec == 6){
        printf("PASSED\n");
    } else {
        printf("FAILED\n");
    }


    ctsTest->cyl = 1;
    ctsTest->trk = 2;
    ctsTest->sec = 3;
    currLBA = cts_to_lba(fakeDev, ctsTest);
    printf("(1,2,3) -> 579\n");
    
    printf("CTS to LBA: ");
    if(currLBA == 579){
        printf("PASSED\n");
    } else {
        printf("FAILED\n");
    }
    
    lba_to_cts(fakeDev, currLBA, ctsTest);
    printf("LBA to CTS: ");
    if(ctsTest->cyl == 1 && ctsTest->trk == 2 && ctsTest->sec == 3){
        printf("PASSED\n");
    } else {
        printf("FAILED\n");
    }
    

} 


int 
ci_blk_create(ci_blk_dev_t** dev, const ci_blk_dev_attrs_t* attrs)
{
    if(!attrs){
        return -CI_BLK_NO_DISK_ATTRS;
    }

    if(attrs->block_size <= 0 || 
        attrs->cylinders <= 0 || 
        attrs->tracks <= 0 || 
        attrs->sectors <= 0){
        return -CI_BLK_INVALID_ATTRS; 
    }

    ci_blk_dev_t* newDev = malloc(sizeof(ci_blk_dev_t));
    newDev->block_size = attrs->block_size;
    newDev->cylinders = attrs->cylinders;
    newDev->tracks = attrs->tracks;
    newDev->sectors = attrs->sectors;

    newDev->fd = NULL;
    *dev = newDev;

    return CI_BLK_SUCCESS;
}

void 
ci_blk_destroy(ci_blk_dev_t* dev)
{
    if(dev){
        free(dev);
    }
    return;
}

int
ci_blk_open(ci_blk_dev_t* dev, const char* path) //Opens path to a folder and sets the device structure appropriately
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

    if(!S_ISDIR(st.st_mode)){ //if it isn't a directory
        return -CI_BLK_NOT_DIRECTORY;
    }

    if(!dev->fd){
        dev->fd = malloc(sizeof(int) * dev->cylinders);
        if(!dev->fd){
            return -CI_BLK_NOT_ENOUGH_SPACE;
        }

        for(int i = 0; i < dev->cylinders; i++){
            dev->fd[i] = -1;
        }
    }

    off_t cyl_bytes = (off_t)dev->tracks * dev->sectors * dev->block_size;

    for(int i = 0; i < dev->cylinders; i++){
        char fullPATH[MAX_PATH];

        // Example: /diskdir + "/platter_" + i  => /diskdir/platter_i
        int n = snprintf(fullPATH, sizeof(fullPATH), "%s%s%d",path,PLATTER,i);

        if(n < 0 || n>= (int)sizeof(fullPATH)){ // If the generated platter file path is invalid or too long, close any previously opened platter files and return an error.
            for(int j = 0; j < i; j++){ 
                if(dev->fd[j] != -1){
                    close(dev->fd[j]);
                    dev->fd[j] = -1;
                }      
            }
            
            free(dev->fd);
            dev->fd = NULL; 

            return -CI_BLK_OPEN_FILE;
        }

        int fd = open(fullPATH, O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR); //0600

        if(fd == -1){ // If opening a platter file fails, close all previously opened platter files then return an error.
            for(int j = 0; j < i; j++){
                if(dev->fd[j] != -1){
                    close(dev->fd[j]);
                    dev->fd[j] = -1;
                }      
            }
            
            free(dev->fd);
            dev->fd = NULL; 

            return -CI_BLK_OPEN_FILE;   
        }
        
        if(ftruncate(fd, cyl_bytes) == -1){ // If resizing the platter file fails, close this file and all previously opened platter files, then return an error.
            close(fd);
            for(int j = 0; j < i; j++){
                if(dev->fd[j] != -1){
                    close(dev->fd[j]);
                    dev->fd[j] = -1;
                }   
            }
            
            free(dev->fd);
            dev->fd = NULL; 

            return -CI_BLK_NOT_ENOUGH_SPACE;
        }

        dev->fd[i] = fd; //set the file descriptor in the array
    }


    dev->blocks = dev->cylinders * dev->tracks * dev->sectors; //set blocks parameter
    return CI_BLK_SUCCESS;
}

int
ci_blk_close(ci_blk_dev_t* dev)
{
    if(!dev){
        return -CI_BLK_NULL_DEV;
    }

    if(!dev->fd){
        return -CI_BLK_NO_FILE_PROVIDED;
    }

    for(int i = 0; i < dev->cylinders; i++){
        if(dev->fd[i] != -1){ // only close valid FDs
            int closeFile; 
            do{
                closeFile = close(dev->fd); //keep trying to close until closeFile turns into 0(success), gets -1 when it fails
            } while (closeFile == -1 && errno == EINTR);
            
            dev->fd = -1; //change file descriptor to closed
        }
    }
    
    free(dev->fd);

    dev->fd = NULL;

    return CI_BLK_SUCCESS;
}

int 
ci_blk_write(ci_blk_dev_t* dev, lba_t block_num, const void* buffer, ssize_t count){
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
ci_blk_read(ci_blk_dev_t* dev, lba_t block_num, void* buffer, ssize_t count){
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
        case -CI_BLK_NO_DISK_ATTRS:
            fprintf(stderr, "Error %d: No Attributes for Disk to use\n", error);
            break;
        case -CI_BLK_INVALID_ATTRS:
            fprintf(stderr, "Error %d: Invalid Attributes for Disk to use\n", error);
            break;
        case -CI_BLK_NOT_DIRECTORY:
            fprintf(stderr, "Error %d: Not a directory\n", error);
            break;
        default:
            fprintf(stderr, "Unknown error code %dn", error);
            break;
    }
}
