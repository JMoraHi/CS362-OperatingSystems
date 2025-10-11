/*
* AJ Bieszczad
* Kevin Scrivnor
* Copyright (C) 2025
* COMP 362 - Operating Systems
*
* Modified by: Jose Morales Hilario 
*/

#include "processQueue.h"

PROCESS* processTable; //singular pointer that will point to the array of processes
int processTableSize = 0;// how many are in use
int processTableCapacity; //max amount 

PROCESS** readyQueue; //singular pointer pointer that will point the array of pointers that will point to their specific process(these pointers are separate from the process table pointer)
int readyQueueSize = 0;
int readyQueueCapacity; //max amount

// constructor of the process table
void
createProcessTable(int capacity)
{
    processTable = malloc(capacity * sizeof(PROCESS)); //create space for an array of PROCESS structs and now processTable points to that array of Structs
    processTableCapacity = capacity; //make an int for the max amount of structs we have
}

// constructor of the ready queue
void
createReadyQueue(int capacity)
{
    readyQueue =  malloc(capacity * sizeof(PROCESS*)); //create space for an array of PROCESS struct pointer and readyQueue, which is a pointer pointer, points to that array
    readyQueueCapacity = capacity;
}

//adds a process and expands the table if necessary
void
addProcessToTable(PROCESS process)
{
    if (processTableSize >= processTableCapacity) //if array too small
    {
        processTableCapacity *= 2; //double capacity
        processTable = realloc(processTable, processTableCapacity * sizeof(PROCESS)); //make the array of PROCESS pointers twice as big
    }
    processTable[processTableSize++] = process; //make the pointer in the array point to the process and then after that, increment the table size so we know how many process pointers point to a process
    //EX: we have two pointers that already point to their processes, so the processTableSize is 2, so we start at that index after these pointers.
}

void
displayProcessTable()
{
    printf("PROCESSES:\n\nName    \tEntry\tBurst\n");
    for (int i = 0; i < processTableSize; i++)
    {
        printf("%-8s\t%3d   \t%3d   \n", processTable[i].name, processTable[i].entryTime, processTable[i].burstTime);
    }
    printf("\n");
}

//adds any processes that arrives at the current time to the fetchProcessFromReadyQueue
void
addArrivingProcessesToReadyQueue(int time)//EX: Time tick is 2
{
    for(int i = 0; i < processTableSize; i++){ //loop through your processTable
        if(processTable[i].entryTime == time){ //check if the process at that index entry time is the same as the argument's time
            addProcessToReadyQueue(&processTable[i]); //if so, give address of that process which results in a pointer that points ONLY to that process for the function we are calling
        }
    }
}

//will determine if any processes in the process queue have more to execute
//check for if there are any process in the table, if so, return true.
//assuming this is the start of the loop from main, we should have all processes currently in the array
bool
processesLeftToExecute()
{
    for(int i = 0; i < processTableSize;i++){
        if(processTable[i].burstTime > 0){
            return true;
        }
    }
    return false;
}


// adds a pointer and expands the ready queue if necessary
void
addProcessToReadyQueue(PROCESS* pointer) //this pointer points to their specified process in the array already
{
    if (readyQueueSize >= readyQueueCapacity) //if array too small
    {
        readyQueueCapacity *= 2; //double capacity
        readyQueue = realloc(readyQueue, readyQueueCapacity * sizeof(PROCESS*)); //make the array of PROCESS pointer pointers twice as big
    }

    
    readyQueue[readyQueueSize++] = pointer; //make the pointer pointer in the array point to the PROCESS pointer and then after that, increment the readyQueue size so we know how many PROCESS pointer pointers point to a PROCESS pointer
    //EX: Two pointers in the array rn, PointerOne points to P1 only and PointerTwo point to P2 only, all while processTable, which is a separate pointer, points to the array of Processes.
}

// removes a process from the ready queue and shifts all lower processes up (fills the gap)
void
removeProcessFromReadyQueue(PROCESS* p) //we are given the pointer to remove from the ReadyQueue
{
    for(int i = 0; i < readyQueueSize; i++){
        if(readyQueue[i] == p){//check if the pointer at i is the same as the argument pointer
            memmove(&readyQueue[i], &readyQueue[i + 1], (readyQueueSize - (i+1)) * sizeof(PROCESS*));//memmove(indexToRemove,WhereToStartSoWeKeepIt,NumberOfBytesToMove), this is smart to know to keep the elements in front of the indexToRemove        
            //we did readyQueue -
            readyQueueSize--; //decrement the size of readyQueue
            return;
        }
    }
}

/***
 * fetches the first process from the ready queue
 * also removes from readyQueue, call remove function
 */
PROCESS*
fetchFirstProcessFromReadyQueue()
{
    PROCESS* p = readyQueue[0]; //save pointer that points to process
    removeProcessFromReadyQueue(p); //remove pointer from readyQueue
    return p; //return pointer from beginning of readyQueue
}

//finds the shortest job in the ready queue and returns its location in the Queue
PROCESS*
findShortestProcessInReadyQueue()
{
    PROCESS* shortP = readyQueue[0]; //start with the first process in the queue for comparisons

    for(int i = 1; i < readyQueueSize; i++){
        if(shortP->burstTime > readyQueue[i]->burstTime){ //if the current process's burst time is greater than the process's burst time at index 'i'
            shortP = readyQueue[i]; //if so, the pointer at i that points to the process is now the shortest process
        }
    }

    removeProcessFromReadyQueue(shortP); //remove the pointer to the shortestProcess from readyQueue
    return shortP; 
}

// displays the contents of the ready queue
void
displayQueue()
{
    int i;
    printf("QUEUE: ");

    if (readyQueueSize <= 0) {
        printf("<empty>");
    } else {
        for (i = 0; i < readyQueueSize; i++) {
            printf("%s(%d) ", readyQueue[i]->name, readyQueue[i]->burstTime);
        }
    }
}

//calculates the average wait time using information in the proc_queue
//and prints the value.
void
printAverageWaitTime()
{
    int i = 0;
    double sum = 0;
    for (i = 0; i < processTableSize; i++) {
        sum = sum + processTable[i].waitTime;
        printf("Process %s Wait Time: %.2lf\n", processTable[i].name, (double) processTable[i].waitTime);
    }
    printf("Average Wait Time: %.2lf\n", (sum / (double) processTableSize));
}

// clean-up the table and the queue
void
cleanUp()
{
    free(processTable);
    free(readyQueue);
}