#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define INITIAL_SIZE 100
#define BUFSIZE 10
#define PI 3.141592654

void exit_error(char *msg)
{
	fprintf(stderr, "%s\n", msg);
	exit(EXIT_FAILURE);
}

int redirect_pipe(int in, int out)
{
	int infd = dup2(in, STDIN_FILENO);
	int outfd = dup2(out, STDOUT_FILENO);
	return -1 ? infd == -1 || outfd == -1 : 0; 
}

int main(int argc, char *argv[])
{
	char buf[10]; 
	int pid1 = 0;
	int pid2 = 0;
	
	int pe[2];
	int re[2];
	int po[2];
	int ro[2];

	if(pipe(pe) == -1 || pipe(re) == -1)
		exit_error("unabele to create pipe");

	if(pipe(po) == -1 || pipe(ro) == -1)
		exit_error("unabele to create pipe");


	float fbuf[2]; 


	int n = 0;
	float value;
	while (fgets(buf, sizeof(buf), stdin) != NULL){
		value = strtof(buf, NULL);
		
		if (n % 2 == 0){ 
			fbuf[0] = value; 
		} else {
			fbuf[1] = value; 
		}

		if ( n % 2 != 0){ // write on every second read
			write(pe[1], fbuf, sizeof(float));
			write(po[1], fbuf+sizeof(float), sizeof(float));
		}


		if (n == 1){ // read two values, can go into recursion
			
			// create two child processes
			pid1 = fork();
			if (pid1 != 0) 
				pid2 = fork();

			if ( (pid1 != 0) && (pid2 == 0) ){ // child 1
				printf("I know of %d and %d, my pid is %d\n", pid1, pid2, getpid());
				
				dup2(po[0], STDIN_FILENO);
				dup2(ro[1], STDOUT_FILENO);

//				if (redirect_pipe(po[0], ro[1]) == -1)
//					exit_error("failed to redirect");
	
				execlp("./forkFFT", "./forkFFT", NULL);
				exit(EXIT_SUCCESS);
			
			}else if ( (pid1 == 0) && (pid2 == 0)){ // child 2
				printf("I know of %d and %d, my pid is %d\n", pid1, pid2, getpid());
			
				if (redirect_pipe(pe[0], re[1]) == -1)
					exit_error("failed to redirect");
			
				execlp("./forkFFT", "./forkFFT", NULL);
				exit(EXIT_SUCCESS);
			}
			fprintf(stderr, "I'm the parent...\n");	
		}

		n++;
	}

	if (n == 1){
		fprintf(stdout, "%f\n", value);
		exit(EXIT_SUCCESS);
	}

	int status, pid, c_count = 2;
	while (c_count > 0) {
		pid = wait(&status);

		if (WIFEXITED(status)) {
			if ( WEXITSTATUS(status) != 0 ){
				printf("child %d exited with error\n", pid);
				exit(EXIT_FAILURE);
			}else{
				printf("child %d exited normally\n", pid);
			}
		}
		--c_count;
	}

	return EXIT_SUCCESS;
}

