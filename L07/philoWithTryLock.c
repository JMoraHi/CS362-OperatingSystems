/*
 * AJ Bieszczad
 * Kevin Scrivnor
 * Copyright (C) 2025
 * COMP 362 - Operating Systems
 *
 * Modified by Jose Morales Hilario
 * A broken dining philosophers implementation
 * numOfSeats philosophers will try to eat for numOfTurns turns
 */

 //Chopstick being picked up means pthread_mutex_lock()
    //pthread_mutex_lock() returns immediately when the mutex is not already locked
 //pthread_mutex_trylock trys to lock the chopstick they attempt to pick up
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#define SLEEP_TIME ( (rand() % 5 + 1) * 1000)

void* philosopher(void* id);

pthread_mutex_t* chopstick;
int numOfSeats, numOfTurns;

void
gotsignal(int sig)
{
    if(sig == SIGINT) {
        fprintf(stderr, "Interrupt signal. Oh no! Hopefully we didn't deadlock...\n");
        exit(EXIT_SUCCESS);
    }
}

int
main(int argc, char** argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: philosophers <number of seats> <number of turns>\n");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, gotsignal);

    numOfSeats = strtod(argv[1], NULL);
    numOfTurns = strtod(argv[2], NULL);

    chopstick = calloc(numOfSeats, sizeof(pthread_mutex_t));

    // set the seed for random number generator
    srand(time(NULL));

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    printf("Start a dinner for %d diners\n", numOfSeats);

    pthread_t philosopher_tid[numOfSeats];

    long i;
    for (i = 0; i < numOfSeats; i++) {
        pthread_mutex_init(chopstick + i, NULL);
    }

    for (i = 0; i < numOfSeats; i++) {
        pthread_create(&philosopher_tid[i], NULL, philosopher, (void*) i);
    }

    for (i = 0; i < numOfSeats; i++) {
        pthread_join(philosopher_tid[i], NULL);
    }

    for (i = 0; i < numOfSeats; i++) {
        pthread_mutex_destroy(chopstick + i);
    }

    printf("Dinner is no more.\n");

    exit(EXIT_SUCCESS);
}

//pthread_mutex_trylock(&(chopstick[id]));//chopstick 1, returns immediately when mutex is not already locked
//pthread_mutex_trylock(&(chopstick[(id + 1) % numOfSeats])); //chopstick 2, returns immediately when mutex is not already locked
void*
philosopher(void* num)
{
    int id = (long) num;

    printf("Philosopher no. %d sits at the table.\n", id);

    // philosophers arrive at the table at various times
    usleep(SLEEP_TIME);

    int i;
    for (i = 0; i < numOfTurns; i++) {
        printf("Philosopher no. %d gets hungry for the %d time!\n", id, i + 1);


        
        printf("Philosopher no. %d tries to grab chopstick %d\n", id, id);

        while(pthread_mutex_trylock(&(chopstick[id])) != EBUSY){ //try to grab first chopstick
            
            printf("Philosopher no. %d has grabbed chopstick %d\n", id, id); //grabbed first chopstick
            usleep(SLEEP_TIME * 1);
            printf("Philosopher no. %d tries to grab chopstick %d\n", id, (id + 1) % numOfSeats);

            if(pthread_mutex_trylock(&(chopstick[(id + 1) % numOfSeats])) != EBUSY){ //try to grab second chopstick
                printf("Philosopher no. %d grabbed chopstick %d\n", id, (id + 1) % numOfSeats);

            } else{ //if failed, 
                printf("Philosopher no. %d has failed and is putting chopstick %d back down\n", id, id);
                pthread_mutex_unlock(&(chopstick[id])); //put first chopstick back down at the table

                printf("Philosopher no. %d tries to grab chopstick %d\n", id, id);
            }

        }
        

        // YEEEAAAAH !!!
        printf("Philosopher no. %d eating\n", id);

        usleep (SLEEP_TIME);

        printf("Philosopher no. %d stopped eating\n", id);
        pthread_mutex_unlock(&(chopstick[id]));
        printf("Philosopher no. %d has returned chopstick %d\n", id, id);
        pthread_mutex_unlock(&(chopstick[(id + 1) % numOfSeats]));
        printf("Philosopher no. %d has returned chopstick %d\n", id, (id + 1) % numOfSeats);

        printf("Philosopher no. %d finished turn %d\n", id, i + 1);
        usleep (SLEEP_TIME);
    }

    printf(">>>>>> Philosopher no. %d finished meal. <<<<<<\n", id);

    pthread_exit(NULL);
}