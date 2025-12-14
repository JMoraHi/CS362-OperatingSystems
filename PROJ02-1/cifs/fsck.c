/*
* Kevin Scrivnor
* Copyright (C) 2025
* COMP 362 - Operating Systems
*
* Modified by: Jose Morales Hilario
*
* fsck
* Note, the filesystem volume must be unmounted for a filesystem check
* Reads the bitvecotr, superblock and the rest of the blocks and checks for inconsistencies 
*/

#include "cifs.h"
#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char** argv)
{
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <volume name>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if(cifs_filesystem_check(argv[1]) == CIFS_SUCCESS) {
        printf("fsck passed...\n");
        exit(EXIT_SUCCESS);
    } else {
        fprintf(stderr, "ERROR: fsck failed. See output above for details.\n");
        exit(EXIT_FAILURE);
    }
}
