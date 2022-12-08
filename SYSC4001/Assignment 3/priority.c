#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define NUM_CORES 4
#define NUM_PROCESSES 5

#define MAX(X, Y) (X > Y ? X:Y)
#define MIN(X, Y) (X > Y ? Y:X)

typedef enum scheduling_policy {
    RR,
    FIFO,
    NORMAL,
}policy;


static const char * const scheduling_types[] = {
    [RR] = "RR",
    [FIFO] = "FIFO",
    [NORMAL] = "NORMAL"
};

typedef struct buffer {
    pid_t pid;
    int sp;
    int dp;
    long int remain_time;
    long int time_slice;
    long int accu_time_slice;
    pthread_t thread_id;
    policy process_policy;
} buffer;

int threads_finished = 0;
int produced = 0;
int total_processes_finished = 0;

buffer buffers[NUM_CORES][NUM_PROCESSES];
void *producer(void *arg){

    printf("Producer is producing data...\n");

    int count = 0;

    for(int i = 0; i < NUM_CORES; i++){
        for(int j = 0; j < NUM_PROCESSES; j++){
            buffers[i][j].pid = count;
            buffers[i][j].remain_time = ((rand()%(21 - 5)) + 5) * 1000; //convert from seconds to milliseconds
            buffers[i][j].time_slice = 10; //100ms
            buffers[i][j].accu_time_slice = 0;
            count++;

            //Set scheduling policy
            switch(j){
                case 0:
                    buffers[i][j].process_policy = FIFO;
                    buffers[i][j].sp = (rand() % 41) + 30; //default SP for FIFO is between 70 and 40
                    buffers[i][j].time_slice = buffers[i][j].remain_time;
                    break;
                case 1:
                    buffers[i][j].process_policy = RR;
                    buffers[i][j].sp = (rand() % 31) + 20; //default sp for RR is between 20 and 50
                    break;
                default:
                    buffers[i][j].process_policy = NORMAL;
                    buffers[i][j].sp = (rand() % 31) + 100; //default sp for NORMAL is between 100 and 130
                    break;
            }
            buffers[i][j].dp = buffers[i][j].sp; //start dynamic priority at static priority
            printf("Produced process with PID %d, sp of %d, scheduling type %s, and remaining time of %dms\n",
                    buffers[i][j].pid, buffers[i][j].sp, scheduling_types[buffers[i][j].process_policy], buffers[i][j].remain_time);
        }
    }
    printf("Data produced! Created %d processes\n\n", count);
    produced = 1;

}

//put in seperate function because it may need to be run again
int run_RQ1(buffer* RQ1, int process_count, buffer* RQ2, int in_RQ2){
    int processes_done = 0;
    //if all processes in RQ1 were run to completion and none sent to RQ2, return 1
    //if at least one process was sent up to RQ2, return 0
    int processes_moved = 0;

    //find highest priority process
    while(processes_done < process_count){

        int lowest_value = 140;
        int prio_index = -1;
        //Get highest priority task
        for(int i = 0; i < process_count; i++){
            if(RQ1[i].dp < lowest_value){
                lowest_value = RQ1[i].dp;
                prio_index = i;
            }
        }

        buffer* current = &RQ1[prio_index];

        //print status information
        printf("Current process information:\nCalling process PID: %d from priority Queue 1\nProcess Scheduling type: %s\nStatic Priority: %d\nDynamic Priority: %d\nRemaining execution time: %dms\nTime slice: %dms\nAccumulated time slice: %dms\nThread ID: %d\n\n",
                current->pid, scheduling_types[current->process_policy], current->sp, current->dp, current->remain_time, current->time_slice, current->accu_time_slice, current->thread_id);
        
        //Run process
        usleep(MIN(current->time_slice, current->remain_time)*1000);

        //adjust values
        current->accu_time_slice += MIN(current->time_slice, current->remain_time);
        current->remain_time -= current->time_slice;
        

        //Random dynamic priority
        int bonus = rand()%11;
        current->dp = MAX(100, MIN(current->dp - bonus + 5, 139));
        current->time_slice = current->dp < 120 ? ((140 - current->sp) * 20) : ((140 - current->dp) * 5);

        //See if process is done
        if(current->remain_time <= 0){
            current->remain_time = 0;
            processes_done++;
            current->dp = 140; //Set DP to highest possible value so other processes in priority Queue will run instead of this
            total_processes_finished++;
        }

        printf("Current process information after processing:\nCalling process PID: %d from priority Queue 1\nProcess Scheduling type: %s\nStatic Priority: %d\nDynamic Priority: %d\nRemaining execution time: %dms\nTime slice: %dms\nAccumulated time slice: %dms\nThread ID: %d\n\n",
                current->pid, scheduling_types[current->process_policy], current->sp, current->dp, current->remain_time, current->time_slice, current->accu_time_slice, current->thread_id);

        //If dynamic priority exceeds limit for this priority queue and isn't using the shelved priority, put it into the next one, and increment process counter for that queue
        if((current->dp > 129) && (current->dp < 140)){
            RQ2[in_RQ2 + processes_moved] = *current;
            processes_done++;
            processes_moved++;
        }

    }
    return processes_moved;
}

