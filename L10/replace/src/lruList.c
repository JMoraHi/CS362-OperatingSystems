/**
 * This implements the LRU page-replacement algorithm.
 * Modified by: Jose Morales Hilario
 */

#include "../inc/lruList.h"

int* referenceString;
int refStringLength;

FRAME* pageTableTop; //The FRAME is at the start of the list, usually the one that was added recently
FRAME* leastRecentlyUsed; //L.R.U. is usually the first one added
int pageTableSize = 0;

int numberOfFramesPerProcess;

// statistics
int nummberOfFaults = 0;

int hitPageNumber;

/*
 * insert pages from a reference string into a page table and measure
 * the page fault rate
 */
int 
testLRU(int numOfFrames, int* refString, int refStrLen)//numOfFrames, array, sizeOfArray
{
    numberOfFramesPerProcess = numOfFrames; //max number of process
    referenceString = refString;
    refStringLength = refStrLen;

    pageTableTop = NULL;
    leastRecentlyUsed = NULL;


    for(int i = 0; i < refStrLen; i++){
        FRAME* search = searchLRU(referenceString[i]); //send number to find in linked list, return pointer to it or NULL

        if(search == NULL){
            hitPageNumber = (referenceString[i]) + 1; // for if-statement check
            insertLRU(referenceString[i]);
        } else if(search->pageNumber == referenceString[i]){
            hitPageNumber = referenceString[i];
            insertLRU(referenceString[i]);
        }
        
        displayLRU();
    }


    freePageTableLRU();
    return nummberOfFaults;
}

/*
 *	try to insert a page into the page table
 */
void 
insertLRU(int pageNumber)
{
    if(hitPageNumber != pageNumber){ //if we don't hit a page
        FRAME* newNode = malloc(sizeof(FRAME));
        newNode->up = NULL;
        newNode->down = NULL;
        newNode->pageNumber = pageNumber;

        
        if(pageTableSize == 0){ //when we don't have any pages
            pageTableTop = newNode;
            leastRecentlyUsed = newNode;
            pageTableSize++;
            nummberOfFaults++;
        } else if(pageTableSize < numberOfFramesPerProcess){ //when list isn't fully filled
            newNode->down = pageTableTop; //pointer to go down the list from newNode
            pageTableTop->up = newNode; //pointer to go up list from current pageTableTop
            pageTableTop = newNode; // newNode is now the pageTableTop
            pageTableSize++;
            nummberOfFaults++;
        } else { //adding to a full list

            FRAME* tobeFreed = leastRecentlyUsed; //keep pointer
            leastRecentlyUsed = leastRecentlyUsed->up; //move LRU pointer up to list since the above page is now the LRU
            leastRecentlyUsed->down = NULL;
            free(tobeFreed);

            newNode->down = pageTableTop; //pointer to go down the list from newNode
            pageTableTop->up = newNode; //pointer to go up list from current pageTableTop
            pageTableTop = newNode; // newNode is now the pageTableTop
            nummberOfFaults++;
        }
    } else {

        int loc = 0;
        FRAME* temp = pageTableTop;
        for(int i = 0; i < pageTableSize; i++){ //go thru list and find pageNumber's location in the list to see if it's first, in the middle, or last
            if(temp->pageNumber == hitPageNumber){
                loc = (i+1); 
                break; //found the page
            }
            temp = temp->down; //keeping going down the list  
        }


        if(loc == 1){//beginning
            //don't change anything
            
        } else if (loc < pageTableSize){ // middle, works for 2nd or 3rd spot, even if there is only 3
            pageTableTop->down = temp->up;
            pageTableTop->up = temp;

            temp->down->up = temp->up;
            temp->up->down = temp->down;
            temp->down = pageTableTop;

            pageTableTop = temp;
            pageTableTop->up = NULL;

        } else { //end/last node of the list, if even if there is only 2
            leastRecentlyUsed = leastRecentlyUsed->up;
            leastRecentlyUsed->down = NULL;

            pageTableTop->up = temp;

            temp->down = pageTableTop;

            pageTableTop = temp;
            pageTableTop->up = NULL;
        }
    }
}

/**
 * Searches for page pageNumber in the page frame list
 * returns NULL if a frame with pageNumber was not found
 * otherwise, returns a reference to the frame with pageNumber
 */
FRAME* 
searchLRU(int pageNumber)
{
    FRAME* temp = pageTableTop;

    for(int i = 0; i < pageTableSize; i++){
        if(temp->pageNumber == pageNumber){
                break; //found the page
        }

        temp = temp->down; //continue down the list
    }

    return temp;
}

void 
displayLRU()
{
    FRAME* temp = pageTableTop;


    printf("%d ->    ", pageTableTop->pageNumber);
    if(pageTableTop->pageNumber != hitPageNumber){
        

        for(int i = 0; i < pageTableSize; i++){
            printf("%d", temp->pageNumber);


            if((i+1) == pageTableSize){
                printf("*");
                break;
            } else {
                printf("    ");
            }
            temp = temp->down;
        }
        
        printf("\n");

    } else {
        for(int i = 0; i < pageTableSize; i++){
            printf("%d", temp->pageNumber);


            if(i == 0){
                printf("<   ");
            } else {
                printf("    ");
            }
            temp = temp->down;
        }
        
        printf("\n");



    }
}

void 
freePageTableLRU()
{
    FRAME* toBeFreed;
    FRAME* curr = pageTableTop;

    for(int i = 0; i < pageTableSize; i++){
        toBeFreed = curr;
        curr = curr->down;
        free(toBeFreed);
    }

    return;
}
