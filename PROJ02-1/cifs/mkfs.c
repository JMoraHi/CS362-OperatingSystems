/*
* Kevin Scrivnor
* Copyright (C) 2025
* COMP 362 - Operating Systems
*
* Modified by: Jose Morales Hilario
*
* mkfs
* Intializes a file as a block device and makes a bare cifs file system on the device
* The file system will contain a single root folder
*/

#include "cifs.h"
#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char** argv)
{
    if(argc != 2) { //give it the name 'cifs.vol'
        fprintf(stderr, "Usage: %s <volume name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if(cifs_make_filesystem(argv[1]) == CIFS_SUCCESS) {
        printf("Successfully made the file system at: %s\n", argv[1]);
    } else {
        printf("Something went wrong...\n");
    }

    exit(EXIT_SUCCESS);
}
