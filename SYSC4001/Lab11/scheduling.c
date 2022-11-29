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
* + type of scheduleuling needed on process (buffer) level
* + sort processes by type of scheduleuling used
* + allocate thread functions by scheduleuling type, and implement scheduleuling
* FIFO is run until completion
* Round robin only runs processes for time slice, then goes back in ready queue if not done
* Normal uses multiple levels of priority queues. If process doesn't finish in time slice,
* put them into next level queue, up to 3 queues
* Each priority queue has a different time slice  
*/

typedef enum sched{
    schedule_RR,
    schedule_NORMAL,
    schedule_FIFO,
} sched;



typedef struct buffer {
    pid_t pid;
    int sp;
    int dp;
    long int remain_time;
    long int time_slice;
    long int accu_time_slice;
    pthread_t thread_id;
    sched schedule;
} buffer;


int threads_finished = 0;
int produced = 0;
buffer fifo[4];
buffer rr[4];
buffer normal[12];
buffer RQ0[20];
buffer RQ1[20];
buffer RQ2[20];
int processes_finished = 0;
int num_normal = 12;
int num_rr = 4;
int num_fifo = 4;

int r0counter = 0;
int r1counter = 0;
int r2counter = 0;

void *producer(void *arg){

    printf("Producer is producing data...\n");

    int count = 0;

    for(int i = 0; i < 20; i++){
        RQ0[i].pid = -1;
        RQ1[i].pid = -1;
        RQ2[i].pid = -1;
    }


    for(int i = 0; i < 4; i++){
        fifo[i].pid = count;
        fifo[i].sp = (rand() % 41) + 30;
        fifo[i].dp = 0;
        fifo[i].remain_time = (rand()%(21 - 5) + 5) * 1000; //put in millis
        fifo[i].time_slice = 100;
        fifo[i].accu_time_slice = 0;
        fifo[i].schedule = schedule_FIFO;

        RQ0[count] = fifo[i];
        count++;
        r0counter++;
    }
    for(int i = 0; i < 4; i++){
        rr[i].pid = count;
        rr[i].sp = rand() % 31 + 20;
        rr[i].dp = 0;
        rr[i].remain_time = (rand()%(21 - 5) + 5) * 1000; //put in millis
        rr[i].time_slice = 100;
        rr[i].accu_time_slice = 0;
        rr[i].schedule = schedule_RR;
        count++;

        RQ0[count] = rr[i];
        r0counter++;
    }


    for(int i = 0; i < 12; i++){
        normal[i].pid = count;
        normal[i].sp = (rand() % 140) + 100;
        normal[i].dp = 0;
        normal[i].remain_time = (rand()%(21 - 5) + 5) * 1000; //put in millis
        normal[i].time_slice = 100;
        normal[i].accu_time_slice = 0;
        normal[i].schedule = schedule_NORMAL;
        RQ1[count] = normal[i];
        r1counter++;
        count++;

    }


    printf("Producer finished\n");
    produced = 1;

}