void run_RQ2(buffer* RQ2, int process_count, buffer* RQ1){
    int processes_done = 0;



    //find highest priority process
    while(processes_done < process_count){
        int lowest_value = 140;
        int prio_index = -1;
        for(int i = 0; i < process_count; i++){
            if(RQ2[i].dp < lowest_value){
                lowest_value = RQ1[i].dp;
                prio_index = i;
            }
        }

        buffer* current = &RQ2[prio_index];

        //print status information
        printf("Current process information:\nCalling process PID: %d from priority Queue 2\nProcess Scheduling type: %s\nStatic Priority: %d\nDynamic Priority: %d\nRemaining execution time: %dms\nTime slice: %dms\nAccumulated time slice: %dms\nThread ID: %d\n\n",
                current->pid, scheduling_types[current->process_policy], current->sp, current->dp, current->remain_time, current->time_slice, current->accu_time_slice, current->thread_id);
        
        //Run process
        usleep(MIN(current->time_slice, current->remain_time) * 1000);

        //adjust values
        current->accu_time_slice += MIN(current->time_slice, current->remain_time);
        current->remain_time -= current->time_slice;
        

        //Random dynamic priority
        int bonus = rand()%11;
        current->dp = MAX(100, MIN(current->dp - bonus + 5, 139));
        current->time_slice = current->dp < 120 ? ((140 - current->sp) * 20) : ((140 - current->dp) * 5);

        //See if process is done
        if(current->remain_time <= 0){
            current->remain_time = 0;
            processes_done ++;
            current->dp = 140; //Set DP to highest possible value so other processes in priority Queue will run instead of this
            total_processes_finished++;
        }

        printf("Current process information after processing:\nCalling process PID: %d from priority Queue 2\nProcess Scheduling type: %s\nStatic Priority: %d\nDynamic Priority: %d\nRemaining execution time: %dms\nTime slice: %dms\nAccumulated time slice: %dms\nThread ID: %d\n\n",
                current->pid, scheduling_types[current->process_policy], current->sp, current->dp, current->remain_time, current->time_slice, current->accu_time_slice, current->thread_id);

        //If dynamic priority exceeds limit for this priority queue, put it into the next one, and increment process counter for that queue
        if(current->dp < 130){
            RQ1[0] = *current;
            processes_done += 1 - run_RQ1(RQ1, 1, RQ2, prio_index);
        }
    }
}


void *consumer(void *arg){
    buffer processes[NUM_PROCESSES];
    int* buffers_index_pointer = (int *)arg;
    int buffers_index = *buffers_index_pointer;

    printf("Buffer array given is array %d\n", buffers_index);

    for(int i = 0; i < NUM_PROCESSES; i++){
        processes[i] = buffers[buffers_index][i];
        printf("Copied over process with PID %d, sp of %d, scheduling type %s, and remaining time of %dms\n",
                    processes[i].pid, processes[i].sp, scheduling_types[processes[i].process_policy], processes[i].remain_time);
    }

    buffer RQ0[NUM_PROCESSES];
    buffer RQ1[NUM_PROCESSES];
    buffer RQ2[NUM_PROCESSES];

    int num_RQ0 = 0;
    int num_RQ1 = 0;
    int num_RQ2 = 0;

    

    //sort processes into appropriate priority Qs
    for(int i = 0; i < NUM_PROCESSES; i++){
        
        switch(processes[i].process_policy){
            case RR:
                RQ0[num_RQ0] = processes[i];
                num_RQ0++;
                break;
            case FIFO:
                RQ0[num_RQ0] = processes[i];
                num_RQ0++;
                break;
            case NORMAL:
                RQ1[num_RQ1] = processes[i];
                num_RQ1++;
                break;
        }

    }


    int total_RQ0_processes = num_RQ0;
    while(num_RQ0 > 0){
        //Get Lowest priority value/highest priority process
        int prio_index = -1;
        int lowest_value = 140;
        for(int i = 0; i < total_RQ0_processes; i++){
            if(RQ0[i].dp < lowest_value){
                lowest_value = RQ0[i].dp;
                prio_index = i;
            }
        }
        printf("Lowest priority value is at index %d\n", prio_index);
        buffer* current = &RQ0[prio_index];
        
        printf("Current process information:\nCalling process PID: %d from priority Queue 0\nProcess Scheduling type: %s\nStatic Priority: %d\nDynamic Priority: %d\nRemaining execution time: %dms\nTime slice: %dms\nAccumulated time slice: %dms\nThread ID: %d\n\n",
                 current->pid, scheduling_types[current->process_policy], current->sp, current->dp, current->remain_time, current->time_slice, current->accu_time_slice, current->thread_id);

        usleep(MIN(current->time_slice, current->remain_time) * 1000);
        current->accu_time_slice += MIN(current->time_slice, current->remain_time);
        current->remain_time -= current->time_slice;

        if(current->remain_time <= 0){
            current->remain_time = 0;
            num_RQ0 --;
            current->dp = 140; //Set DP to highest possible value so other processes in priority Queue will run instead of this
            total_processes_finished++;
        }
        current->time_slice = (140 - current->sp) * 20;

        printf("Current process information after processing:\nCalling process PID: %d from priority Queue 0\nProcess Scheduling type: %s\nStatic Priority: %d\nDynamic Priority: %d\nRemaining execution time: %dms\nTime slice: %dms\nAccumulated time slice: %dms\nThread ID: %d\n\n",
                current->pid, scheduling_types[current->process_policy], current->sp, current->dp, current->remain_time, current->time_slice, current->accu_time_slice, current->thread_id);
    }
    num_RQ2 += run_RQ1(RQ1, num_RQ1, RQ2, num_RQ2);
    run_RQ2(RQ2, num_RQ2, RQ1);

    threads_finished++;
    printf("Threads that have finished: %d\nTotal processes finished so far: %d\n\n", threads_finished, total_processes_finished);
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
        res = pthread_create(&threads[i], &attribute, consumer, (void *)&i);
        if(res != 0){
            perror("Could not create thread\n");
            exit(EXIT_FAILURE);
            
        } else {
            for(int j = 0; j < NUM_PROCESSES; j++){
                buffers[i][j].thread_id=threads[i];
            }
        }
        sleep(1);
    }
    while(threads_finished < NUM_CORES){sleep(1);};
    exit(EXIT_SUCCESS);

}