/*
* Kevin Scrivnor
* Copyright (C) 2025
* COMP 362 - Operating Systems
*
* Tests the libciblk device by writing/reading random data
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include "lib/libciblk.h"

#define bytes 1024


char CHAR_SET[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ@_";
#define CHAR_SET_LEN 64

void   /* less garbo this time? */
generate_data(char* buffer, int len)
{
    for(int i = 0; i < len-1; i++) {
        int ind = rand() % CHAR_SET_LEN;
        buffer[i] = CHAR_SET[ind];
    }
    buffer[len-1] = '\0';
}

int
main(int argc, char** argv)
{
    if(argc != 2) {
        fprintf(stderr, "Error, please provide a folder\n\tUsage: %s <foldername>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    
    //test_conv_funcs();
    //END STEP1 

    //STEP 2
    ci_blk_dev_attrs_t attrs = {
        .block_size = 16,
        .cylinders = 4,
        .tracks = 16,
        .sectors = 32
    };
    ci_blk_dev_t* dev = NULL;
    //attrs.cylinders = 0; //Test 1st error return value, uncomment to test

    int result = ci_blk_create(&dev, &attrs);
    printf("=====STEP 2 Error Check====\n");
    ci_blk_print_error(result);

    // result = ci_blk_create(&dev, NULL); //Test 2nd error return value, uncomment to test
    // if(result < 0){
    //     ci_blk_print_error(result);
    // } 
    //END STEP 2 


    //STEP 3
    // TODO: check return value, print errors
    result = ci_blk_open(dev, argv[1]);

    printf("=====STEP 3 Error Check====\n");
    ci_blk_print_error(result);
    //END OF STEP 3


    // STEP 4
    srand((unsigned int) time(NULL));

    int blocks = attrs.cylinders * attrs.tracks * attrs.sectors; // that's like, way more now

    char* data = malloc(attrs.block_size * blocks);
    char* compare = malloc(attrs.block_size * blocks);

    // simulate read and writes
    for(int i = 0; i < 10; i++) {
        // Generate a random data size that is at most an eighth of the disk
        int size = rand() % (attrs.block_size * blocks / 16);
        int data_blocks = size / attrs.block_size;
        if(size % attrs.block_size != 0) data_blocks++;

        // Pick a random starting block within the disk
        int start_block = rand() % blocks;

        // Determine if random data size and starting block will cause an issue or not
        int should_error = false;
        if(start_block + data_blocks > blocks) should_error = true;

        // Generate the random data
        generate_data(data, size);

        printf("===============================================================================\n");
        printf("TRIAL %d\n", i+1);
        printf("Starting Block: %d\tEnding Block: %d\tBlocks: %d\n", start_block, start_block+data_blocks, blocks);
        printf("Data (%d bytes, %d blocks):\n%s\n", size, data_blocks, data);
        int ret = ci_blk_write(dev, start_block, data, size);
        if(should_error) { // write should fail, skip read
            if(ret != CI_BLK_SUCCESS) {
                printf("SUCCESS -- Write fails as expected, skipping Read test\n");
            } else {
                printf("ERROR -- Write expected to fail!\nStopping trials\n");
                exit(EXIT_FAILURE);
            }
        } else { // write supposed to work, check read next
            if(ret == CI_BLK_SUCCESS) {
                ret = ci_blk_read(dev, start_block, compare, size);
                if(ret == CI_BLK_SUCCESS) {
                    printf("Data read:\n%s\n", compare);
                    if(strcmp(data, compare) != 0) { // read works, but data doesn't match
                        printf("ERROR -- Data mismatch\nStopping trials\n");
                        exit(EXIT_FAILURE);
                    } else { // ALL GOOD
                        printf("SUCCESS -- Read data and data matches\n");
                    }
                } else { // read didn't work when it was supposed to
                    printf("ERROR -- Read expected to succeed!\nStopping trials\n");
                    exit(EXIT_FAILURE);
                }
            } else { // write didn't work when it was supposed to
                printf("ERROR -- Write expected to succeed!\nStopping trials\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    free(data);
    free(compare);

    ci_blk_close(dev);
    ci_blk_destroy(dev);
    //END STEP 4 

    exit(EXIT_SUCCESS);
}
