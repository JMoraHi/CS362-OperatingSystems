/*
* Kevin Scrivnor
* Copyright (C) 2025
* COMP 362 - Operating Systems
*
* Modified by: Jose Morales Hilario
*
* Simulates the banker's algorithm
* Just like the activity!
*/

#include "solvency.h"

int
main(int argc, char** argv)
{
    int n, m;
    scanf("%d", &n);
    scanf("%d", &m);

    BANK bank;
    bank.n = n;
    bank.m = m;

    bank.available = calloc(m, sizeof(int));
    bank.allocation = malloc(sizeof(int*) * n);
    bank.max = malloc(sizeof(int*) * n);
    bank.need = malloc(sizeof(int*) * n);
    for(int i = 0; i < n; i++) {
        bank.allocation[i] = calloc(m, sizeof(int));
        bank.max[i] = calloc(m, sizeof(int));
        bank.need[i] = calloc(m, sizeof(int));
    }

    for(int i = 0; i < m; i++) {
        scanf("%d", &bank.available[i]);
    }

    for(int i = 0; i < n; i++) {
        for(int j = 0; j < m; j++) {
            scanf("%d", &bank.allocation[i][j]);
        }
        for(int j = 0; j < m; j++) {
            scanf("%d", &bank.max[i][j]);
            bank.need[i][j] = bank.max[i][j] - bank.allocation[i][j];
        }
    }
    getchar();
    char command;
    char line[MAX_LINE];
    do {
        printf("Enter command (h for help): ");
        if(fgets(line, MAX_LINE, stdin)) {
            command = line[0];
        } else { // EOF or something went wrong
            command = 'q';
        }
        switch(command) {
            case 'p':
                printf("Available:  ");
                arrPrint(bank.available, m);
                printf("\nMax:        ");
                arr2dPrint(bank.max, n, m);
                printf("\nAllocation: ");
                arr2dPrint(bank.allocation, n, m);
                printf("\nNeed:       ");
                arr2dPrint(bank.need, n, m);
                printf("\n");
                break;
            case 'r':
                request(&bank);
                break;
            case 'l':
                release(&bank);
                break;
            default:
            case 'h':
                printf("p -- Display state of the system\n");
                printf("r -- Request resources\n");
                printf("l -- Release resources\n");
                printf("q -- Quit\n");
            case 'q': break;
        }
    } while(command != 'q');

    exit(EXIT_SUCCESS);
}

void
release(BANK* bank)
{
    int process;
    int* rel = malloc(sizeof(int) * bank->m);

    printf("Releasing Resource =====\n");
    printf("Process (0-%d): ", bank->n-1);
    scanf("%d", &process);

    if(process < 0 || process >= bank->n) {
        printf("Invalid process\n");
        return;
    }

    printf("Release (%d instances): ", bank->m);
    for(int i = 0; i < bank->m; i++) {
        scanf("%d", &rel[i]);
    }
    getchar(); // eat new line character

    if(!arrLte(rel, bank->allocation[process], bank->m)) {
        printf("Released denied: release > allocation\n");
        return;
    }

    printf("Release has been granted\n");

    // TODO: give the resources back

    //Put Resources back to available
    arrAdd(bank->available, bank->allocation[process], bank->m);
    //Update the current process's allocated resources to be 0 since we gave them back
    arrSub(bank->allocation[process],bank->allocation[process], bank->m);
    
    // Free things you malloc() that you don't need in other functions
    // ie, free things when you're done using them
    free(rel);
}

void
request(BANK* bank) //bank->n is number of processes, bank->m is number of elements 
{
    int process; //process that is requesting
    int* req = malloc(sizeof(int) * bank->m); //this is array for request


    printf("Releasing Resource =====\n");
    printf("Process (0-%d): ", bank->n-1);
    scanf("%d", &process);


    printf("Request (%d instances): ", bank->m);
    for(int i = 0; i < bank->m; i++) {
        scanf("%d", &req[i]);
    }

    //S1: Is Request <= Need && Available?
    for(int i = 0; i < bank->m; i++){
        if(req[i] <= (bank->need)[process][i] && req[i] <= (bank->available)[i]){
            //printf("Need and Available are good. %d \n", i);
        }else{
            printf("Request DENIED!\n");
            free(req);
            exit(EXIT_FAILURE);
        }
    }

    //Pretend to allocate
    arrAdd(bank->allocation[process], request, bank->m);
    //Change available since we now just allocated resources
    arrSub(bank->available, req, bank->m);






    // TODO
}

bool
isSafeState(BANK* bank, int* req, int process)
{

    // TODO: Implement as discussed during the lab
}

/*
 * Array helper functions - shortened formatting for the lab document
 * All functions need dimensions
 *
 * arrSub       a[] = a[] - b[]
 * arrAdd       a[] = a[] + b[]
 * arrLte       check if a[] <= b[]
 * arrCopy      copies a[] into b[], and makes space for b
 * arrSetAllFalse
 * arrAllTrue   check if all a[] filled with true
 * arrPrint
 * arr2dPrint
 */

void arrSub(int* a, int *b, int n) {
    for(int i = 0; i < n; i++)  a[i] = a[i] - b[i];
}

void arrAdd(int* a, int *b, int n) {
    for(int i = 0; i < n; i++) a[i] = a[i] + b[i];
}


void arrSetAllFalse(bool* a, int n) {
    for(int i = 0; i < n; i++) a[i] = false;
}

bool arrAllTrue(bool* a, int n) {
    for(int i = 0; i < n; i++) if(!a[i]) return false;
    return true;
}

bool arrLte(int* a, int* b, int n) {
    for(int i = 0; i < n; i++) if(a[i] > b[i]) return false;
    return true;
}

void arrCopy(int* a, int** b, int n) {
    *b = malloc(sizeof(int) * n);
    for(int i = 0; i < n; i++)  (*b)[i] = a[i];
}

void arrPrint(int* a, int n) {
    printf("[ ");
    for(int i = 0; i < n; i++) printf("%2d ", a[i]);
    printf(" ]");
}

void arr2dPrint(int** a, int n, int m) {
    for(int i = 0; i < n; i++) {
        printf("[ ");
        for(int j = 0; j < m; j++) printf("%2d ", a[i][j]);
        printf(" ]  ");
    }
}