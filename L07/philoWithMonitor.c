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

enum { THINKING, HUNGRY, EATING }; //Giving the number of States a meaning for philos, 0 = THINKING, 1 = HUNGRY, 2 = EATING, this is done so we can know clearly see what state they are in
// we now use 0 for thinking or just write THINKING and it is taken as 0 for the machine but it gives meaning to the reader of this code
int numOfSeats, numOfTurns;

int* philo;// pointer to array of each philosophers' state, will be dynamically allocated via calloc with 'numOfSeats'
                //EX: philo* = [EATING, THINKING, HUNGRY, THINKING, EATING]
                //EX: philo[0]'s value is EATING
                //EX: changing a philo's state is : philo[4] = THINKING, it is now in the thinking state

//for threads, they are either doing something or waiting to be signaled 
pthread_cond_t* self; // pointer to the threads conditions of the threads, will dynamically allocated via malloc with 'numOfSeats'
 
pthread_mutex_t lock; //global lock for philos uninitialized until start of main, will be set and changed throughout the program

int pickUp(int id){
    while(pthread_mutex_trylock(&lock) == EBUSY){ //trys to lock when we enter this function, will only not loop when the lock is not in use
        //if locks fails, we keep trying until we lock it
    }

    philo[id] = HUNGRY;//switch to hungry state
    test(id);//test(i) for both neighbors to see if they are eating
    while(philo[id] != EATING){
        pthread_cond_wait(&self[id], &lock); //puts philo's thread to sleep and unlocks the lock, will lock it again when woken up
    }
    printf("Philosopher no. %d picked up the chopsticks and will now eat.\n", id);
    usleep(SLEEP_TIME);
    pthread_mutex_unlock(&lock);//unlock when we leave this function
}

int test(int id){

    if(philo[id] == HUNGRY && //if philo is hungry AND
        philo[(id + numOfSeats - 1) % numOfSeats] != EATING && //if left neighbor isn't eating AND
        philo[(id + 1) % numOfSeats] != EATING){ //if right neighbor isn't eating
            philo[id] = EATING;//switch to eating
            pthread_cond_signal(&self[id]); //wake up philo's thread so it can continue in the pickUp function if failed previously
    }
    
}   

int putDown(int id){
    //lock so we can change states
    while(pthread_mutex_trylock(&lock) == EBUSY){}//returns EBUSY when lock is in use, will break out of loop once we have locked it sucessfully

    philo[id] = THINKING; //change to thinking since we are done eating, aka put chopsticks down
    printf("Philosopher no. %d has put down the chopsticks and is now THINKING again.\n", id);
    test((id + numOfSeats - 1) % numOfSeats);//then test() left neighbor, neighbor before me 
    test((id + 1) % numOfSeats);//and test() right neighbor so they can wake up and eat, neighbor after me
    pthread_mutex_unlock(&lock);//unlock so others can change states now
}


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

    pthread_attr_t attr;//create thread attrivute
    pthread_attr_init(&attr); //initialize the thread attribute
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE); //set the attributes of the threads to default, basically don't think bout it

    
    pthread_t philosopher_tid[numOfSeats]; //threads for each philo
    pthread_mutex_init(&lock, NULL); //initialize global mutex lock for philo to use in functions
    self = malloc(numOfSeats * sizeof(pthread_cond_t)); //create an array of thread condition states for the threads and have the global pointer point to this array
    philo = calloc(numOfSeats, sizeof(int));//calloc sets every value to zero, global philo pointer now points to this array of ints


    printf("Start a dinner for %d diners\n", numOfSeats);
    long i;
    for (i = 0; i < numOfSeats; i++) { //number of philosophers
        pthread_create(&philosopher_tid[i], NULL, philosopher, (void*) i); //create threads/philos, then after main program continues on its own
    }

    for (i = 0; i < numOfSeats; i++) {
        pthread_join(philosopher_tid[i], NULL); //main program now waits for all threads to finish/join back until continuing with the program
    }

    printf("Dinner is no more.\n");


    pthread_mutex_destroy(&lock);  //destroy at the end of the program
    exit(EXIT_SUCCESS);

}

void*
philosopher(void* num)
{
    int id = (long) num; //philo's id, turn argument back to correct datatype

    usleep(SLEEP_TIME); //random time for philo to arrive

    printf("Philosopher no. %d sits at the table.\n", id);

    for(int i = 0; i < numOfTurns; i++){ //number of times via numOfTurns
        printf("Philosopher no. %d gets hungry for the %d time!\n", id, i + 1);
        pickUp(id); //try to get both chopsticks
        putDown(id); //put both chopsticks back down
        usleep(SLEEP_TIME * 1);
    }
    printf(">>>>>> Philosopher no. %d FINISHED! <<<<<<\n", id);
    pthread_exit(NULL);
}