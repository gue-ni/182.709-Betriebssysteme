/*
	@author: Jakob G. Maier <e11809618@student.tuwien.ac.at>
	@date: 2019-12-11 
	@brief: forkFFT 

	TODO: 
	- documentation

*/
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <complex.h>
#include "forkFFT.h"
#include "childIO.h"

char *prog = "<not defined>";

/**
 * @brief Prints error message and exits with EXIT_FAILURE
 **/
void exit_error(char *msg)
{
	fprintf(stderr, "%s %s\n", prog, msg);
	exit(EXIT_FAILURE);
}

/**
 * @brief Used to check whether functions return the frequent error value of -1 
 **/
void check_error(int v)
{
	if (v == -1) exit_error("an error occured");
}

/**
 * Program entry point.
 * @brief Implements the Cooley-Tukey Fast Fourier Transform algorithm.
 * Recursivly calls itself and aplies the "butterfly operation" to calculate 
 * the Fourier Transform 
 * @details
 * @param argc The argument counter
 * @param argv The argument vector
 * @return returns EXIT_SUCCESS
 */
int main(int argc, char *argv[])
{
	prog = argv[0];
	int even_R[2];
	int even_P[2];
	int odd_R[2];
	int odd_P[2];

	int pid1, pid2;

	check_error(pipe(even_R));
	check_error(pipe(even_P));
	check_error(pipe(odd_R));
	check_error(pipe(odd_P));

	char buf[BUFSIZE];
	char buffer[2][BUFSIZE];

	memset(buffer[0], 0, sizeof(buffer[0]));
	memset(buffer[1], 0, sizeof(buffer[1]));

	int n = 0;
	while (fgets(buf, sizeof(buf), stdin) != NULL && strcmp(buf, "\n") != 0){
		
		strncpy(buffer[n % 2], buf, sizeof(buf)); 

		if ( n == 1){ 
			pid1 = fork();
			check_error(pid1);
			if (pid1 == 0){ // child 1
				close_both_ends(odd_P);
				close_both_ends(odd_R);
				create_child(even_P, even_R);
			} 

			pid2 = fork();
			check_error(pid2);
			if (pid2 == 0){ // child 2 
				close_both_ends(even_R);
				close_both_ends(even_P);
				create_child(odd_P, odd_R);
			} 

			if(close(even_P[OUTPUT]) == -1) exit_error("error closing");
			if(close(odd_P[OUTPUT])  == -1) exit_error("error closing");
			if(close(odd_R[INPUT])   == -1) exit_error("error closing");
			if(close(even_R[INPUT])  == -1) exit_error("error closing");
		}

		if ( n % 2 != 0){ // at least two values have been read
			if (write(odd_P[INPUT],  buffer[ODD], strlen(buffer[ODD])) == -1)
				exit_error("error writing");

			if (write(even_P[INPUT], buffer[EVEN], strlen(buffer[EVEN])) == -1)
				exit_error("error writing");
		}
		n++;
	}

	if (close(even_P[INPUT]) == -1) exit_error("error closing"); 
	if (close(odd_P[INPUT]) == -1) exit_error("error closing"); 

	if (n == 0){
		exit_error("Did not read anything");

	} else if(n == 1){
		printf("%f 0.0*i\n", strtof(buffer[EVEN], NULL));  
		exit(EXIT_SUCCESS);

	} else if (n % 2 != 0){
		exit_error("Input array is not even");
	}

	int status1, status2;
	waitpid(pid1, &status1, 0);
	waitpid(pid2, &status2, 0);
	
	if (WEXITSTATUS(status1) == 1 || WEXITSTATUS(status2) == 1){
		exit_error("child encountered error");
	}

	// read from even child
	float complex *R_e = malloc(sizeof(float complex) * (n/2)); 
	if (R_e == NULL)
		exit_error("malloc failed");

	read_child(even_R[OUTPUT], R_e, n/2);
	if(close(even_R[OUTPUT]) == -1) 
		exit_error("error closing");

	// read from odd child
	float complex *R_o = malloc(sizeof(float complex) * (n/2));
	if (R_o == NULL) 
		exit_error("malloc failed");

	read_child(odd_R[OUTPUT], R_o, n/2);
	if(close(odd_R[OUTPUT]) == -1) 
		exit_error("error closing");

	// perform "butterfly operation"	
	float complex *R = malloc(sizeof(float complex) * n);
	if (R == NULL)
		exit_error("malloc failed");

	float complex exp;
	float x;
	for (int k = 0; k < n/2; k++){
		x = (-(2 * PI) / n) * k;
		exp = cos(x) + sin(x)*I;
		
		R[k] 		= R_e[k] + exp * R_o[k];
		R[k+n/2] 	= R_e[k] - exp * R_o[k];
	}
	
	free(R_e);
	free(R_o);

	for (int i = 0; i < n; i++){
		fprintf(stdout, "%f %f*i\n", creal(R[i]), cimag(R[i]));
	}

	free(R);
	return EXIT_SUCCESS;
}

