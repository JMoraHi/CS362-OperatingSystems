#pragma once

#define _POSIX_C_SOURCE 200809L // to deal with the crazy "implicit declaration of getline()
// OR #define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct process {
    char name[8];
    int entryTime; // time of entry to the system
    int burstTime; // tick to run
    int offTime; // time when cpu lost a.k.a time tick when you were on the cpu and was kicked off
    int waitTime; // accumulative waiting time
} PROCESS;

// process table functions
void createProcessTable(int capacity);
void addArrivingProcessesToReadyQueue(int time);
void addProcessToTable(PROCESS process);
bool processesLeftToExecute();
void displayProcessTable();

// process ready queue functions
void createReadyQueue(int capacity);
void addProcessToReadyQueue(PROCESS *process);
PROCESS* fetchFirstProcessFromReadyQueue();
PROCESS* fetchProcessFromReadyQueue(int index);
PROCESS* findShortestProcessInReadyQueue();
void removeProcessFromReadyQueue(PROCESS *process);
void displayQueue();
void printAverageWaitTime();
void cleanUp();