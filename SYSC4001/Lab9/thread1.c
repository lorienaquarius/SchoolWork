#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void* thread_function(){
    pthread_exit("Exiting thread\n");
}

int main(){


    struct timeval start, end;
    // Find out the elapsed time for creating a new process using fork()
    gettimeofday(&start, NULL); // get the 1st time stamp

    pid_t pid = fork();

    switch(pid){
        case -1:
            perror("could not create child process\n");
            exit(EXIT_FAILURE);
        case 0:
            exit(EXIT_SUCCESS);
        default:
            break;
    }
    gettimeofday(&end, NULL); // get the 2nd time stamp

    printf("pid= %d, Time to create fork: %ld\n", getpid(), ((end.tv_sec * 1000000 +
    end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));
    // Find out the elapsed time for creating a new thread

    pthread_t thread;

    gettimeofday(&start, NULL); // get the 1st time stamp
    int res = pthread_create(&thread, NULL, thread_function, NULL);
    if(res != 0){
        perror("Thread creation failed\n");
    }
    gettimeofday(&end, NULL); // get the 2nd time stamp
    printf("pid= %d, Time to create thread: %ld\n", getpid(), ((end.tv_sec * 1000000 +
    end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec)));

    exit(EXIT_SUCCESS);

}