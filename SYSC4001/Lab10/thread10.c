#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define NUM_CORES 4
#define NUM_PROCESSES 3
#define MAX(X, Y) (X > Y ? X:Y)
#define MIN(X, Y) (X > Y ? Y:X)
typedef struct buffer {
    pid_t pid;
    int sp;
    int dp;
    int remain_time;
    int time_slice;
    int accu_time_slice;
    pthread_t thread_id;
} buffer;

int threads_finished = 0;

void *consumer(void *arg){
    buffer *processes = (buffer *)arg;
    buffer current;
    int bonus = rand()%11;
    for(int i = 0; i < NUM_PROCESSES; i++){
        current = processes[i];
        printf("Current process information:\nCalling process PID: %d\nStatic Priority: %d\nDynamic Priority: %d\nRemaining execution time: %d\nTime slice: %d\nAccumulated time slice: %d\nThread ID: %d\n",
                current.pid, current.sp, current.dp, current.time_slice, current.accu_time_slice, current.thread_id);
        current.dp = MAX(100, MIN(current.sp - bonus + 5, 139));
        current.sp = current.dp;
        current.time_slice = current.sp < 120 ? ((140 - current.sp)*20000):((140 - current.sp)*5000);
        usleep(current.time_slice); 
        current.accu_time_slice += current.time_slice;
        current.remain_time -= current.time_slice;
        printf("Current process information after processing:\nCalling process PID: %d\nStatic Priority: %d\nDynamic Priority: %d\nRemaining execution time: %d\nTime slice: %d\nAccumulated time slice: %d\nThread ID: %d\n",
                current.pid, current.sp, current.dp, current.time_slice, current.accu_time_slice, current.thread_id);
    }
    threads_finished++;
}


int main(){
    srand(time(NULL));
    int res;
    pthread_t threads[NUM_CORES];
    pthread_attr_t attribute;
    buffer buffers[NUM_CORES][NUM_PROCESSES];
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
    

    for(int i = 0; i < NUM_CORES; i++){
        for(int j = 0; j < NUM_PROCESSES; j++){
            buffers[i][j].pid = getpid();
            buffers[i][j].sp = 120;
            buffers[i][j].dp = 0;
            buffers[i][j].remain_time = rand()%(21 - 5) + 5;
            buffers[i][j].time_slice = 100;
            buffers[i][j].accu_time_slice = 0;
            
        }
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