#include "files.h"

void action(){
    printf("Actuator has been switched\n");
}


int main(int argc, char* argv[]){

    //Special sensor acts as actuator
    sensor act;
    strcpy(act.name, argv[1]);
    act.sensor_pid = getpid();
    act.value = 0;
    act.threshold = 0;
    //activated value of -1 specifies sensor as actuator
    act.activated = -1;

    int controller_fifo_fd = open(CONTROLLER_FIFO, O_WRONLY);

    if(controller_fifo_fd == -1){
        printf("could not open controller FIFO\n");
        exit(1);
    }

    printf("Sending actuator info to controller\n");
    //Actuator process is registered in controller as active actuator
    int result = write(controller_fifo_fd, &act, sizeof(sensor));

    struct sigaction actuate;
    actuate.sa_handler = action;
    actuate.sa_flags = 0;
    sigemptyset(&actuate.sa_mask);

    //Signal is sent from controller when threshold value is exceeded on a sensor
    sigaction(SIGALRM, &actuate, 0);

    while(1){
        //wait for incoming signal
        pause();
    }

}