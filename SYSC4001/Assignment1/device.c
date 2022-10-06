//writing data to controller goes in order: 1 if initializing, 0 if not, Then send entire structure over

#include "files.h"

int main(int argc, char* argv[]){

    int controller_fifo_fd;
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

    if(device.is_actuator){
        device.value = -1;
        device.threshold = -1; //assign error threshold for actuator
    } else {
        device.value = 1;
        device.threshold = atoi(argv[3]);
    }
    
    //Confirm device creation
    if(device.is_actuator){
        printf("Created actuator device named %s\n", device.name);
    } else {
        printf("Created sensor device named %s with threshold %d.\n", device.name, device.threshold);
    }

    //Open FIFO
    controller_fifo_fd = open(CONTROLLER_FIFO, O_WRONLY);
    if(controller_fifo_fd == -1){
        perror("Could not open controller FIFO\n");
        exit(EXIT_FAILURE);
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
    
    //simulated repeat writes
    for(int i = 0; i < 5; i++){
        device.value++;
        //write a 0 so controller knows this is not a first time write
        result = write(controller_fifo_fd, &first, sizeof(int));
        //write the PID so the controller can find the right sensor index in its array
        result = write(controller_fifo_fd, &device.sensor_pid, sizeof(pid_t));
        //write the rest of the data
        result = write(controller_fifo_fd, &device, sizeof(device));
        sleep(1);
    }
    close(controller_fifo_fd);
    exit(0);

}