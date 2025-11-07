/**
 * This implements the LRU page-replacement algorithm.
 * Modified by: Jose Morales Hilario
 */

#include "../inc/optArray.h"

int* rString;
int rLength;

// the page frame list
int* pageTable; //list that we will display
int numOfFramesPerProcess; //max number of elements

int victimIndex; //index of the one that will be removed
int hitPage; //index of the page that already exist

// statistics
int numOfFaults = 0;

// this index is used to go through the sequence of pages references
int currentPgeReferenceIndex;

int 
testOPT(int numOfFrames, int* refString, int refStrLen)
{
    rLength = refStrLen;
    rString = refString;
    numOfFramesPerProcess = numOfFrames;
    hitPage = -1;

    pageTable = calloc(numOfFramesPerProcess, sizeof(int));

    for(int i = 0; i < numOfFramesPerProcess; i++){
        pageTable[i] = -1; //set every element as -1 in the beginning
    }

    for(int i = 0; i < rLength; i++){
        currentPgeReferenceIndex = i;
        
        int index = searchOPT(rString[i]); //returns index of an empty, found, or victim pageNumber, if victim is returned, victimIndex holds the index as well.

        insertOPT(index); 

        displayOPT();
    }

    freePageTableOPT();
    return numOfFaults;
}

/*
 *	try to insert a page into the page table
 */
void 
insertOPT(int pageNumberIndex)
{

    if(pageTable[pageNumberIndex] == rString[currentPgeReferenceIndex]){ //no fault, already exist in the page table
        hitPage = pageNumberIndex;
        victimIndex = -1;

    } else { //replacing, either victim or an empty spot
        pageTable[pageNumberIndex] = rString[currentPgeReferenceIndex];
        numOfFaults++;
        victimIndex = pageNumberIndex;
        hitPage = -1;
    }
    
}

/*
 *  find either an empty slot, or the page, or a victim to evict
 */
int 
searchOPT(int pageNumber) //in this method, either it finds the index of number or the index of an empty slot. If not, we call findVictimOpt to find the index of the one to remove
{
    
    for(int i = 0; i < numOfFramesPerProcess; i++){
        if(pageTable[i] == -1){ //empty slot
            victimIndex = i;
            return i;

        }

        if(pageTable[i] == pageNumber){ //found the page in the list
            hitPage = i;
            return i;
        }
    }

    return findVictimPageOPT(); //return the one to replace in the list
}

int 
findVictimPageOPT()
{
    //referenceString(rString)
    // 8 0 3 1 0 3 4 1 6 8 1 0 8 6 4 1, 
    // currentPageReference index is at 8's index
    //pageTable with number in front that is going to be placed in
    //8 | 6 1 0 7*        || 7 is going to be replace here because it appears furthest in the string


    int furthestNumberIndex = 0;

    int found = 0;


    
    for(int i = 0; i < numOfFramesPerProcess; i++){ //go thru table
        for(int j = (currentPgeReferenceIndex + 1); j < rLength; j++){ // go thru string starting after currentPageReferenceIndex
            if(pageTable[i] == rString[j]){ //current page found in the reference string

                if(j > furthestNumberIndex){ // if current index is greater than furthest Number's Index
                    furthestNumberIndex = j; //change it since it is much futher
                    found = 1; //say that we found it in the string
                } else {
                    found = 1; //say that we found it in the string
                }

                break; //break out of current for-loop (j version) since we don't keep going and overwrite the current futherestNumberIndex with a possible further index of the number
            }
        }

        if(found == 0){
            victimIndex = i;
            return victimIndex;
        } else {
            found = 0;
        }

    }

    for(int i = 0; i < numOfFramesPerProcess; i++){ //find index of the number to replace in page Table
        if(pageTable[i] == rString[furthestNumberIndex]){
            victimIndex = i;
        } 
    }
    
    return victimIndex;
}

void 
displayOPT()
{
    printf("%d ->    ", rString[currentPgeReferenceIndex]);


    for(int i = 0; i < numOfFramesPerProcess; i++){
        printf("%d",pageTable[i]);


        if(victimIndex == i){
            printf("*    ");
        } else if(hitPage == i){
            printf("<    ");
        } else {
            printf("     ");
        }

        
    }

    printf("\n");
}

void 
freePageTableOPT()
{
    free(pageTable);
}
