/*
	@author: Jakob G. Maier <e11809618@student.tuwien.ac.at>
	@date: 2019-12-11 
	@brief: forkFFT 

	TODO:
	fix reading from child eg read_pipe and parse_complex


*/
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "forkFFT.h"

#define OUTPUT 	0
#define INPUT 	1  
#define BUFSIZE (256)
#define EVEN 	0
#define ODD 	1
#define DEBUG   0	

void complex_mult(complex x, complex y, complex *result)
{
	result->a = x.a * y.a - x.b * y.b;
	result->b = x.a * y.b + x.b * y.a;
}

void debug(char *msg)
{
	if (DEBUG) 
		fprintf(stderr, "(%d) %s\n", getpid(), msg);
}

void print_complex(complex c)
{
	fprintf(stdout, "%.7f %.7f*i\n", c.a, c.b);
}

void print_complex_err(complex c)
{
	fprintf(stderr, "(%d) %.7f %.7f*i\n", getpid(), c.a, c.b);
}

void parse_complex(char *buf, complex *result)
{
	char *endptr;
	endptr = buf;
	result->a = strtof(endptr, &endptr);
	result->b = strtof(endptr, NULL);
}

void parse_mult_complex(char *buf, complex *result, int n)
{
	char *endptr;
	endptr = buf;
	float a, b;

	for (int i = 0; i < n; i++){
		a = strtof(endptr, &endptr);
		b = strtof(endptr, &endptr);
		endptr += 3; // cut of *i \n
		result[i].a = a;
		result[i].b = b;

		//printf("test: %f %f*i\n", result[i].a, result[i].b);
	}
}

void close_all(int *fd)
{
	for (int i = 0; i < 2; i++){
		close(fd[i]);
	}
}

void exit_error(char *msg)
{
	fprintf(stderr, "%s\n", msg);
	exit(EXIT_FAILURE);
}

void create_child(int *P, int *R, int *P1, int *R1)
{
	dup2(R[INPUT], STDOUT_FILENO);
	dup2(P[OUTPUT], STDIN_FILENO);

	close_all(R1);
	close_all(P1);
	close_all(R);
	close_all(P);

	execlp("./forkFFT", "./forkFFT", "0", NULL);
	fprintf(stderr, "Failed to execute '\n");
	exit(EXIT_FAILURE);
}

void read_child(int fd, complex *R, int n)
{
	char buf[1024]; // size of 1 complex number
	for (int i = 0; read(fd, buf, sizeof(buf)) > 0; i++){
		//fprintf(stderr, "(%d) reading from child: %s", getpid(), buf);

		parse_mult_complex(buf, R+i, n);
		fprintf(stderr, "(%d) reading from child: %f %f*i\n", getpid(), R[i].a, R[i].b);
	}
}

