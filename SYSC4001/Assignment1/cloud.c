#include "files.h"

int main(){

    mkfifo(CLOUD_FIFO, 0777);
    sensor actuated;
    int rd_res;
    int cloud_fifo_fd = open(CLOUD_FIFO, O_RDONLY);
    if(cloud_fifo_fd == -1){
        printf("Could not open cloud FIFO\n");
        exit(1);
    }

    while(1){
        rd_res = read(cloud_fifo_fd, &actuated, sizeof(sensor));
        if(rd_res > 0){
            printf("A sensor has reached it's threshold, and has activated an actuator!\n");
        }
    }

}