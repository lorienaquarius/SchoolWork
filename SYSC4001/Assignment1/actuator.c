#include "files.h"

void action(){
    printf("Actuator has been switched\n");
}


int main(int argc, char* argv[]){

    sensor act;
    strcpy(act.name, argv[1]);
    act.sensor_pid = getpid();
    act.value = 0;
    act.threshold = 0;
    act.activated = -1;

    int controller_fifo_fd = open(CONTROLLER_FIFO, O_WRONLY);

    if(controller_fifo_fd == -1){
        printf("could not open controller FIFO\n");
        exit(1);
    }

    printf("Sending actuator info to controller\n");
    int result = write(controller_fifo_fd, &act, sizeof(sensor));

    struct sigaction actuate;
    actuate.sa_handler = action;
    actuate.sa_flags = 0;
    sigemptyset(&actuate.sa_mask);

    sigaction(SIGALRM, &actuate, 0);

    while(1){
        pause();
    }

}