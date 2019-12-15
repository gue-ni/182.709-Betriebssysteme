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
 * @details
 * @param fd filedescriptor to read from
 * @param R array to store the complex numbers received
 * @param n number of numbers to read
 */ 
void read_child(int fd, float complex *R, int n)
{
	size_t s = sizeof(char) * n * 22; // 22 is the max char length a complex number can have
	char *buf = malloc(s);
	if (buf == NULL) exit_error("malloc failed");

	if (read(fd, buf, s) == -1) exit_error("error reading");
	
	float a, b;
	char *endptr = buf;
	
	for (int i = 0; i < n; i++){
		a = strtof(endptr, &endptr);
		b = strtof(endptr, &endptr);
		endptr += 3; // cut of "*i \n"
		R[i] = a + b*I;
	}

	free(buf);
}

/**
 * @brief close both ends of an unnamed pipe
 * @param int array that contains two filedescriptors
 */
void close_both_ends(int *fd)
{
	for (int i = 0; i < 2; i++){
		if(close(fd[i]) == -1) exit_error("error closing");
	}
}

/**
 * @brief redirect two filedescriptors to stdin and stdout 
 * respectivly and execute ./forkFFT recursivly
 * @details read end of P will be redirected to stdin, write end of R to stdout 
 * @param P int array containing a pipe
 * @param R int array containing a pipe
 */
void create_child(int *P, int *R)
{
	dup2(P[OUTPUT], STDIN_FILENO);
	dup2(R[INPUT], STDOUT_FILENO);

	close_both_ends(R);
	close_both_ends(P);

	execlp("./forkFFT", "./forkFFT", NULL);
	fprintf(stderr, "Failed to execute\n");
	exit(EXIT_FAILURE);
}