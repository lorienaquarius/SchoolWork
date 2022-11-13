#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#define NUM_BUFFER 100

//Structure to hold 128 bytes from a text file and byte count
typedef struct buffer{
    char data[128];
    int count;
} buffer;


int main(){

    //Semaphore S
    int S = semget((key_t) 1111, 1, 0777 | IPC_CREAT);

    if(S == -1){
        perror("Could not create semaphore S\n");
    }

    //Semaphore E
    int E = semget((key_t) 2222, 1, 0777 | IPC_CREAT);
    
    if(E == -1){
        perror("Could not create semaphore E\n");
    }

    //Semaphore N
    int N = semget((key_t) 3333, 1, 0777 | IPC_CREAT);

    if(N == -1){
        perror("Could not create semaphore N\n");
    }

    //structs for semop()
    struct sembuf wait = {0, -1, SEM_UNDO};
    struct sembuf signal = {0, 1, SEM_UNDO};
    
    
    int write_file_fd = open("write.txt", O_WRONLY);

    if(write_file_fd == -1){
        perror("Could not open read file\n");
    }

    int shm_id = shmget((key_t) 1234, (size_t) (sizeof(buffer) * NUM_BUFFER), 0777 | IPC_CREAT);
    
    if(shm_id == -1){
        perror("could not create shared memory\n");
    }
    buffer *shm;
    shm = (buffer *)shmat(shm_id, (void *)0, SHM_RND);
    if(shm == (buffer *)-1){
        perror("could not attach shared memory\n");
    }

    buffer temp;
    int done = 1;
    long int byte_count = 0;
    int success;
    int out = 0;
    int payload_size = 128;
    while(done){
        //wait(n), wait(s)
        success = semop(N, &wait, 1);
        if(success == -1){
            perror("Semaphore N operation wait failed\n");
        }
        success = semop(S, &wait, 1);
        if(success == -1){
            perror("Semaphore S operation wait failed\n");
        }


        strcpy(temp.data, shm[out].data);
        temp.count = shm[out].count;

        //signal(s), signal(e)
        success = semop(S, &signal, 1);

        if(success == -1){
            perror("Semaphore S operation signal failed\n");
        }

        success = semop(E, &signal, 1);

        if(success == -1){
            perror("Semaphore E operation signal failed\n");
        }
        for(int i = 0; i < 128; i++){
            if(temp.data[i] == '\0'){
                done = 0;
                payload_size = i;
            }
        }
        out = (out + 1) % 100;
        byte_count += temp.count;

        success = write(write_file_fd, &temp.data, payload_size);
        if(success == -1){
            perror("Write operation failed\n");
        }

    }
    printf("Total bytes read from shared memory: %ld\n", byte_count);
    success = shmdt(shm);
    if(success == -1){
        printf("Could not detach shared memory\n");
    }

    success = semctl(S, 0, IPC_RMID, 0);

    if(success == -1){
        printf("Could not delete semaphore S\n");
    }
    success = semctl(E, 0, IPC_RMID, 0);

    if(success == -1){
        printf("Could not delete semaphore E\n");
    }

    success = semctl(N, 0, IPC_RMID, 0);

    if(success == -1){
        printf("Could not delete semaphore N\n");
    }

    success = close(write_file_fd);

    if (success == -1){
        printf("Could not close write file\n");
    }
    

}