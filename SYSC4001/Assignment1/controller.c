#include "files.h"
#define INITIAL_SENSOR_COUNT 3


void check_threshold(sensor s){
    if(s.value > s.threshold){
        kill(s.sensor_pid, SIGPOLL);
    }
}

int main(){

    pid_t pid;
    pid = fork();

    switch(pid){
        case -1:
            //Failed fork
            perror("Fork failed\n");
            exit(1);
        case 0:
            //start at 2 sensors because this assignment is being tested with 2 sensors
            int read_result;
            int max_sensors = INITIAL_SENSOR_COUNT;
            int initialized_sensors = 0;
            sensor* sensors = malloc(sizeof(sensor) * max_sensors);
            mkfifo(CONTROLLER_FIFO, 0777);
            
            int controller_fifo_fd = open(CONTROLLER_FIFO, O_RDONLY);

            if(controller_fifo_fd == -1){
                perror("Failed to open controller FIFO");
            }

            int first;
            do{


                read_result = read(controller_fifo_fd, &first, sizeof(int));
                printf("read result has value %d\n", read_result);
                
                if(read_result > 0){
                    switch(first){ //First time device has sent information, register relevant info
                        case 1:
                            read_result = read(controller_fifo_fd, &sensors[initialized_sensors], sizeof(sensor)); //Read the struct into device array
                            
                            if(initialized_sensors == max_sensors){
                                //If maximum number of sensors has been reached, need to make more space in the array
                                sensors = realloc(sensors, max_sensors * 2 * sizeof(sensor));
                                max_sensors *= 2;
                            }
                            printf("Successfully initialized new %s device with name %s and threshold %d and PID %d.\n", sensors[initialized_sensors].is_actuator ? "actuator":"sensor", sensors[initialized_sensors].name, sensors[initialized_sensors].threshold, sensors[initialized_sensors].sensor_pid);
                            initialized_sensors++;
                            break;
                        case 0: //Normal sensor read
                            pid_t device_pid;
                            read_result = read(controller_fifo_fd, &device_pid, sizeof(pid_t)); //Read PID info
                            int index;
                            for(int i = 0; i < initialized_sensors; i++){
                                if(sensors[i].sensor_pid == device_pid){
                                    index = i;
                                    break;
                                }
                            }
                            
                            check_threshold(sensors[index]);
                            if(read_result > 0){
                                read_result = read(controller_fifo_fd, &sensors[index], sizeof(sensor)); //get sensor value by re-reading it into array
                                printf("Device threshold is at %d\n", sensors[index].threshold);
                                printf("Received sensor data from device %s, with pid %d. Value is %d / %d.\n", sensors[index].name, sensors[index].sensor_pid, sensors[index].value, sensors[index].threshold);
                            }
                            break;
                            

                    }
                }

            } while(read_result >= 0);

        default:
            break;

    }

}