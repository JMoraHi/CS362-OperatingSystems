#include "./inc/inverted.h"

// "hidden" symbols (i.e., not included in the .h file)
// Modified by Jose Morales Hilario
int tableSize;
int frameSize;
int numOfPages;

int lookUp(PROC *, int, int);
int findOldest(PROC *table);

/*
 * initializes the table for the given sizes of the memory and frame
 * all entries should be zeroed (e.g., calloc())
 */
void initInverted(PROC **table, int msize, int fsize)
{
    frameSize = fsize; //page size is fsize
    tableSize = msize; //total memory is msize
    numOfPages = msize/fsize; //give resulted num, like 16/5 = 3
    *table = calloc(numOfPages,sizeof(PROC)); //make an array of X amount of PROCs and make table point to it
}


/*
 * translate a logical address <pid, page, offset> into a physical address
 * if there is no entry for the pid and the page number in the table, it has to be added
 * if there is no room in the table for the new entry, then the oldest entry has to be removed
 */
long translate(PROC *table, int pid, int page, int offset)
{
    
    struct timespec time;
    clock_gettime(CLOCK_REALTIME, &time);
    time_t timeStamp = time.tv_sec * 1000000000 + time.tv_nsec; //make a time stamp


    int index = lookUp(table, pid, page);
    if(index == -1){ //index was not found 
        index = findOldest(table); //will return oldest or first empty slot
        table[index].page = page;
        table[index].pid = pid;
        table[index].timeStamp = timeStamp;
    } else {
        table[index].timeStamp = timeStamp; //update time slot
    }

    long pAdd = (index * frameSize) + offset; // translate logical → physical

    return pAdd;
}
//
// find the entry for a given pid and page
//
int lookUp(PROC *table, int pid, int page)
{
    
    for(int i = 0; i < numOfPages; i++){
        if(table[i].pid == pid && table[i].page == page){
            
            return i; //return index
        }
    }

    return -1; //failed to find the index
}

//
// find the oldest entry (or not used that has the timestamp == 0)
//
int findOldest(PROC *table)
{
    int oldestIndex = 0;
    PROC oldest = table[0];
    if(oldest.timeStamp == 0) return oldestIndex; //return the first unused slot


    for(int i = 1; i < numOfPages;i++){

        if(table[i].timeStamp == 0) return i; //free slot, return immediately


        if(table[i].timeStamp < oldest.timeStamp){ //comparing timeStamps
            oldest = table[i];
            oldestIndex = i;
            
        }
    }
    
    return oldestIndex;
}


/***
 * prints the inverted table
*/
void printInverted(PROC *table)
{
    printf("-------------------------------------\n");
    printf("        INVERTED TABLE CONTENT\n");
    printf("-------------------------------------\n");
    printf("| PID | PAGE |       TIMESTAMP\n");
    printf("-------------------------------------\n");
    for (int i = 0; i < tableSize; i++)
        printf("| %3d | %4d | %20ld |\n", table[i].pid, table[i].page, table[i].timeStamp);
}
