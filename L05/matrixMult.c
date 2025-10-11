/*
* Kevin Scrivnor
* Copyright (C) 2025
* COMP 362 - Operating Systems
*
* Modified by: Jose Morales Hilario
*
* An efficient matrix multiplication program
*/

#include "matrixMult.h"

int
main(int argc, char** argv)
{
    //CHECK in.txt example for understanding
    //first argument gives are our matrices, 2 by 3 matix and a 2 by 3 
    //then it gives us our first matrix, 2 by 3, with the values in each element
    //then the second matrix, 3 by 2, with the values in each element
    //then we multiply each value from each matrix it will result in a 3 by 3 martix
    //9 different threads for each space in the resulting matrix, 3 x 3 = 9
    //m, k,n ; m * k & k * n

    // two pairs of matrices
    int** a1,** b1,** c1; //first martices, 
    //int** a2,** b2,** c2; //second matrices, if asked to do

    // dimensions of the matices m x k and k x n
    int m1, k1, n1; // m1*k1, k1*n1
    //int m2, k2, n2;

    // the real magic happens in here
    //allocateAndLoadMatrices(&a1, &b1, &c1, &m1, &k1, &n1); //make empty matrices and make them available to put values into elements
    //allocateAndLoadMatrices(&a2, &b2, &c2, &m2, &k2, &n2);
    // TODO: implement
    //multiply(a1, b1, c1, m1, k1, n1);
    //multiply(a2, b2, c2, m2, k2, n2);
    //multiply(int** a, int** b, int** c, int m, int k, int n);


    int counter;
    printf("Time to loop: ");
    if(scanf("%d",&counter) == 0) { //scanf needs an address to know where to put stuff in, it is basically doing the extra step. That's why we don't need to put the '*'
        fprintf(stderr, "Error, didn't get loop count\n");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < counter; i++){
        allocateAndLoadMatrices(&a1, &b1, &c1, &m1, &k1, &n1);
        multiply(a1, b1, c1, m1, k1, n1);

        printf("\nMATRIX A%d\n", (i+1));
        displayMatrix(a1, m1, k1);
        freeMatrix(a1, m1);

        printf("\nMATRIX B%d\n", (i+1));
        displayMatrix(b1, k1, n1);
        freeMatrix(b1, k1);

        printf("\nMATRIX A%d x B%d\n", (i+1),(i+1));
        displayMatrix(c1, m1, n1);
        freeMatrix(c1, m1);
    }






    // display results of matrix multiplication
    /*
    printf("\nMATRIX A1\n");
    displayMatrix(a1, m1, k1);
    freeMatrix(a1, m1);
    printf("\nMATRIX B1\n");
    displayMatrix(b1, k1, n1);
    freeMatrix(b1, k1);
    printf("\nMATRIX A1 x B1\n");
    displayMatrix(c1, m1, n1);
    freeMatrix(c1, m1);

    printf("\nMATRIX A2\n");
    displayMatrix(a2, m2, k2);
    freeMatrix(a2, m2);
    printf("\nMATRIX B2\n");
    displayMatrix(b2, k2, n2);
    freeMatrix(b2, k2);
    printf("\nMATRIX A2 x B2\n");
    displayMatrix(c2, m2, n2);
    freeMatrix(c2, m2);
    */

    exit(EXIT_SUCCESS);
}

void                    //we were given addresses of the variables, we now have pointers
allocateAndLoadMatrices(int*** a, int*** b, int*** c, int* m, int* k, int* n)
{
    // we get our input from STDIN, so either manually or from a file
    if (scanf("%d %d %d", m, k, n) == 0) { //scanf needs an address to know where to put stuff in, it is basically doing the extra step. That's why we don't need to put the '*'
        fprintf(stderr, "Error, cannot read matrix sizes\n");
        exit(EXIT_FAILURE);
    }

    //make array
    //allocate proper space and make 'a' point to array of m * k
    *a = malloc(sizeof(int*) * *m); //rows of 'a' matrix, makes an array of pointers, we need to dereference so we need the star/pointer for 'a' so we make it 'point' to the array
    for(int i = 0; i < *m; i++) (*a)[i] = malloc(sizeof(int) * *k); //pointer -> column pointer at i -> array of that row; EX:  (*a)[0][2] = point to first pointer of the column at 0 and then go to 3 element of that row

    //allocate proper space and make 'b' point to array of k * n
    *b = malloc(sizeof(int*) * *k); //rows of 'b' matrix, makes an array of pointers
    for(int i = 0; i < *k; i++) (*b)[i] = malloc(sizeof(int) * *n); //columns of 'b' matrix, will hold actual values

    //allocate proper space and make 'c' point to array of m*n
    *c = malloc(sizeof(int*) * *m); //rows of 'b' matrix, makes an array of pointers
    for(int i = 0; i < *m; i++) (*c)[i] = malloc(sizeof(int) * *n); //columns of 'b' matrix, will hold actual values



    loadMatrix(a, *m, *k); //pass it an address for future pointer, then the values what m and k point to.
    loadMatrix(b, *k, *n); 
}

