//writing data to controller goes in order: 1 if initializing, 0 if not, Then send entire structure over

#include "files.h"

int controller_fifo_fd;
sensor device;
void stop(){
    printf("Sensor has reached threshold, and has been signalled to stop. Stopping device.\n");
    exit(0);
}

void acknowledge(){
    return;
}

int main(int argc, char* argv[]){

    
    
    device.sensor_pid = getpid();
    device.activated = 0;
    //Assign device actuator status and check if it's valid
    
    //Assign name and threshold value
    strcpy(device.name, argv[1]);
    
    device.value = 1;
    device.threshold = atoi(argv[2]);
    printf("Created sensor device named %s with threshold %d.\n", device.name, device.threshold);
    
    //Open FIFO
    controller_fifo_fd = open(CONTROLLER_FIFO, O_WRONLY);
    if(controller_fifo_fd == -1){
        perror("Could not open controller FIFO\n");
        exit(EXIT_FAILURE);
    }

    //Write device information in it's entirety
    int result = write(controller_fifo_fd, &device, sizeof(sensor));

    //make sure result gets written in
    while(result <= 0){
        sleep(1);
        result = write(controller_fifo_fd, &device, sizeof(sensor));
    }

    //Setup for acknowledge signal
    struct sigaction ack;
    ack.sa_handler = acknowledge;
    ack.sa_flags = 0;
    sigemptyset(&ack.sa_mask);
    sigaction(SIGCONT, &ack, 0);

    printf("Awaiting controller acknowledge\n");

    pause();

    printf("Sensor Initiated\n");

    //Setup for threshold reached signal
    device.activated = 1;
    struct sigaction die;
    die.sa_handler = stop;
    die.sa_flags = 0;
    sigemptyset(&die.sa_mask);

    sigaction(SIGPOLL, &die, 0);
    while(1){
        device.value++;
        result = write(controller_fifo_fd, &device, sizeof(device));
        if(result < 0){
            printf("FIFO could not be written to\n");
        }
        sleep(1);
    }

}