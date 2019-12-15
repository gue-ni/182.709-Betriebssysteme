#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <complex.h>
#include "childIO.h"
#include "forkFFT.h"

/**
 * @brief reads n complex numbers from filedescriptor and stores them in R
 */ 
void read_child(int fd, float complex *R, int n)
{
	char buf[1024]; 
	for (int i = 0; read(fd, buf, sizeof(buf)) > 0; i++){
		parse_mult_complex_2(buf, R+i, n);
	}
}

void parse_mult_complex_2(char *buf, float complex *result, int n)
{
	char *endptr;
	endptr = buf;
	float a, b;

	for (int i = 0; i < n; i++){
		a = strtof(endptr, &endptr);
		b = strtof(endptr, &endptr);
		endptr += 3; // cut of *i \n
		result[i] = a + b*I;
	}
}

void close_all(int *fd)
{
	for (int i = 0; i < 2; i++){
		close(fd[i]);
	}
}

void create_child(int *P, int *R, int *P1, int *R1)
{
	dup2(R[INPUT], STDOUT_FILENO);
	dup2(P[OUTPUT], STDIN_FILENO);

	close_all(R1);
	close_all(P1);
	close_all(R);
	close_all(P);

	execlp("./forkFFT", "./forkFFT", NULL);
	fprintf(stderr, "Failed to execute\n");
	exit(EXIT_FAILURE);
}