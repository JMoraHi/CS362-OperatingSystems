/*
* Kevin Scrivnor
* Copyright (C) 2025
* COMP 362 - Operating Systems
*
* Modified by: Jose Morales Hilario
*
* Searches a graph defined by a set of vertices and edges for cycles
* using an adjacency matrix
*/

//strdup = string duplicate
//strsep = string separate
#include "deadlock.h"

int deadlocked = 0;

void
loadProcesses(char* input, GRAPH* graph) //char* processes
{
    char* v = strdup(input); //duplicate the inputted string
    

    if(v[0] != 'V'){
        exit(EXIT_FAILURE); //they inputted it wrong, that's what you get!
    }

    v = &v[1]; //move the char pointer forward from start, EX: &v = "V=....", &v[1] = "=..."
    char *token; //create string pointer
    graph->numOfProcesses = 0; //for index start for processes pointer

    while ((token = strsep(&v, "={,}")) != NULL) {
        if (*token == '\0') continue; //skip empty tokens and does next iteration
        
        graph->processes[graph->numOfProcesses] = token;
        //printf("%s\n", token);
        graph->numOfProcesses++;
    
    }
}

int
findProcess(char* process, GRAPH* graph)
{
    for(int n = 0; n < graph->numOfProcesses; n++) {
        if(strcmp(process, graph->processes[n]) == 0) {
            return n;
        }
    }
    fprintf(stderr, "INPUT ERROR: process %s not found in V\nExiting...\n", process);
    exit(EXIT_FAILURE);
}

void
loadDependencies(char* input, GRAPH* graph)
{
    char* e = strdup(input); //duplicate the inputted string

    graph->dependencies = malloc(graph->numOfProcesses * sizeof(int*)); //create array of int pointers
    for(int i = 0; i < graph->numOfProcesses; i++){
        graph->dependencies[i] = calloc((graph->numOfProcesses)+2, sizeof(int)); //make array of ints for vertice connections and manually add the last two elements for VISITED and PROCESSED for later, calloc so they all are initialized as 0
    }
    
    
    if(e[0] != 'E'){
        exit(EXIT_FAILURE); //they inputted it wrong, that's what you get!
    }

    e = &e[1]; //move the char pointer forward from start, EX: &e = "E=....", &e[1] = "=..."

    //graph->dependencies is a 2D Array(Matrix) that holds the vertices and their connections to other vertices. Example: {A,C} = graph->dep[0][2] = 1
    char *token1; //create string pointer
    char *token2;
    int i, j; //coordinates for matrix for vertices

    while((token1 = strsep(&e, "={,}")) != NULL){ //getting letter
        if (*token1 == '\0') continue; //skip empty tokens  and do next iteration of loop

        i = findProcess(token1,graph); //find the first vertex's row number. EX: Row A would '0' OR Row D would be '3'

        if((token2 = strsep(&e, "={,}")) != NULL){
            j = findProcess(token2, graph); //find the second vertex location within Row i. EX:[i][j], C in Row A would be [0][2] OR A in Row E would be [4][0]
            
            //printf("%s,%s\n",token1,token2);
            graph->dependencies[i][j] = 1;

        } else {
             exit(EXIT_FAILURE); //they inputted it wrong, that's what you get!
        }

    }
}

void
printCycle(PATH* path)
{
    //we have to print starting with the first letter. So we have to go down the pointer until we've reached the last letter
    //EX: C->B->A->NULL, we start at C and we have to print beginning with A so we call printCycle until we reached A, which has path->next as NULL, then we print with A then return until done
    if(path->next != NULL){ 
        printCycle(path->next); //keeping going, method will not keep going until this has returned
    } 
    
    //only prints when we've reached full printCycle
    printf("%s ", path->process); //print current character/process
    return;
}

void
detectDeadlocks(int n, GRAPH* graph, PATH* path) //detectCycles(Letter, Process, path)
{
    graph->dependencies[n][graph->numOfProcesses+1] = 1;//mark as processed, process is last element

    PATH* newPath = malloc(sizeof(PATH));//malloc a new PATH
    newPath->process = graph->processes[n];//set current process to the name
    newPath->next = path;//set the current next to path, will be NULL if this is the first call of detectDeadlocks


    if(graph->dependencies[n][graph->numOfProcesses] == 1){//if already visited,
        printf("Cycle %d detected: ", (deadlocked+1));
        printCycle(newPath);//we cycled so, do printCycle() with newPath so we can print the cycle we've done so far
        printf("\n");
        deadlocked++;

        free(newPath); //free the current Path
        
        return; //we can go back and try a new path from the previous path

    } else { //else, mark as visited
        graph->dependencies[n][(graph->numOfProcesses)] = 1; //now is visited, 'visited' is second-to-last element
    }
    
    
    for(int i = 0; i < graph->numOfProcesses; i++){ //for each process in Row 'n',
        //if there is a 1, meaning if the connection are true like A->C, A->D would be [0][2] = 1 OR [0][3] = 1                                                                   
        if(graph->dependencies[n][i] == 1) detectDeadlocks(i, graph, newPath); //detectDeadlock for that element then it does deadlock() from that row with this currentdeadlock as the path it started from, EX: A->NULL => call deadlock C => C->A->NULL 

    }

    graph->dependencies[n][(graph->numOfProcesses)] = 0;//mark as unvisited
}

int
main(int argc, char** argv)
{
    GRAPH graph;

    // Load V = {...}
    char line[MAX_LINE];
    scanf("%s\n", line);
    loadProcesses(line, &graph);

    // load E = {{,},..}
    scanf("%s", line);
    loadDependencies(line, &graph);

    printAdjacencyMatrix(&graph);

    // Search for deadlocks
    for(int i = 0; i < graph.numOfProcesses; i++) {
        if(graph.dependencies[i][graph.numOfProcesses+1] == 0) {
            detectDeadlocks(i, &graph, NULL);
        }
    }

    if(deadlocked == 0) {
        printf("No deadlocks detected!\n");
    } else {
        printf("\nDeadlocked...\n");
    }

    exit(EXIT_SUCCESS);
}

void
printAdjacencyMatrix(GRAPH* graph)
{
    for(int i = 0; i < graph->numOfProcesses; i++) {
        for(int j = 0; j < graph->numOfProcesses; j++) {
            printf("%2d ", graph->dependencies[i][j]);
        }
        printf("\n");
    }
}