/*
 * Kevin Scrivnor
 * Copyright (C) 2025
 * COMP 362 - Operating Systems
 *
 * A process that runs forever until SIGINT signal arrives
 *
 * Modified by: Jose Morales Hilario
 */

/*
 * To stop this process, run the command: kill PID, where PID is the process ID
 * ex: $ kill 1234
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h> //defines pid_t
#include <unistd.h> //provides functions that use pid_t (like fork(), getpid(), getppid(), etc.)
// TODO: add the missing includes!

void
gotsignal(int sig)
{
	// this function is automatically called when SIGINT is caught
    if(sig == SIGINT) {
        fprintf(stderr, "%d: Received SIGINT, terminating\n", getpid());
        exit(EXIT_SUCCESS);
    }
}

int
main(int argc, char** argv)
{
    const int children = 2;

    // catch ctrl-C or kill [pid] signals and exit
    signal(SIGINT, gotsignal);

    if(argc != 2) { //checks for the kill 
        fprintf(stderr, "Not enough or too many arguments\n");
        exit(EXIT_FAILURE);
    }
	// TODO: print the ID (1.1.1, etc) from the command line arguments and the PID of the process
    pid_t pid = getpid();
    printf("I am %s (%d)\n", argv[1], (int)pid); 
	// TODO: Loop and wait for the children if there are any, get wait from children
    
    //make sure it waits for 2 children, do a for-loop
    for(int i = 0; i < 2; i++){ //every process has two children
        int ret; 
        pid_t pid;
        pid = wait(&ret); //waits to receive a pid and checks status of the exit

        if(pid == -1){ //checking if the pid has any children
            
            printf("(%d): I have no children to wait for.\n",(int)getpid());
            goto done; //goes to done line if no children are present
        }

        if(WEXITSTATUS(ret) == 0) {
            printf("Child process %d has exited normally\n", pid);
        } else {
            printf("Child process %d has exited abnormally\n", pid);
        }
    }
    

    

    // sleep forever, only happens after we see have no children. 
    done:
    for(;;) {
        sleep(1);
    }

    exit(EXIT_SUCCESS); // should never be reached
}