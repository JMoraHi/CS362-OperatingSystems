#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_PROCESSES   24
#define MAX_LINE        256

typedef struct {
    int numOfProcesses;
    char* processes[MAX_PROCESSES];
    int** dependencies;
} GRAPH;

typedef struct path {
    char* process;
    struct path* next;
} PATH;

void loadProcesses(char* input, GRAPH* graph);
int findProcess(char* process, GRAPH* graph);
void loadDependencies(char* input, GRAPH* graph);
void printAdjacencyMatrix(GRAPH* graph);
void printCycle(PATH* path);
void detectDeadlocks(int n, GRAPH* graph, PATH* path);