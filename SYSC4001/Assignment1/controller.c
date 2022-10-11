#include "files.h"

sensor actuator; //designated actuator

//Check sensor threshold to see if the sensor's value is at it's threshold
void check_threshold(sensor s){

    //Check threshold value
    if(s.value > s.threshold){
        kill(s.sensor_pid, SIGPOLL); //send signal to sensor that it has maxed out and to stop
        kill(getppid(), SIGUSR1); //send signal to parent
        if(actuator.name[0] != 0){
            kill(actuator.sensor_pid, SIGALRM); //Send signal to actuator that threshold has been reached for a device
        } else {
            printf("Sensor has reached threshold, however no actuator has been registered\n");
        }
    }
}

//Dummy function so parent process signal handler has a function to use
void alert_cloud(){
    return;
}

int main(){

    pid_t pid;
    
    //create child process
    pid = fork();
    
    switch(pid){
        case -1:
            //Failed fork
            perror("Fork failed\n");
            exit(1);
        //Child Process
        case 0:
            actuator.name[0] = 0;
            int read_result;
            sensor reader;

            //Make FIFO for devices to write to controller
            mkfifo(CONTROLLER_FIFO, 0777);
            
            int controller_fifo_fd = open(CONTROLLER_FIFO, O_RDONLY);

            if(controller_fifo_fd == -1){
                perror("Failed to open controller FIFO");
            }
            
            do{

                read_result = read(controller_fifo_fd, &reader, sizeof(sensor));
                if(read_result > 0){
                    switch(reader.activated){
                        //device is not yet activated
                        case 0:
                            kill(reader.sensor_pid, SIGCONT);
                            printf("activating sensor\n");
                            break;
                        //device is activated
                        case 1:
                            
                            check_threshold(reader);
                            printf("Received sensor data from device %s, with pid %d. Value is %d / %d.\n", reader.name, reader.sensor_pid, reader.value, reader.threshold);
                            break;
                        //device is an actuator
                        case -1:
                            actuator = reader;
                            printf("Actuator Registered with name %s, pid %d\n", actuator.name, actuator.sensor_pid);
                            
                            break;
                        }
                    }  
                        
                } while(read_result > 0);
                break;
            //Parent Process            
        default:

            int wr_res;

            //Open Cloud FIFO to write to
            int cloud_fifo_fd = open(CLOUD_FIFO, O_WRONLY);
            if(cloud_fifo_fd == -1){
                printf("Could not open Cloud FIFO\n");
                exit(1);
            }

            //Check for signal
            struct sigaction trigger;
            trigger.sa_handler = alert_cloud;
            trigger.sa_flags = 0;
            sigemptyset(&trigger.sa_mask);
            sigaction(SIGUSR1, &trigger, 0);

            //Main Loop
            while(1){
                //Wait for SIGUSR1 signal from child, signifying threshold has been reached on a device
                pause();
                printf("Writing to cloud\n");
                wr_res = write(cloud_fifo_fd, &actuator, sizeof(sensor));
                //Check if write operation was successful
                if(wr_res < 1){
                    printf("Cloud FIFO was not written to\n");
                }
            }
            break;                     
    }
    exit(0);
}
