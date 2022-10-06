#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#define CONTROLLER_FIFO "/tmp/controller_fifo"
typedef struct sensor{
    pid_t sensor_pid;
    char name[256];
    int threshold;
    int value;
    int is_actuator; //1 is actuator, 0 is sensor
} sensor;
