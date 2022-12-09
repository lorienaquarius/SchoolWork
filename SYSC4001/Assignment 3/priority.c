#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define NUM_CORES 4 //number of cores
#define NUM_PROCESSES 5 //number of processes per core

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
            switch(j % 6){
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

void sort_priority(buffer* to_sort, int num_elements){

    //Bubble sort algorithm

    for(int i = 0; i < num_elements - 1; i++){
        for(int j = 0; j < num_elements - i - 1; j++){
            if(to_sort[j].dp > to_sort[j+1].dp){

                //swap elements
                buffer temp = to_sort[j];
                to_sort[j] = to_sort[j+1];
                to_sort[j+1] = temp;

            }
        }
    }
}


//put in seperate function because it may need to be run again
int run_RQ1(buffer* RQ1, int process_count, buffer* RQ2, int in_RQ2){
    int processes_done = 0;
    //if all processes in RQ1 were run to completion and none sent to RQ2, return 1
    //if at least one process was sent up to RQ2, return 0
    int processes_moved = 0;

    //find highest priority process
    while(processes_done < process_count){
        
        //Since priorities change dynamically, need to re-sort every time.
        //Since the number of processes in this queue isn't huge, this shouldn't take an absurd amount of time
        //And even if it did, time efficiency is not marked on this assignment
        sort_priority(RQ1, process_count);
        //implement round robin structure
        for(int i = 0; i < process_count; i++){
            buffer* current = &RQ1[i];

            //don't re-run finished or moved processes
            if((current->dp > 129) || (current->remain_time <= 0)){
                continue;
            }
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
                total_processes_finished++;
            }

            printf("Current process information after processing:\nCalling process PID: %d from priority Queue 1\nProcess Scheduling type: %s\nStatic Priority: %d\nDynamic Priority: %d\nRemaining execution time: %dms\nTime slice: %dms\nAccumulated time slice: %dms\nThread ID: %d\n\n",
                    current->pid, scheduling_types[current->process_policy], current->sp, current->dp, current->remain_time, current->time_slice, current->accu_time_slice, current->thread_id);

            //If dynamic priority exceeds limit for this priority queue and isn't using the shelved priority, put it into the next one, and increment process counter for that queue
            if(current->dp > 129){
                RQ2[in_RQ2 + processes_moved] = *current;
                processes_done++;
                processes_moved++;
            }
        }

        

    }
    //return how many processes were moved into RQ2
    return processes_moved;
}

void run_RQ2(buffer* RQ2, int process_count, buffer* RQ1){
    int processes_done = 0;



    //find highest priority process
    while(processes_done < process_count){
        //Need to resort after every pass
        sort_priority(RQ2, process_count);

        //implement round robin behaviour, every process runs once in priority order
        for(int i = 0; i < process_count; i++){
            buffer* current = &RQ2[i];

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

            //If dynamic priority is lower than this priority queue allows, move it back to RQ1
            if(current->dp < 130){
                RQ1[0] = *current;
                //increment processes_done by 1 if no processes were moved back to RQ2, meaning the process finished execution in RQ1
                processes_done += 1 - run_RQ1(RQ1, 1, RQ2, i);
            }
        }

    }
}


