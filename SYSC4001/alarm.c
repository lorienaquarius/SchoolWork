#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

static int alarm_fired = 0;

void ding(int sig){
    alarm_fired=1;
}

int main(){
    pid_t pid;
    printf("alarm application starting\n");

    pid = fork();
    switch(pid){
        case -1:
            perror("Fork Failed");
            exit(1);

        case 0:
            printf("waiting for signal in child with PID: %d\n", getpid());
            
            struct sigaction action;
            action.sa_handler = ding;
            action.sa_flags = 0;
            sigemptyset(&action.sa_mask);

            sigaction(SIGALRM, &action, 0);

            pause();

            printf("received alarm signal in child\n");

            if(alarm_fired){
                printf("Ding!\n");

            }
            printf("Child process finished with exit code 0\n");
            exit(0);

        default:
            printf("in parent with PID %d putting thread to sleep for 5 seconds to allow child to reach pause()\n", pid);
            sleep(5);
            int internetPackets = 0;

            while(internetPackets < 25){
                internetPackets++;
            }
            printf("Packet threshold reached, parent sending SIGALRM\n");
            kill(pid, SIGALRM);
            printf("Parent finished with exit code 0\n");
            exit(0);
    }



}