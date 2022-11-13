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
    //Set Sem S to initial value of 1
    int success = semctl(S, 0, SETVAL, 1);

    if(success == -1){
        perror("Could not set S semaphore value\n");
    }

    //Semaphore E
    int E = semget((key_t) 2222, 1, 0777 | IPC_CREAT);
    
    if(E == -1){
        perror("Could not create semaphore E\n");
    }
    //Set Sem E value to NUM_BUFFER
    success = semctl(E, 0, SETVAL, NUM_BUFFER);

    if(success == -1){
        perror("Could not set semaphore E value\n");
    }
    //Semaphore N
    int N = semget((key_t) 3333, 1, 0777 | IPC_CREAT);

    if(N == -1){
        perror("Could not create semaphore N\n");
    }
    //Set Sem N to 0
    success = semctl(N, 0, SETVAL, 0);
    if(success == -1){
        perror("Could not set semaphore N value\n");
    }

    //structs for semop()
    struct sembuf wait = {0, -1, 0};
    struct sembuf signal = {0, 1, 0};
    
    //open file for reading
    int read_file_fd = open("read.txt", O_RDONLY);

    //initialize shared memory
    int shm_id = shmget((key_t) 1234, (size_t) (sizeof(buffer) * NUM_BUFFER), 0777 | IPC_CREAT);
    
    if(shm_id == -1){
        perror("could not create shared memory\n");
    }
    //cast shared memory as buffer pointer
    buffer *shm;
    //attach shared memory
    shm = (buffer *)shmat(shm_id, (void *)0, 0);
    if(shm == (buffer *)-1){
        perror("could not attach shared memory\n");
    }
    
    //buffer to recieve reads
    char *buf = malloc(BUFSIZ);
    //initial producer index
    int in = 0;
    //buffer structure to write into shared memory
    buffer temp;

    //start byte count
    int byte_count = 0;
    
    while(read(read_file_fd, buf, BUFSIZ)){
        
        //wait(e), wait(s)

        success = semop(E, &wait, 1);
        if(success == -1){
            perror("Semaphore E operation wait failed\n");
        }
        
        success = semop(S, &wait, 1);
        if(success == -1){
            perror("Semaphore S operation wait failed\n");
        }
        //critical section
         
        for(int i = 0; i < BUFSIZ / 128; i++){
            //Copy data into buffer structure
            strncpy(temp.data, buf + (128 * i), 128);
            temp.count = sizeof(temp);

            //If temp.data is just null, skip everything else
            if(temp.data[0] == '\0'){
                continue;
            }
            //append(v)
            shm[in] = temp;

            //signal(s), signal(n)
            semop(S, &signal, 1);
            semop(N, &signal, 1);

            //increment index
            in = (in + 1) % 100;
            //increment bytes written
            byte_count += temp.count;


        }

    }
    
    printf("Total bytes written to shared memory: %d\n", byte_count);
    success = shmdt(shm);
    if(success == -1){
        perror("Could not detach shared memory\n");
    }
    close(read_file_fd);
    if(success == -1){
        perror("Could not close read file\n");
    }

}