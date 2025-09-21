/*
 * Kevin Scrivnor
 * Copyright (C) 2025
 * COMP 362 - Operating Systems
 *
 * Creates a hierarchy of processes and waits for them to exit
 *
 * Modified by: Jose Morales Hilario
 */

// TODO: add your includes! (DONE?)
#include <sys/types.h> //defines pid_t
#include <unistd.h> //provides functions that use pid_t (like fork(), getpid(), getppid(), etc.)
#include <stdio.h>
#include <stdlib.h>

pid_t
Fork(void)
{
    // TODO: call fork() and exit() on error, otherwise return pid from fork() (DONE?)

    pid_t pid;

    //fork creates a new process by duping the calling one
    pid = fork(); //fork returns a process ID
    if(pid < 0){ //error checking if our fork call didn't return an error, 0 means child process, positive integer means parent process
          ferror("fork error"); //print error
          exit(EXIT_FAILURE);
    
    } 

    return pid;
}

int
main(int argc, char** argv)
{
    pid_t pid;
    const int children = 2;

    // TODO: create a nested if-elseif-else structure calling Fork() and exec() to create the process hierarchy
    //execlp(PATH,argv[0],argcv[1], ..., NULL); -> you give it a path, then runs the program with the agrv[0] and holds the rest of the arguments(argv[1],argv[2],...). 
    //proc_hier -> 1.1, 1.2
    //1.1 -> 1.1.1, 1.1.2
    //1.2 -> 1.2.1, 1.2.2

    pid = Fork(); //resulted two process, parent(1) and c1(1.1)

    if(pid == 0){ //1.1
        pid = Fork(); //resulted in two processes, 1.1 and 1.1.1
        if(pid == 0){//checking if child processes, 1.1.1
            execlp("./iam", "iam", "1.1.1", NULL);

        } else { //1.1
            pid = Fork(); //two processes, 1.1 and 1.1.2

            if(pid == 0){ //if child process, 1.1.2
                execlp("./iam", "iam", "1.1.2", NULL);

            } else { // 1.1
                execlp("./iam", "iam", "1.1", NULL);

            }
        }
    
    } else { //1
        pid = Fork(); //two processes, 1 and 1.2

        if(pid == 0){//checking if child processes, 1.2
            pid = Fork(); //two processes, 1.2 and 1.2.1

            if(pid == 0){ //if child, 1.2.1
                execlp("./iam", "iam", "1.2.1", NULL);

            } else { //1.2
                pid = Fork(); //two processes, 1.2 and 1.2.2
                if(pid == 0){ //if child, 1.2.2
                    execlp("./iam", "iam", "1.2.2", NULL);
                } else { //1.2
                    execlp("./iam", "iam", "1.2", NULL);
                }
            }
        }
    }
       
     
	// waits for the 2 children 1.1 and 1.2
    for(int i = 0; i < children; i++) {
        int ret;
        pid_t pid;
        pid = wait(&ret);
        if(WEXITSTATUS(ret) == 0) {
            printf("Child process %d has exited normally\n", pid);
        } else {
            printf("Child process %d has exited abnormally\n", pid);
        }
    }

    printf("All children have exited, root process terminating\n");

    exit(EXIT_SUCCESS);
}