#pragma once

#include "replace.h"

// the page frame list
typedef struct frame {
    int pageNumber;
    struct frame* up;
    struct frame* down;
} FRAME;

void insertLRU(int);
FRAME* searchLRU(int);
void displayLRU(void);
void freePageTableLRU(void);

