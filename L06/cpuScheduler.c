/*
* AJ Bieszczad
* Kevin Scrivnor
* Copyright (C) 2025
* COMP 362 - Operating Systems
*
* Modified by: Jose Morales Hilario
*/

#include "cpuScheduler.h"

int
main(int argc, char **argv)
{
    // simulation parameters
    ALGORITHM_PARAMS parameters = {
            .cpu = NULL, .algorithm = "",
            .step = NULL, .quantum = 0,
            .time = 0
    };

    // read the algorithm type and time quantum if necessary
    scanf("%s", parameters.algorithm);

    //check which algorithm was passed, set values accordingly
    // recall function pointers from the activity
    if (strcmp(parameters.algorithm, "RR") == 0) {
        // RR has one extra input, the quantum
        scanf("%d", &parameters.quantum);
        parameters.step = &rrStep;
    } else if (strcmp(parameters.algorithm, "FCFS") == 0) {
        parameters.step = &fcfsStep;
    } else if (strcmp(parameters.algorithm, "SJF") == 0) {
        parameters.step = &sjfStep;
    } else if (strcmp(parameters.algorithm, "SRTF") == 0) {
        parameters.step = &srtfStep;
    }else {
        printf("The job type input is not a valid input!");
        exit(EXIT_FAILURE);
    }

    scanf("\n"); // skip over the end of line marker

    printf("\nALGORITHM: %s", parameters.algorithm);
    if (strcmp(parameters.algorithm, "RR") == 0) {
        printf("%3d", parameters.quantum);
    }
    printf("\n\n");

    createProcessTable(INITIAL_CAPACITY);
    createReadyQueue(INITIAL_CAPACITY);

    readProcessTable(); //populate global process table
    displayProcessTable();

    printf("SIMULATION:\n\n");

    // the current time tick
    int time = 0;
    while (processesLeftToExecute()) { // any processes left to execute?, true = loop, false = stop loop
        // check if a process is arriving, add it to ready queue if so
        addArrivingProcessesToReadyQueue(time);

        // set the time to the current time tick, changes starting on the second loop iteration
        parameters.time = time;

        // perform the logic of the algorithm for the given time tick
        doStep(parameters.step, &parameters);

        // print the line like in the activity
        displayTimeTick(time, parameters.cpu);

        // process gets time on the CPU (does work)
        if (parameters.cpu != NULL) {
            parameters.cpu->burstTime--;
            
        }

        // and onto the next time tick
        time++;
    }

    printAverageWaitTime();
    cleanUp();

    exit(EXIT_SUCCESS);
}

/***
 * step executor
 */
void
doStep(void (*func)(void *), void *param)
{
    func(param);
}

/***
 * function implementing a step of FCFS
 */
void
fcfsStep(void *param)
{
    ALGORITHM_PARAMS *p = (ALGORITHM_PARAMS *) param;

    //if the cpu has nothing currently executing
    if (p->cpu== NULL || p->cpu->burstTime == 0) { //if cpu has nothing in it OR the process in CPU has finished
        p->cpu = fetchFirstProcessFromReadyQueue(); //start executing the first process in the ready queue
        if (p->cpu != NULL) { //Only happens if a process 'is' fetched from the above function
            p->cpu->waitTime = p->time - p->cpu->entryTime; // update the wait time (waitTime = timeTick - process in cpu entryTime)
        }
    }
    //Otherwise, keep doing the process in the CPU until done.
}

/***
 * function implementing a step of SJF
 */
void
sjfStep(void *param) //shortest job first
{
    ALGORITHM_PARAMS *p = (ALGORITHM_PARAMS *) param;
    if (p->cpu== NULL || p->cpu->burstTime == 0) { //if cpu has nothing in it OR the process in CPU has finished

        p->cpu = findShortestProcessInReadyQueue(); //start executing the first process in the ready queue, this process we find stays in CPU until finished
        if (p->cpu != NULL) { //Only happens if a process 'is' fetched from the above function
            p->cpu->waitTime = p->time - p->cpu->entryTime; // update the wait time (waitTime = timeTick - process in cpu entryTime)
        }
    }
    //Otherwise, keep doing the process in the CPU until done.
}

/***
 * function implementing a step of SRTF
 */
