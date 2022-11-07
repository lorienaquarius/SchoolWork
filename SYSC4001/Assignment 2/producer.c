#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/shm.h>
#define NUM_BUFFER 100
// typedef struct sembuf{
//     short sem_num;
//     short sem_op; //-1 for wait, +1 for signal
//     short sem_flag;
// } sembuf;


int main(){

    //Semaphore S
    int success = semget((key_t) 1111, 1, 0666);

    //Semaphore E
    if(success == -1){
        perror("Could not create semaphore\n");
    }
    success = semget((key_t) 2222, 1, 0666);

    //Semaphore N
    if(success == -1){
        perror("Could not create semaphore\n");
    }

    success = semget((key_t) 3333, 1, 0666);

    if(success == -1){
        perror("Could not create semaphore\n");
    }

    struct sembuf wait = {0, -1, SEM_UNDO};
    
    FILE *fp = fopen("test.txt", "r");

    success = shmget((key_t) 1234, (size_t) (128 + sizeof(int) * NUM_BUFFER), 0777 | IPC_CREAT);

    if(success == -1){
        perror("could not create shared memory\n");
    }


}