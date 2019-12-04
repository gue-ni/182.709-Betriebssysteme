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

int main(int argc, char *argv[])
{
	char buf[10]; 
	float value;

	int pid1 = 0, pid2 = 0, n = 0;

	int pipefd1_c1[2];
	int pipefd2_c1[2];
//	int pipeo_c2[2];
//	int pipei_c2[2];

	if(pipe(pipefd1_c1) == -1 || pipe(pipefd2_c1) == -1){
		fprintf(stderr, "unabele to create pipe\n");
	}

	// create two child processes
	pid1 = fork();

//	if (pid1 != 0)
//		pid2 = fork();

	if ( (pid1 != 0)){
		// parent
		printf("I'm parent of process %d and %d\n", pid1, pid2);
		while (fgets(buf, sizeof(buf), stdin) != NULL){
			value = strtof(buf, NULL);
			write(pipefd1_c1[1], &value, sizeof(float));




		}
	

//	} else if ( pid1 + pid2 > 0 ){
		// child 1
	}else{
		// child 2
		dup2(pipefd1_c1[0], STDIN_FILENO);
		dup2(pipefd2_c1[1], STDOUT_FILENO);
		
		float fbuf;
	
		read(STDIN_FILENO, &fbuf, sizeof(float));
		fprintf(stderr, "child read: %f\n", fbuf);

		
		
		
	}




	return 0;
}

