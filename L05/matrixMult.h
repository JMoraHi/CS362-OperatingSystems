#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

// parameter struct for thread start_routine/runner function
typedef struct matrixCell {
   int i;
   int j;
   int k;
   int** a; //pointer to a pointer
   int** b;
   int** c;
} MATRIX_CELL;

// function prototypes
void* matrixThread(void*);
void allocateAndLoadMatrices(int***, int***, int***, int*, int*, int*);
void loadMatrix(int***, int, int);
void freeMatrix(int**, int);
pthread_t** alloc_thead_ids(int, int);
void free_thread_ids(pthread_t**, int);
pthread_t** multiply(int**, int**, int**, int, int, int);
void displayMatrix(int**, int, int);
void join(pthread_t**, int, int);