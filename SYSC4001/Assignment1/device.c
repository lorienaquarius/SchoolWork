//writing data to controller goes in order: 1 if initializing, 0 if not, Then send entire structure over

#include "files.h"

int controller_fifo_fd;

void stop(){
    printf("Sensor has reached threshold, stopping device.\n");
    exit(0);
}

int main(int argc, char* argv[]){

    
    sensor device;
    device.sensor_pid = getpid();

    //Assign device actuator status and check if it's valid
    device.is_actuator = atoi(argv[1]);
    if(device.is_actuator < 0 || device.is_actuator > 1){
        perror("Actuator value not supported. Please select 0 for sensor, 1 for actuator");
        exit(1);
    }
    
    //Assign name and threshold value
    strcpy(device.name, argv[2]);
    
    switch(device.is_actuator){
        case 0:
            device.value = 1;
            device.threshold = atoi(argv[3]);
            printf("Created sensor device named %s with threshold %d.\n", device.name, device.threshold);
            
            //Open FIFO
            controller_fifo_fd = open(CONTROLLER_FIFO, O_WRONLY);
            if(controller_fifo_fd == -1){
                perror("Could not open controller FIFO\n");
                exit(EXIT_FAILURE);
            }
            break;
        case 1:
            device.value = -1; //assign error value for actuator
            device.threshold = -1; //assign error threshold for actuator

            printf("Created actuator device named %s\n", device.name);
            break;
    }

    //Initial write a 1 to let controller know this is a new device
    int first = 1;
    int result = write(controller_fifo_fd, &first, sizeof(int));

    //Write device information in it's entirety
    if(result > 0){
        result = write(controller_fifo_fd, &device, sizeof(device));
    }
    first = 0;
    sleep(3);
    
    switch(device.is_actuator){
        case 0:
            struct sigaction die;
            die.sa_handler = stop;
            die.sa_flags = 0;
            sigemptyset(&die.sa_mask);

            sigaction(SIGPOLL, &die, 0);
            while(1){
                //increment device value
                device.value++;
                //write a 0 so controller knows this is not a first time write
                result = write(controller_fifo_fd, &first, sizeof(int));
                //write the PID so the controller can find the right sensor index in its array
                result = write(controller_fifo_fd, &device.sensor_pid, sizeof(pid_t));
                //write the rest of the data
                result = write(controller_fifo_fd, &device, sizeof(device));
                sleep(1);
            }
            break;
    }
    //simulated repeat writes
    printf("Signal was not received, exitting abnormally\n");
    exit(1);

}