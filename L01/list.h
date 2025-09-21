/*
CS 362- Operating Systems by Kevin Scrivnor
Edited by Jose Morales Hilario



*/
#pragma once

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define MAX_SIZE_OF_DATA 32
#define NUM_OF_NODES 5

typedef struct list {
   char* data;
   struct list* next;
} LIST;

void allocate_list(LIST**, int);

void load_list(LIST*);

void print_list(LIST*);

void free_list_emb(LIST*);
void free_list_tail(LIST*);
void free_list_iter(LIST*);
