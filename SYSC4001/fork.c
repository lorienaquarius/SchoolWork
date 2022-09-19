#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#define MICRO_SEC_IN_SEC 1000000
int main(int argc, char *argv[]){

	struct timeval start, end;
	pid_t pid;
	char *message;
	int n;

	printf("running fork program\n");
	
	gettimeofday(&start, NULL);

	pid=fork();

	switch(pid) {
		case -1: //Error
			perror("fork failed");
			exit(1);
		case 0:
			//fork();
			message = "This is the child";
			//printf("Value of child PID is: %d, PPID is: %d\n", getpid(), getppid());
			n = 5;
			gettimeofday(&end, NULL);
			printf("Time to get to child block: %ld micro-seconds\n", ((end.tv_sec * MICRO_SEC_IN_SEC + end.tv_usec) - (start.tv_sec * MICRO_SEC_IN_SEC + start.tv_usec)));
			
			break;
		default:
			message = "This is the parent";
			n = 3;
			//printf("Value of parent PID is: %d\n", pid);
			gettimeofday(&end, NULL);
			printf("Time to get to parent block: %ld micro-seconds\n", ((end.tv_sec * MICRO_SEC_IN_SEC + end.tv_usec) - (start.tv_sec * MICRO_SEC_IN_SEC + start.tv_usec)));
			break;
	}
	printf("My PID is %d, n = %d\n", getpid(), n);

	for(; n > 0; n--){
		puts(message);
		sleep(1);
	}
	exit(0);
}
