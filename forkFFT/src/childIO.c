/**
 * @file childIO.c
 * @author Jakob G. Maier <e11809618@student.tuwien.ac.at>
 * @date 2019-12-11 
 * @brief utility functions for communication with child processes 
 */ 
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <complex.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "childIO.h"
#include "forkFFT.h"

/**
 * @brief Reads n complex numbers from filedescriptor and stores them in R
 * @details Reads and parses complex numbers from fd and stores to R
 * @param fd Filedescriptor to read from
 * @param R Array to store the complex numbers received
 * @param n Number of numbers to read
 * @return Amount of numbers read, -1 on error
 */ 
int read_child(int fd, float complex *R, int n)
{
	size_t s = sizeof(char) * n * 22; // 22 is the max char length a complex number can have
	char *buf = malloc(s);
	if (buf == NULL) exit_error("malloc failed");

	if (read(fd, buf, s) == -1) exit_error("error reading");
	
	float a, b;
	char *endptr = buf;
	int read = 0;
	
	for (int i = 0; i < n; i++){
		a = strtof(endptr, &endptr);
		b = strtof(endptr, &endptr);
		endptr += 3; // cut of "*i \n"
		R[i] = a + b*I;
		read++;
	}

	free(buf);

	if (read != n){
		return -1;
	} else {
		return read;
	}



}

/** Close both ends of pipe
 * @brief close both ends of an unnamed pipe
 * @details close both ends or exit with error if an error occurs
 * @param int array that contains two filedescriptors
 */
void close_both_ends(int *fd)
{
	for (int i = 0; i < 2; i++){
		if(close(fd[i]) == -1) exit_error("error closing");
	}
}

/** Duplicate stdin and stdout and call ./forkFFT
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
	exit_error("Failed to execute");
}