void
srtfStep(void *param) //shortest remaining time first
{
    ALGORITHM_PARAMS *p = (ALGORITHM_PARAMS *) param;
    PROCESS* currShortP = findShortestProcessInReadyQueue();
    bool changeCPU = false;
    


    if(p->cpu == NULL || p->cpu->burstTime == 0 || p->cpu->burstTime > currShortP->burstTime){ //if cpu is empty  OR  process in CPU is done  OR  the process in CPU no longer has the shortest burst time
        if(p->cpu != NULL && p->cpu->burstTime > 0){ //only happens if there was a process in the CPU AND it still has a burstTime greater than 0
            changeCPU = true; //process in progress in CPU is being booted off and will be replace with a shorter Process 
        }

        if(changeCPU == true){ //if there was a shorter process detected,
            p->cpu->offTime = p->time; //save the time tick that the current process in cpu got kicked off at
            addProcessToReadyQueue(p->cpu); //add the pointer of kicked-off process back to readyQueue
            p->cpu = currShortP; //change the cpu pointer to the shorter process
        } else { //if the current process is done OR if the cpu currently has nothing in it.
             p->cpu = currShortP;
        }
        
        if (p->cpu != NULL) { //Only happens if a process 'is' fetched from the above function
            if(p->cpu->offTime > 0){ //only happens if the process was previously on the CPU but was kicked off, this is done thru checking the waitTime since it is given to the Process when on the CPU for 1st time
                p->cpu->waitTime += (p->time - p->cpu->offTime); //add the time that were off of the cpu to waitTime of the current Process in CPU
                p->cpu->offTime = p->time + 1;
            }else{ //only happens if this is the current process's first time on the CPU
                p->cpu->waitTime = p->time - p->cpu->entryTime;//give the wait time when they first got on the CPU (waitTime = timeTick - process in cpu entryTime)
            }   
        }
    } else { //happens if the current process in the CPU is still the shortest and isn't done yet
        if(p->cpu != currShortP){
            addProcessToReadyQueue(currShortP); //add it back to readyQueue because getting a process from the functions removes them from the readyQueue
        }
    }
}

/***
 * function implementing a step of RR
 * parameters/p already holds the quantum value within the struct
 */
void
rrStep(void *param) //round robin
{
    ALGORITHM_PARAMS *p = (ALGORITHM_PARAMS *) param;
    static int quantum = 0; // counter to keep track of how many more executions it has left, this is only intialized once


    
    if(p->cpu == NULL || quantum == p->quantum || p->cpu->burstTime == 0){ //if cpu is pointing to no process  OR  quantum step is greater than quantum timer OR cpu's burstTime is 0
        if(p->cpu == NULL){ //if there is no process in the cpu, should only happen in the beginning of the program
            p->cpu = fetchFirstProcessFromReadyQueue();
            quantum = 0; //update the value so we do the correct amount of iterations
        } else if(p->cpu->burstTime == 0){ //if there process has finished AKA burstTime is 0
            p->cpu = fetchFirstProcessFromReadyQueue();
            quantum = 0; //update the value so we do the correct amount of iterations
            
        } else if(quantum == p->quantum){ //if the quantumCounter reached the condition,
            
            PROCESS* returnP = p->cpu; //save pointer to temp pointer
            returnP->offTime = p->time; //save the current time tick for future update of waitTime when process is put back on the CPU

            p->cpu = fetchFirstProcessFromReadyQueue(); //get the process pointer at the front of readyQueue and change the cpu pointer to this new process

            addProcessToReadyQueue(returnP); //return unfinished pointer to process to back of readyQueue
            
            quantum = 0; //update the value so we do the correct amount of iterations
        }

        if (p->cpu != NULL) { //Only happens if a process 'is' fetched from the above function
            if(p->cpu->offTime > 0){ //only happens if the process was previously on the CPU but was kicked off, this is done thru checking the waitTime since it is given to the Process when on the CPU for 1st time
                p->cpu->waitTime += (p->time - p->cpu->offTime); //add the time that were off of the cpu to waitTime of the current Process in CPU
                p->cpu->offTime = p->time + 1;
            }else{ //only happens if this is the current process's first time on the CPU
                p->cpu->waitTime = p->time - p->cpu->entryTime;//give the wait time when they first got on the CPU (waitTime = timeTick - process in cpu entryTime)
            }   
        }

    }

    
    quantum++;
}

/***
 * fills the process table with processes from input
 */
int
readProcessTable()
{
    PROCESS tempProcess = {
            .name = "",
            .entryTime = 0,
            .burstTime = 0,
            .offTime = 0,
            .waitTime = 0
    };

    char *line = NULL;
    char *currPos;
    size_t len = 0;

    int counter = 0;
    int offset = 0;

    while (getline(&line, &len, stdin) != -1) {
        currPos = line;
        sscanf(currPos, "%s%n", tempProcess.name, &offset);
        currPos += offset;
        sscanf(currPos, "%d%n", &tempProcess.entryTime, &offset);
        tempProcess.offTime = tempProcess.entryTime; // simplifies computation of the wait time
        currPos += offset;
        sscanf(currPos, "%d", &tempProcess.burstTime);

        addProcessToTable(tempProcess);

        counter++;
    }

    free(line);

    return counter;
}

void
displayTimeTick(int time, PROCESS *cpu)
{
    printf("T%d:\nCPU: ", time);
    if (cpu == NULL) {
        printf("<idle>\n");
    } else {
        printf("%s(%d)\n", cpu->name, cpu->burstTime);
    }

    displayQueue();
    printf("\n\n");
}