void *consumer(void *arg){
    pthread_t this_thread = *(pthread_t *)arg;
    while(processes_finished < 20){
        int bonus = rand()%11;
        //Priority Queue 0
        for(int i = 0; i < 20; i++){
            if((RQ0[i].pid == -1) || RQ0[i].remain_time <= 0 || RQ0[i].time_slice == 0){
                continue;
            }
            buffer current = RQ0[i];
            printf("Current process information:\nCalling process PID: %d\nStatic Priority: %d\nDynamic Priority: %d\nRemaining execution time: %dms\nTime slice: %dus\nAccumulated time slice: %dms\nThread ID: %d\n\n",
            current.pid, current.sp, current.dp, current.remain_time, current.time_slice, current.accu_time_slice, this_thread);

            if(current.schedule == schedule_NORMAL){
                RQ0[i].dp = MAX(100, MIN(current.sp - bonus + 5, 139));
                RQ0[i].time_slice = current.sp < 120 ? ((140 - current.dp)*20000):((140 - current.dp)*5000);
                
            } 
            if(current.schedule = schedule_FIFO){
                usleep(current.remain_time);
                RQ0[i].time_slice = current.remain_time;
                RQ0[i].remain_time = 0;
                
            }
            RQ0[i].time_slice = RQ0[i].time_slice <= 0 ? 100:RQ0[i].time_slice;
            usleep(current.time_slice);

            RQ0[i].accu_time_slice += RQ0[i].time_slice;
            RQ0[i].remain_time -= RQ0[i].time_slice;

            current = RQ0[i];

            printf("Current process information after processing:\nCalling process PID: %d\nStatic Priority: %d\nDynamic Priority: %d\nRemaining execution time: %dms\nTime slice: %dms\nAccumulated time slice: %dms\nThread ID: %d\n\n",
                current.pid, current.sp, current.dp, current.remain_time, current.time_slice/1000, current.accu_time_slice, this_thread);

            if(RQ0[i].dp >= 100 && RQ0[i].dp <= 130){
                RQ1[i] = RQ0[i];
                RQ0[i].pid = -1;
            } else if(RQ0[i].dp < 140){
                RQ2[i] = RQ0[i];
                RQ0[i].pid = -1;
            }

            if(RQ0[i].remain_time <= 0){
                processes_finished++;
            }

        }
        //Priority Queue 1
        for(int i = 0; i < 20; i++){
            if((RQ1[i].pid == -1) || RQ1[i].remain_time <= 0 || RQ1[i].time_slice == 0){
                continue;
            }
            buffer current = RQ1[i];
            printf("Current process information:\nCalling process PID: %d\nStatic Priority: %d\nDynamic Priority: %d\nRemaining execution time: %dms\nTime slice: %dus\nAccumulated time slice: %dms\nThread ID: %d\n\n",
            current.pid, current.sp, current.dp, current.remain_time, current.time_slice, current.accu_time_slice, this_thread);

            RQ1[i].dp = MAX(100, MIN(current.sp - bonus + 5, 139));
            RQ1[i].time_slice = current.sp < 120 ? ((140 - current.dp)*20000):((140 - current.dp)*5000);

            RQ1[i].time_slice = RQ1[i].time_slice <= 0 ? 100:RQ1[i].time_slice;
            usleep(RQ1[i].time_slice);

            current = RQ1[i];

            RQ1[i].accu_time_slice += current.time_slice / 1000;
            RQ1[i].remain_time -= current.time_slice / 1000;

            printf("Current process information after processing:\nCalling process PID: %d\nStatic Priority: %d\nDynamic Priority: %d\nRemaining execution time: %dms\nTime slice: %dms\nAccumulated time slice: %dms\nThread ID: %d\n\n",
                current.pid, current.sp, current.dp, current.remain_time, current.time_slice/1000, current.accu_time_slice, this_thread);
            
            if(RQ1[i].remain_time <= 0){
                processes_finished++;
            }

            if(RQ1[i].dp < 100){
                RQ0[i] = RQ1[i];
                RQ1[i].pid = -1;
            } else if(RQ0[i].dp > 130){
                RQ2[i] = RQ1[i];
                RQ1[i].pid = -1;
            }

        }
        for(int i = 0; i < 20; i++){
            if((RQ2[i].pid == -1) || RQ2[i].remain_time <= 0 || RQ2[i].time_slice == 0){
                continue;
            }
            buffer current = RQ2[i];
            printf("Current process information:\nCalling process PID: %d\nStatic Priority: %d\nDynamic Priority: %d\nRemaining execution time: %dms\nTime slice: %dus\nAccumulated time slice: %dms\nThread ID: %d\n\n",
            current.pid, current.sp, current.dp, current.remain_time, current.time_slice, current.accu_time_slice, this_thread);

            RQ2[i].dp = MAX(100, MIN(current.sp - bonus + 5, 139));
            RQ2[i].time_slice = (current.sp < 120) ? ((140 - current.dp)*20000):((140 - current.dp)*5000);

            RQ2[i].time_slice = RQ2[i].time_slice <= 0 ? 100:RQ2[i].time_slice;

            usleep(RQ2[i].time_slice);

            current = RQ2[i];

            RQ2[i].accu_time_slice += current.time_slice / 1000;
            RQ2[i].remain_time -= current.time_slice / 1000;
            
            if(RQ2[i].remain_time <= 0){
                processes_finished++;
            }

            if(RQ2[i].dp < 100){
                RQ0[i] = RQ2[i];
                RQ2[i].pid = -1;
            } else if(RQ0[i].dp < 130){
                RQ1[i] = RQ2[i];
                RQ2[i].pid = -1;
            }
        }
        printf("Processes finished: %d\n", processes_finished);
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
        perror("could not set scheduleuling policy\n");
    }

    max_prio = sched_get_priority_max(SCHED_OTHER);
    min_prio = sched_get_priority_min(SCHED_OTHER);
    scheduling_value.sched_priority = min_prio;

    res = pthread_attr_setschedparam(&attribute, &scheduling_value);
    
    if(res != 0){
        perror("could not set scheduleuling parameters\n");
    }
    pthread_t producer_thread;

    res = pthread_create(&producer_thread, &attribute, producer, NULL);

    scheduling_value.sched_priority = max_prio;
    res = pthread_attr_setschedparam(&attribute, &scheduling_value);


    while(!produced){
        usleep(1000);
    }

    for(int i = 0; i < NUM_CORES; i++){
        res = pthread_create(&threads[i], &attribute, consumer, (void *)&threads[i]);
        if(res != 0){
            perror("Could not create thread\n");
            exit(EXIT_FAILURE);
            
        }
    }
    
    while(threads_finished < 4){usleep(1000);};
    printf("All Threads finished!\n");
    exit(EXIT_SUCCESS);

}