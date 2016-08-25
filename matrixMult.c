#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <stdbool.h>


#define M 2000
#define P 500
#define N 2400
int number_threads;


int A [N][M];
int B [M][P];
int C [N][P];
int C1 [N][P];


void *multiply(void *param); /* the thread */
void *compute(int num_threads);
bool checkErrors();

/*main method takes # of threads as an argument and calls compute
 * for each number of threads */
int main(int argc, char *argv[]) {

    //get the number of threads
    if(argc!=2){
        printf("Invalid Arguments");
        exit(-1);
    }
    int num_threads = atoi(argv[1]);

    for(int k = 1; k<=num_threads; k++){
        compute(k);
    }

}

/* This method takes the # of threads as the input and does the matrix multiplication,
 * also keeps track of the time it takes to multiply */
void* compute(int num_threads) {

    //initialize the threads
    pthread_t* threads;
    threads = (pthread_t*) malloc(num_threads*sizeof(pthread_t));

    //initialize matrix A:
    for(int i = 0; i<N; i++){
        for(int j = 0; j< M; j++){
            A[i][j] = j-i+2;
        }

    }
    //initialize matrix B:
    for(int i = 0; i<M; i++){
        for(int j = 0; j<P; j++){
            B[i][j] = i-j+1;
        }
    }

    //have to make global var because I can't pass num_threads onto the multiply function :(
    number_threads = num_threads;


    //start timing
    struct timeval  tv1, tv2;
    gettimeofday(&tv1, NULL);


    // this for loop not entered if thread number is specified as 1
    for (int i = 1; i < num_threads; i++)
    {
        // creates each thread working on its own set of rows
        if (pthread_create (&threads[i], NULL, multiply, (void*)i) != 0 )
        {
            perror("Can't create thread");
            free(threads);
            exit(-1);
        }
    }


    // main thread works on slice 0 (it does all the work if num_thrds = 1
    multiply(0);

    // join the threads
    for (int i = 1; i < num_threads; i++){
        pthread_join (threads[i], NULL);
    }


    //end timing
    gettimeofday(&tv2, NULL);
    printf ("Total time = %f seconds for %d threads\n",
            (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
            (double) (tv2.tv_sec - tv1.tv_sec), num_threads);


    /*check matrix for errors*/
    if (checkErrors()){
        printf("Errors found!\n");
    }else{
        printf("No Errors.\n");
    }



    //Print out the resulting matrix to a file because it is really big
    /*FILE *fp;
    fp = fopen("matrix.txt", "w+");

    for(int i = 0; i < 5; i++) {
        for(int j = 0; j < 4; j++) {
            fprintf(fp,"%d ", C1[i][j]);
        }
        fprintf(fp,"\n");
    }
    fclose(fp); */

    return NULL;
}




/*The thread will begin control in this function*/
void* multiply(void* thread)
{

    int threadNum = (int)thread;   // this is the number of the thread

    //assign each thread its own set of rows, so no conflicts
    //this works even if N mod num_threads != 0, meaning it works for 7 or more threads.
    int fromRow = (threadNum * N)/number_threads;
    int toRow = ((threadNum+1) * N)/number_threads;

    //printf("Thread %d is computing (from row %d to %d)\n", threadNum, fromRow, toRow-1);

    if(number_threads == 1){
        //save the resulting matrix to C1 if there is only 1 thread
        for (int i = fromRow; i < toRow; i++) //i is the row index for matrix C
        {
            for (int j = 0; j < P; j++) // j is the column index for matrix C
            {
                C1[i][j] = 0;
                for (int k = 0; k < M; k++) { //k is the column index for matrix A and the row index for matrix B
                    C1[i][j] += A[i][k] * B[k][j]; //C will be a N*P matrix
                }
            }
        }

    }else{
        //save the resulting matrix to C if there are multiple threads
        for (int i = fromRow; i < toRow; i++) //i is the row index for matrix C
        {
            for (int j = 0; j < P; j++) // j is the column index for matrix C
            {
                C[i][j] = 0;
                for (int k = 0; k < M; k++) { //k is the column index for matrix A and the row index for matrix B
                    C[i][j] += A[i][k] * B[k][j]; //C will be a N*P matrix
                }
            }
        }

    }


    //printf("The following thread %d has finished\n", threadNum);
    return NULL;
}

/*this method checks if matrix C1 = matrix C */
bool checkErrors(){
    bool error = false;

    for (int i = 0; i<5; i++){
        for (int j = 0; j<4; j++){
            if(C1[i][j] != C[i][j]){
                error = true;
                //printf("The entry at C1 is %d but the entry at C is %d\n", C1[i][j], C[i][j]);
            }
        }
    }

    if(number_threads == 1){
        //if there is only 1 thread, only C1 will have values and C will be empty, so return false;
        error = false;
    }

    return error;

}
