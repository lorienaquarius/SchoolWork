#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#define NUM_THREADS 8
#define SIZE 1000000
void *thread_function(void *arg);

int main() {
    int res;
    pthread_t a_thread[NUM_THREADS];
    void *thread_result;
    int thread_num;

    double* numbers = malloc(sizeof(double) * SIZE);

    struct timeval start, end;
    // Find out the elapsed time for creating a new process using fork()
    gettimeofday(&start, NULL); // get the 1st time stamp

    for(int i = 0; i < SIZE; i++){
        numbers[i] = sqrt(i);
    }

    gettimeofday(&end, NULL); // get the 2nd time stamp

    printf("\n\n");

    printf("pid= %d, Time for single threaded application: %ld\n", getpid(), ((end.tv_sec * 1000000 +
    end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));
    // Find out the elapsed time for creating a new thread
    gettimeofday(&start, NULL); // get the 1st time stamp
    // add pthread_create() to create a thread

    for(thread_num = 0; thread_num < NUM_THREADS; thread_num++) {
        res = pthread_create(&(a_thread[thread_num]), NULL, thread_function, (void *)(numbers + (SIZE/NUM_THREADS * thread_num)));
        if (res != 0) {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }
    }
    printf("Waiting for threads to finish...\n");
    for(thread_num = NUM_THREADS - 1; thread_num >= 0;
    thread_num--) {
        res = pthread_join(a_thread[thread_num], &thread_result);
        if (res == 0) {
            printf("Picked up a thread\n");
        }
        else {
            perror("pthread_join failed");
        }
    }
    gettimeofday(&end, NULL); // get the 2nd time stamp
    printf("pid= %d, Time taken for %d threads: %ld\n", getpid(), NUM_THREADS, ((end.tv_sec * 1000000 +
    end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));  

    
    exit(EXIT_SUCCESS);
}
void *thread_function(void *arg) {
    double* numbers = (double *)arg;
    printf("thread_function is running.\n");

    for(int i = 0; i < SIZE / NUM_THREADS; i++){
        numbers[i] = sqrt(i);
    }

    pthread_exit(NULL);
}