void
loadMatrix(int*** matrix, int m, int n) //give each element of the matrix the value to hold
{
    int input = 0;
    for(int i = 0; i < m; i++) {
		for(int j = 0; j < n; j++) {//scanf needs an address to know where to put stuff in, it is basically doing the extra step. That's why we don't need to put the '*'
            if (scanf("%d", &input) == 0) { //receiving current input for the matrix from STDIN and putting the value within the location of input
                fprintf(stderr, "Error, Could not get input correctly for Matrix A/B\n");
                exit(EXIT_FAILURE);
            }
			(*matrix)[i][j] = input; //put our current input into the current element we are currently inru
		}
	}
    
}

void
displayMatrix(int** matrix, int m, int n) //print out the matrix 
{
    // TODO: implement
    for(int i = 0; i < m; i++) {
		for(int j = 0; j < n; j++) {
			printf("%3d", (matrix)[i][j]); //display number that is up to 3 digit length
		}
		printf("\n");
	}
}

pthread_t**
alloc_thread_ids(int m, int n) 
{
    //create 2d array same size as C and holds threadsIDs
    pthread_t** thread_ids;
    thread_ids = malloc(sizeof(pthread_t*) * m);  
    for(int i = 0; i < m; i++) (thread_ids)[i] = malloc(sizeof(pthread_t) * n); //pointer -> column pointer at i -> array of that row;


    return thread_ids;
}

pthread_t**
multiply(int** a, int** b, int** c, int m, int k, int n) // will use pthread_create
{

    //create our cell parameters
    //create all the mult. threads, (mult threads?)
    pthread_t** thread_ids = alloc_thread_ids(m, n); //2d array 




    /*
    
    int r = 0; //result value to hold current dot product
    bool flag = false;
    for(int v1 = 0; v1 < m ; v1++){ //move thru rows of A
        for(int v3 = 0; v3 < n; v3++){ //move thru columns of B
            if(flag == true){ //if we did a dot product,   
                r = 0; //reset value for next element of array, seems right to me
            }
            for(int v2 = 0; v2 < k; v2++){ //gets index of A's x and B's y
                r += ((a)[v1][v2] * (b)[v2][v3]);
                flag = true; //return true, so we can set value and reset 'r'      
            }
        }
    }
    // TODO: implement
    */  

    // create the threads the amount of threads for the C matrix while also changing values within the struct so the matrixThread() can use the value to get dot product of that element
    for (int v1 = 0; v1 < (m); v1++){
        for(int v3 = 0; v3 <(n); v3++){    

            MATRIX_CELL* matrixCell = malloc(sizeof(MATRIX_CELL)); //have a pointer for a MATRIX_CELL datatype point to MATRIX_CELL that we malloc;
            
            matrixCell -> a = a; //pointer to A matrix
            matrixCell -> b = b; //struct's pointer to B
            matrixCell -> c = c; //struct's poiner to C

            matrixCell -> j = v3; 
            
            
            matrixCell -> i = v1;
            matrixCell -> k = k;                                           
		    pthread_create(&thread_ids[v1][v3], NULL, matrixThread, (void *) (matrixCell)); //create a thread, (thread's address, NULL, method, method's ((return type)(argument)))
        }
    }


    join(thread_ids, m, n);

    return thread_ids;
}

void*
matrixThread(void* param) // this is the "runner" start_routine  //pthread_exit
{
    int r = 0; //result value holder
    MATRIX_CELL* matrixCell = (MATRIX_CELL*) param; // convert our param back to a MATRIX_CELL pointer datatype

    for(int v2 = 0; v2 < matrixCell -> k; v2++){ // 
        r += ((matrixCell -> a)[matrixCell -> i][v2] * (matrixCell -> b)[v2][matrixCell -> j]); //result value that holds the dot product
    }

    matrixCell -> c[matrixCell -> i][matrixCell -> j] = r; //put result in appropriate element in C array as well.

    free(matrixCell); //free struct after usage 

    pthread_exit(NULL); //return value 
}

void
join(pthread_t** thread_ids, int m, int n) //pthread_join
{
    //interate over the threadID 2d array, and join each thread
    // wait for the threads to join


	for (int i = 0; i < m; i++) {
        for(int j = 0; j < n; j++){
            pthread_join((thread_ids)[i][j], NULL); //we wait for each thread in the array to finish it process so it can "join" back
        }
	}

    free_thread_ids(thread_ids, m);
    return;
    // TODO: implement
}

void
free_thread_ids(pthread_t** thread_ids, int m) //cleanup
{
    for(int i = 0; i < m; i++)
    {
        free(thread_ids[i]); //go to the thread pointer at i in the array and then free the thread array that the pointer points to

    }

    free(thread_ids); //free the thread pointer array, resulting in an thread pointer pointer that points to nothing
    // TODO: implement
}

void
freeMatrix(int** matrix, int m) //cleanup
{
    for(int i = 0; i < m; i++)
    {
        free(matrix[i]); //go to the int pointer at i in the array and then free the int array that the pointer points to

    }

    free(matrix); //free the int pointer array, resulting in an int pointer pointer that points to nothing

    // TODO: implement
}