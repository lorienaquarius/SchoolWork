#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define NUM_CORES 4
#define NUM_PROCESSES 3

#define MAX(X, Y) (X > Y ? X:Y)
#define MIN(X, Y) (X > Y ? Y:X)

/*
* TODO
* + type of scheduling needed on process (buffer) level
* + sort processes by type of scheduling used
* + allocate thread functions by scheduling type, and implement scheduling
* FIFO is run until completion
* Round robin only runs processes for time slice, then goes back in ready queue if not done
* Normal uses multiple levels of priority queues. If process doesn't finish in time slice,
* put them into next level queue, up to 3 queues
* Each priority queue has a different time slice  
*/



typedef struct buffer {
    pid_t pid;
    int sp;
    int dp;
    long int remain_time;
    long int time_slice;
    long int accu_time_slice;
    pthread_t thread_id;
} buffer;

int threads_finished = 0;
int produced = 0;
buffer buffers[NUM_CORES][NUM_PROCESSES];
void *producer(void *arg){

    printf("Producer is producing data...\n");

    int count = 0;

    for(int i = 0; i < NUM_CORES; i++){
        for(int j = 0; j < NUM_PROCESSES; j++){
            buffers[i][j].pid = count;
            buffers[i][j].sp = 120;
            buffers[i][j].dp = 120;
            buffers[i][j].remain_time = (rand()%(21 - 5) + 5) * 1000; //put in millis
            buffers[i][j].time_slice = 100;
            buffers[i][j].accu_time_slice = 0;
            count++;
        }
    }
    produced = 1;

}

void *consumer(void *arg){
    buffer *processes = (buffer *)arg;
    buffer current;
    int bonus = rand()%11;
    for(int i = 0; i < NUM_PROCESSES; i++){
        current = processes[i];
        printf("Current process information:\nCalling process PID: %d\nStatic Priority: %d\nDynamic Priority: %d\nRemaining execution time: %dms\nTime slice: %dus\nAccumulated time slice: %dms\nThread ID: %d\n\n",
                current.pid, current.sp, current.dp, current.remain_time, current.time_slice, current.accu_time_slice, current.thread_id);
        current.dp = MAX(100, MIN(current.sp - bonus + 5, 139));
        current.time_slice = current.sp < 120 ? ((140 - current.dp)*20000):((140 - current.dp)*5000);
        usleep(current.time_slice); 
        current.accu_time_slice += current.time_slice / 1000;
        current.remain_time -= current.time_slice / 1000;
        printf("Current process information after processing:\nCalling process PID: %d\nStatic Priority: %d\nDynamic Priority: %d\nRemaining execution time: %dms\nTime slice: %dms\nAccumulated time slice: %dms\nThread ID: %d\n\n",
                current.pid, current.sp, current.dp, current.remain_time, current.time_slice/1000, current.accu_time_slice, current.thread_id);
    }
    threads_finished++;
}


int main(){
    srand(time(NULL));
    int res;
    pthread_t threads[NUM_CORES];
    pthread_attr_t attribute;
    
    struct sched_param scheduling_value;
    int max_prio;
    int min_prio;

    res = pthread_attr_init(&attribute);
    if(res != 0){
        perror("could not create attributes\n");
    }

    res = pthread_attr_setdetachstate(&attribute, PTHREAD_CREATE_DETACHED);
    if(res != 0){
        perror("could not set detached\n");
    }
    res = pthread_attr_setschedpolicy(&attribute, SCHED_OTHER);
    if(res != 0){
        perror("could not set scheduling policy\n");
    }

    max_prio = sched_get_priority_max(SCHED_OTHER);
    min_prio = sched_get_priority_min(SCHED_OTHER);
    scheduling_value.sched_priority = min_prio;

    res = pthread_attr_setschedparam(&attribute, &scheduling_value);
    
    if(res != 0){
        perror("could not set scheduling parameters\n");
    }
    pthread_t producer_thread;

    res = pthread_create(&producer_thread, &attribute, producer, NULL);

    scheduling_value.sched_priority = max_prio;
    res = pthread_attr_setschedparam(&attribute, &scheduling_value);


    while(!produced){
        usleep(1000);
    }

    for(int i = 0; i < NUM_CORES; i++){
        res = pthread_create(&threads[i], &attribute, consumer, (void *)buffers[i]);
        if(res != 0){
            perror("Could not create thread\n");
            exit(EXIT_FAILURE);
            
        } else {
            for(int j = 0; j < NUM_PROCESSES; j++){
                buffers[i][j].thread_id=threads[i];
            }
        }
    }
    while(threads_finished < NUM_CORES){};
    exit(EXIT_SUCCESS);

}