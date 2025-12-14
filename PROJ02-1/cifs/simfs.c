/*
* Kevin Scrivnor
* AJ Bieszczad
* Copyright (C) 2025
* COMP 362 - Operating Systems
*
* Modified by: Jose Morales Hilario
*
* CI File System (cifs)
* Tests the file system, assumes the volume is named cifs.vol
*/

#include "cifs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_LINE 256

void create_file(void);

/*
NOTE: Run ./mkfs.cifs cifs.vol first
*/
int
main(int argc, char** argv) {
    fuser = malloc(sizeof(struct cifs_user));
    fuser->uid = 1000;
    fuser->gid = 1000;
    fuser->pid = 19;

    if(cifs_mount_filesystem("cifs.vol") == CIFS_SUCCESS) {
        printf("Successfully mounted the filesystem.\n");
    } else {
        fprintf(stderr, "Something went wrong with mounting the filesystem...\n");
    }

    char line[MAX_LINE];
    int c;
    int ret;
    do {
        printf("=================\nOptions:\n");
        printf("   c - create file\n");
        printf("   m - mount filesystem\n");
        printf("   u - unmount filesystem\n");
        printf("   q - quit\n");
        printf("Enter: ");
        fgets(line, MAX_LINE, stdin);
        c = line[0];
        switch(c) {
        case 'c':
            create_file();
            break;
        case 'm':
            if((ret = cifs_mount_filesystem("cifs.vol")) != CIFS_SUCCESS) {
                fprintf(stderr, "Something went wrong with mounting the filesystem...\n");
            }
            break;
        case 'u':
            if((ret = cifs_umount_filesystem()) != CIFS_SUCCESS) {
                fprintf(stderr, "Something went wrong with unmounting the filesystem...\n");
            }
            break;
        case 'q':
            printf("See yah!\n");
            break;
        default:
            fprintf(stderr, "Unknown command\n");
        }
    } while(c != 'q');

    cifs_umount_filesystem();
    
    exit(EXIT_SUCCESS);
}

void
create_file(void)
{
    char type[3];
    printf("[f]ile or [d]irectory? ");
    fgets(type, 3, stdin);
    if(type[0] != 'f' && type[0] != 'd') {
        fprintf(stderr, "Expected f or d but got %c\n", type[0]);
        return;
    }

    char name[MAX_LINE];
    printf("Enter name: ");
    fgets(name, MAX_LINE, stdin);
    name[strlen(name)-1] = '\0';

    if(type[0] == 'f') {
        cifs_create_file(name, CIFS_FILE_REGULAR_TYPE);
    } else if(type[0] == 'd') {
        cifs_create_file(name, CIFS_FILE_DIRECTORY_TYPE);
    }
}
