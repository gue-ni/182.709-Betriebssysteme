#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "forkFFT.h"

#define OUTPUT 0
#define INPUT 1  
#define BUFSIZE 10
#define EVEN 0
#define ODD 1

void complex_mult(complex x, complex y, complex *result)
{
	result->a = x.a * y.a - x.b * y.b;
	result->b = x.a * y.b + x.b * y.a;
}

void debug(char *msg)
{
	fprintf(stderr, "(%d) %s\n", getpid(), msg);
}

void print_complex(complex *c)
{
	fprintf(stdout, "%f %f*i\n", c->a, c->b);
}

void parse_complex(char *buf, complex *result)
{
	char *endptr;
	result->a = strtof(buf, &endptr);
	result->b = strtof(endptr, NULL);
}

void close_all(int *fd)
{
	for (int i = 0; i < 2; i++){
		close(fd[i]);
	}
}

void imexp(int n, int k, complex *result)
{
	int x = (-(2 * PI) / n) * k;
	result->a = cos(x);
	result->b = sin(x);
}

void exit_error(char *msg)
{
	fprintf(stderr, "%s\n", msg);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	int rec = 1;

	if ( argc > 1){
		rec = 0;	
		fprintf(stderr, "(%d) recursion is turned off...\n", getpid());
	}

	fprintf(stderr, "(%d) was started...\n", getpid());
	int even_R[2];
	int even_P[2];
	int odd_R[2];
	int odd_P[2];

	int pid1, pid2;

	// TODO check for return value
	pipe(even_R);
	pipe(even_P);
	pipe(odd_R);
	pipe(odd_P);

	char buf[10];
	char buffer[2][10];

	int n = 0;
	while (fgets(buf, sizeof(buf), stdin) != NULL && memcmp(buf, "\0", 1) != 0){
//		fprintf(stderr, "(%d) has read something... %s", getpid(), buf);	

		if (n % 2 == 0){
			strncpy(buffer[EVEN], buf, 10);
		}else{
			strncpy(buffer[ODD], buf, 10);
		}

		if ( n == 1  && rec){ // TODO remove rec
			fprintf(stderr, "(%d) forking...\n", getpid());
			pid1 = fork();

			if (pid1 == 0){ // child 1
				dup2(even_R[INPUT], STDOUT_FILENO);
				dup2(even_P[OUTPUT], STDIN_FILENO);

				close_all(even_R);
				close_all(even_P);
				close_all(odd_R);
				close_all(odd_P);

				execlp("./forkFFT", "./forkFFT", "0", NULL);
				fprintf(stderr, "Failed to execute '\n");
				exit(EXIT_FAILURE);
			} 

			pid2 = fork();
			if (pid2 == 0){ // child 2 
				dup2(odd_R[INPUT], STDOUT_FILENO);
				dup2(odd_P[OUTPUT], STDIN_FILENO);

				close_all(odd_R);
				close_all(odd_P);
				close_all(even_R);
				close_all(even_P);

				execlp("./forkFFT", "./forkFFT", "0", NULL);
				fprintf(stderr, "Failed to execute '\n");
				exit(EXIT_FAILURE);
			} 

			close(even_P[OUTPUT]);
			close(odd_P[OUTPUT]);
			close(odd_R[INPUT]);
			close(even_R[INPUT]);
		}

		if ( n % 2 != 0){ // at least two values have been read
			fprintf(stderr, "(%d) writing...\n", getpid());
			write(odd_P[INPUT], buffer[ODD], 10);
			write(even_P[INPUT], buffer[EVEN], 10);
		}

		n++;
	}

//	fprintf(stderr, "(%d) is finished reading, closing pipes...\n", getpid());
	close(even_P[INPUT]); // this should send EOF to child
	close(odd_P[INPUT]); // why doesn't it?

	if (n == 1){
		close(odd_P[OUTPUT]);
		close(odd_R[INPUT]);
		close(even_R[INPUT]);
		close(even_P[OUTPUT]);
		
//		fprintf(stderr, "(%d) read only one value... %s \n", getpid(), buf);
		float value = strtof(buffer[EVEN], NULL);

		fprintf(stdout, "%f 0.0*i\n", value); 
		fprintf(stderr, "(%d) %f 0.0*i\n", getpid(), value); 

		exit(EXIT_SUCCESS);
	}

	if (n % 2 != 0){
		exit_error("Input array is not even");
	}
	
	debug("works until here");
	fprintf(stderr, "(%d) size of n: %d\n", getpid(), n);
	
	complex cval;
	complex *R_e = malloc(sizeof(complex) * n/2);
	if (R_e == NULL)
		exit_error("failed to malloc");	

	int k = 0;
	while (read(even_R[OUTPUT], buf, sizeof(buf)) != -1){
		parse_complex(buf, &cval);
		R_e[k] = cval; // segfaults
		k++;
	} 
	
	complex *R_o = malloc(sizeof(complex) * n/2);
	if (R_o == NULL)
		exit_error("failed to malloc");	

	k = 0;
	while (read(odd_R[OUTPUT], buf, sizeof(buf)) != -1){
		parse_complex(buf, &cval);
		R_o[k] = cval;
		k++;
	}

	complex *R = malloc(sizeof(complex) * n);
	if (R == NULL)
		exit_error("failed to malloc");	


	complex tmp, exp;
	for (k = 0; k < n/2; k++){
		imexp(n, k, &exp);	
		complex_mult(R_o[k], exp, &tmp);			
			
		(R+k)->a = (R_e+k)->a + tmp.a;
		(R+k)->b = (R_e+k)->b + tmp.b;

		(R+k+n/2)->a = (R_e+k)->a - tmp.a;	
		(R+k+n/2)->b = (R_e+k)->b - tmp.b;	
	}
	
	free(R_e);
	free(R_o);

	for (int i = 0; i < n; i++){
		print_complex(R+i);
	}

	free(R);

	int status;
	close(even_R[OUTPUT]);
	close(odd_R[OUTPUT]);

	waitpid(pid1, &status, 0);
	waitpid(pid2, &status, 0);
	return EXIT_SUCCESS;
}