void *consumer(void *arg){
    //processes for this core
    buffer processes[NUM_PROCESSES];
    //get which set of processes should be accessed
    int* buffers_index_pointer = (int *)arg;
    //dereference value
    int buffers_index = *buffers_index_pointer;

    printf("Buffer array given is array %d\n", buffers_index);

    //move global array into local
    for(int i = 0; i < NUM_PROCESSES; i++){
        processes[i] = buffers[buffers_index][i];
        printf("Copied over process with PID %d, sp of %d, scheduling type %s, and remaining time of %dms\n",
                    processes[i].pid, processes[i].sp, scheduling_types[processes[i].process_policy], processes[i].remain_time);
    }

    //Priority queues
    buffer RQ0[NUM_PROCESSES];
    buffer RQ1[NUM_PROCESSES];
    buffer RQ2[NUM_PROCESSES];

    //counter for each priority queue
    int num_RQ0 = 0;
    int num_RQ1 = 0;
    int num_RQ2 = 0;

    

    //sort processes into appropriate priority Queues
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

    sort_priority(RQ0, num_RQ0);

    //num_RQ0 gets decremented, need to keep track of total processes
    int total_RQ0_processes = num_RQ0;
    while(num_RQ0 > 0){
        
        //This implements proper round robin algorithm: processes are sorted by priority already, and so each process will run once, even if it's
        //Lower priority than other processes in the queue
        for(int i = 0; i < total_RQ0_processes; i++){
        //which process got selected

            //Use a pointer so we can work with a single buffer element, but still change the values inside the array
            buffer* current = &RQ0[i];
            
            //Don't repeat done processes
            if(current->remain_time == 0){
                continue;
            }

            printf("Current process information:\nCalling process PID: %d from priority Queue 0\nProcess Scheduling type: %s\nStatic Priority: %d\nDynamic Priority: %d\nRemaining execution time: %dms\nTime slice: %dms\nAccumulated time slice: %dms\nThread ID: %d\n\n",
                    current->pid, scheduling_types[current->process_policy], current->sp, current->dp, current->remain_time, current->time_slice, current->accu_time_slice, current->thread_id);

            //run process
            usleep(MIN(current->time_slice, current->remain_time) * 1000);
            //adjust values
            current->accu_time_slice += MIN(current->time_slice, current->remain_time);
            current->remain_time -= current->time_slice;

            
            //Process is done
            if(current->remain_time <= 0){
                current->remain_time = 0;
                num_RQ0 --;
                total_processes_finished++;
            }
            //Round robin processes will have their time slice adjusted
            current->time_slice = (140 - current->sp) * 20;
            
            printf("Current process information after processing:\nCalling process PID: %d from priority Queue 0\nProcess Scheduling type: %s\nStatic Priority: %d\nDynamic Priority: %d\nRemaining execution time: %dms\nTime slice: %dms\nAccumulated time slice: %dms\nThread ID: %d\n\n",
                    current->pid, scheduling_types[current->process_policy], current->sp, current->dp, current->remain_time, current->time_slice, current->accu_time_slice, current->thread_id);
    
        }

   }

    //Run RQ1, and increment the number of elements in RQ2 by how many processes were moved from RQ1
    num_RQ2 += run_RQ1(RQ1, num_RQ1, RQ2, num_RQ2);

    //Run RQ2
    run_RQ2(RQ2, num_RQ2, RQ1);

    //increment number of threads finished for main thread
    threads_finished++;
    printf("Threads that have finished: %d\nTotal processes finished so far: %d\n\n", threads_finished, total_processes_finished);
}


int main(){
    //Randomize seed
    srand(time(NULL));
    int res;

    //cores
    pthread_t threads[NUM_CORES];
    //setup for detached threads, so they don't need thread_join
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

    //setting scheduling for thread creation, not terribly important because we don't care which core is running when, just look at scheduling
    //within each core
    max_prio = sched_get_priority_max(SCHED_OTHER);
    min_prio = sched_get_priority_min(SCHED_OTHER);
    scheduling_value.sched_priority = min_prio;

    res = pthread_attr_setschedparam(&attribute, &scheduling_value);
    
    if(res != 0){
        perror("could not set scheduling parameters\n");
    }
    pthread_t producer_thread;

    //create producer
    res = pthread_create(&producer_thread, &attribute, producer, NULL);

    scheduling_value.sched_priority = max_prio;
    res = pthread_attr_setschedparam(&attribute, &scheduling_value);

    //wait for producer to finish
    while(!produced){
        usleep(1000);
    }

    //initialize core
    for(int i = 0; i < NUM_CORES; i++){
        res = pthread_create(&threads[i], &attribute, consumer, (void *)&i);
        if(res != 0){
            perror("Could not create thread\n");
            exit(EXIT_FAILURE);
            
        } else {
            //if core creation was successful, assign the thread ID for it's processes
            for(int j = 0; j < NUM_PROCESSES; j++){
                buffers[i][j].thread_id=threads[i];
            }
        }
        //wait a second, so the newly created core can access all of it's data before values are changed
        sleep(1);
    }
    //wait for all cores to finish
    while(threads_finished < NUM_CORES){sleep(1);};
    exit(EXIT_SUCCESS);

}