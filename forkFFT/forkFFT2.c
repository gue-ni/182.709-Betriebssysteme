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


	int pid1 = 0, pid2 = 0;
	int pipefd_1[2], pipefd_2[2];
	
	if(pipe(pipefd_1) == -1 || pipe(pipefd_2) == -1){
		fprintf("unabele to create pipe\n");
	}

	// create two child processes
	pid1 = fork();

//	if (pid1 != 0)
//		pid2 = fork();

	if ( (pid1 != 0) && (pid2 != 0) ){
		// parent

		printf("I'm parent of process %d and %d\n", pid1, pid2);
		

	} else if ( pid1 + pid2 > 0 ){
		// child 1
	}else{
		// child 2
		dup2(pipefd[0], STDIN_FILENO);
		dup2(pipefd[1], STDOUT_FILENO);
		
		
	}





/*
	char buf[BUFSIZE];
	int n = 0;
 	float *array, value;
	size_t size = INITIAL_SIZE;

	array = malloc(sizeof(float) * INITIAL_SIZE);
	
	while (fgets(buf, sizeof(buf), stdin) != NULL){
		value = strtof(buf, NULL);
		array[n++] = value;

//		fprintf(stderr, "read: %f\n", array[n - 1]);
		
		if ( n > size - 2){
//			fprintf(stderr, "increasing size...\n");
			size += INITIAL_SIZE;
			array = realloc(array, size);
		}
	}
	
	fprintf(stderr, "n: %d, size: %ld\n", n, size);
	if ( n == 1 ){
		fprintf(stdout, "%f\n", array[0]);
		free(array);
		return EXIT_SUCCESS;
	}

	if (n % 2 != 0)
		return EXIT_FAILURE;

	float *pe, *po;
	pe = malloc(sizeof(float) * n/2);
	po = malloc(sizeof(float) * n/2);

	for (int i = 0; i < n/2; i++){
		pe[i] = array[2 * i];
		po[i] = array[2 * i + 1];	
	}

	int pid1 = 0, pid2 = 0;
	int pipefd_1[2], pipefd_2[2];
	
	if(pipe(pipefd_1) == -1 || pipe(pipefd_2) == -1){
		fprintf("unabele to create pipe\n");
	}

	// create two child processes
	pid1 = fork();

	if (pid1 != 0)
		pid2 = fork();

	if ( (pid1 != 0) && (pid2 != 0) ){
		// parent

		printf("I'm parent of process %d and %d\n", pid1, pid2);
		

	} else if ( pid1 + pid2 > 0 ){
		// child 1
		//execlp("./forkFFT", "./forkFFT", (char *) 0);





	}else{
		// child 2
		close(pipefd[1]);
		dup2(pipefd[0], STDIN_FILENO);
		
	}


	free(array);
	free(pe);
	free(po);
*/
	return 0;
}

