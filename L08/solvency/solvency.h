#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_LINE 24

typedef struct {
    int n; //bank->n is number of processes
    int m; //bank->m is number of elements
    int* available; //available array
    int** allocation; //2d array with each row/arr being a process's current allocated resources
    int** max; //2d array with each row/arr being a process's max allowed resources
    int** need; //2d array with each row/arr being a process's needed/wanted/desired allocated resources
} BANK;

void arrSub(int*, int *, int);
void arrAdd(int*, int *, int);
bool arrAllTrue(bool*, int);
bool arrLte(int*, int*, int);
void arrSetAllFalse(bool*, int);
void arrCopy(int*, int**, int);
void arrPrint(int*, int);
void arr2dPrint(int**, int, int);
bool isSafeState(BANK*, int*, int);
void request(BANK*);
void release(BANK*);