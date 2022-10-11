//All necessary libraries in one convenient place
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>


//FIFO locations
#define CONTROLLER_FIFO "/tmp/controller_fifo"
#define CLOUD_FIFO "/tmp/cloud_fifo"

/* Used for sensor devices and has special setup for actuator
*  This makes it easier for sensors and actuators to write to the same FIFO
*  since they are the same data size
*/
typedef struct sensor{
    pid_t sensor_pid;
    char name[256];
    int threshold;
    int value;
    int activated;
} sensor;