int main(int argc, char *argv[])
{
	int rec = 1, m = 0;
	if (argc > 1){
		rec = 1;
		m = 0;
	} else {
		m = 1;
	}

	int even_R[2];
	int even_P[2];
	int odd_R[2];
	int odd_P[2];

	int pid1, pid2;

	pipe(even_R);
	pipe(even_P);
	pipe(odd_R);
	pipe(odd_P);

	char buf[BUFSIZE];
	char buffer[2][BUFSIZE];

	memset(buf, 0, sizeof(buf));
	memset(buffer[0], 0, sizeof(buffer[0]));
	memset(buffer[1], 0, sizeof(buffer[1]));

	int n = 0;
	while (fgets(buf, sizeof(buf), stdin) != NULL && strcmp(buf, "\n") != 0){
		if (DEBUG) fprintf(stderr, "(%d) has read something... %s", getpid(), buf);	

		if (n % 2 == 0){
			strncpy(buffer[EVEN], buf, 10);
		}else{
			strncpy(buffer[ODD], buf, 10);
		}

		if ( n == 1 && rec == 1){ // TODO remove rec
			if (DEBUG) fprintf(stderr, "(%d) forking...\n", getpid());
			pid1 = fork();
			if (pid1 == 0){ // child 1
				n = 0;
				create_child(even_P, even_R, odd_P, odd_R);
			} 

			pid2 = fork();
			if (pid2 == 0){ // child 2 
				n = 0;
				create_child(odd_P, odd_R, even_P, even_R);
			} 

			if (close(even_P[OUTPUT]) == -1)
				exit_error("error closeing");
			if(close(odd_P[OUTPUT]) == -1)
				exit_error("error closing");
			if(close(odd_R[INPUT]) == -1)
				exit_error("error closing");
			if(close(even_R[INPUT]) == -1)
				exit_error("error closing");
		}

		if ( n % 2 != 0){ // at least two values have been read
			//if (DEBUG) fprintf(stderr, "(%d) %d values, writing...\n", getpid(), n+1);
			if (write(odd_P[INPUT],  buffer[ODD], strlen(buffer[ODD])) == -1)
				exit_error("error writing");

			if (write(even_P[INPUT], buffer[EVEN], strlen(buffer[EVEN])) == -1)
				exit_error("error writing");

			//if (DEBUG) fprintf(stderr, "(%d) wrote %d bytes\n", getpid(), wrote);

		}
		n++;
	}

	if (DEBUG) fprintf(stderr, "(%d) is finished reading, closing pipes...\n", getpid());
	close(even_P[INPUT]); 
	close(odd_P[INPUT]); 

	if (n == 1){
		float value = strtof(buffer[EVEN], NULL);
		printf("%.7f 0.0000000*i\n", value); 
		if (DEBUG) fprintf(stderr, "(%d) only one value %f 0.0*i\n", getpid(), value); 
		exit(EXIT_SUCCESS);
	} else  if (n % 2 != 0){
		exit_error("Input array is not even");
	}

	if (n == 0){
		exit_error("did not read anything");
	}
	
	int status1, status2;
	waitpid(pid2, &status2, 0);
	if (WIFEXITED(status2)) {
		if (DEBUG) fprintf(stderr, "(%d) child %d exited...\n", getpid(), pid2);
		if (WEXITSTATUS(status1) == 1){
			exit_error("child exited with error");
		}
	}

	waitpid(pid1, &status1, 0);
	if (WIFEXITED(status1)) {
		if (DEBUG) fprintf(stderr, "(%d) child %d exited...\n", getpid(), pid1);
		if (WEXITSTATUS(status1) == 1){
			exit_error("child exited with error");
		}
	}

	if (DEBUG) fprintf(stderr, "(%d) size of n: %d\n", getpid(), n);
	
	complex *R_e = malloc(sizeof(complex) * (n/2)); 
	read_child(even_R[OUTPUT], R_e, n/2);
	close(even_R[OUTPUT]);

	complex *R_o = malloc(sizeof(complex) * (n/2));
	read_child(odd_R[OUTPUT], R_o, n/2);
	close(odd_R[OUTPUT]);

	if (1 && n > 1){
		if (1)fprintf(stderr, "(%d) print even:\n", getpid());
		for (int i = 0; i < n/2; i++){
			if (1) print_complex_err(R_e[i]);
		}
	}

	if (1 && n > 1){
		fprintf(stderr, "(%d) print odd:\n", getpid());
		for (int i = 0; i < n/2; i++){
			if (1) print_complex_err(R_e[i]);
		} 
	}

	complex *R = malloc(sizeof(complex) * n);

	complex tmp, exp;
	int x;
	for (int k = 0; k < n/2; k++){
		x = (-(2 * PI) / n) * k;
		exp.a = cos(x);
		exp.b = sin(x);

		complex_mult(R_o[k], exp, &tmp);			
			
		R[k].a = R_e[k].a + tmp.a;
		R[k].b = R_e[k].b + tmp.b;
		
		R[k+n/2].a = R_e[k].a - tmp.a;	
		R[k+n/2].b = R_e[k].b - tmp.b;	
	}
	
	free(R_e);
	free(R_o);

	if (0) fprintf(stderr, "(%d) output:\n", getpid());
	for (int i = 0; i < n; i++){
		if (1) fprintf(stderr, "(%d) writing output: %f %f*i\n", getpid(), R[i].a, R[i].b);
		print_complex(R[i]);
	}

	free(R);

	return EXIT_SUCCESS;
}

