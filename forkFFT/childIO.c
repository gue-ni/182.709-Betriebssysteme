#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "childIO.h"
#include "forkFFT.h"


void read_child(int fd, complex *R, int n)
{
	char buf[1024]; 
	for (int i = 0; read(fd, buf, sizeof(buf)) > 0; i++){
		parse_mult_complex(buf, R+i, n);
	}
}

void complex_mult(complex *result, complex x, complex y)
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
	fprintf(stdout, "%f %f*i\n", c.a, c.b);
}

void print_complex_err(complex c)
{
	fprintf(stderr, "(%d) %f %f*i\n", getpid(), c.a, c.b);
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