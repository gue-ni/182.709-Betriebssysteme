/**
 * @file forkFFT.c
 * @author Jakob G. Maier <e11809619@student.tuwien.ac.at>
 * @date 2019-12-22 
 * @brief Calculate the Fast Fourier Transformation from floating point input values
 * recursivly
 * @details Number of input values must be a power of two, otherwise the programm exits with
 * an error
 * 
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

char *prog = "<not defined>"; /** < program name used in error messages */

/**
 * @brief Prints error message and exits with EXIT_FAILURE
 * @details Does not check anything and simply prints the error message
 * and exits with EXIT_FAILURE
 * @param msg Message to print
 * @return void
 **/
void exit_error(char *msg)
{
	fprintf(stderr, "%s %s\n", prog, msg);
	exit(EXIT_FAILURE);
}

/**
 * Program entry point.
 * @brief Implements the Cooley-Tukey Fast Fourier Transform algorithm.
 * Recursivly calls itself and aplies the "butterfly operation" to calculate 
 * the Fourier Transform 
 * @details Uses 4 pipes to communicate with its two children respectivly.
 * Recursive call is only performed if two or more input values are read.
 * @param argc The argument counter
 * @param argv The argument vector
 * @return returns EXIT_SUCCESS
 */
int main(int argc, char *argv[])
{
	prog = argv[0];
	int even_out[2];
	int even_in[2];
	int odd_out[2];
	int odd_in[2];

	int pid1, pid2;

	if (pipe(even_out) == -1) exit_error("error opening pipe");
	if (pipe(even_in) == -1) exit_error("error opening pipe");
	if (pipe(odd_out) == -1) exit_error("error opening pipe");
	if (pipe(odd_in) == -1) exit_error("error opening pipe");

	char buf[BUFSIZE];
	char buffer[2][BUFSIZE];

	memset(buffer[0], 0, sizeof(buffer[0]));
	memset(buffer[1], 0, sizeof(buffer[1]));

	int n = 0;
	while (fgets(buf, sizeof(buf), stdin) != NULL && strcmp(buf, "\n") != 0){
		
		strncpy(buffer[n % 2], buf, sizeof(buf)); 

		if ( n == 1){ 
			pid1 = fork();
			if (pid1 == -1) exit_error("error forking");
			if (pid1 == 0){ // child 1
				close_both_ends(odd_in);
				close_both_ends(odd_out);
				create_child(even_in, even_out);
			} 

			pid2 = fork();
			if (pid2 == -1) exit_error("error forking");
			if (pid2 == 0){ // child 2 
				close_both_ends(even_out);
				close_both_ends(even_in);
				create_child(odd_in, odd_out);
			} 

			if(close(even_in[OUTPUT]) == -1) exit_error("error closing");
			if(close(odd_in[OUTPUT])  == -1) exit_error("error closing");
			if(close(odd_out[INPUT])   == -1) exit_error("error closing");
			if(close(even_out[INPUT])  == -1) exit_error("error closing");
		}

		if ( n % 2 != 0){ // at least two values have been read
			if (write(odd_in[INPUT],  buffer[ODD], strlen(buffer[ODD])) == -1)
				exit_error("error writing");

			if (write(even_in[INPUT], buffer[EVEN], strlen(buffer[EVEN])) == -1)
				exit_error("error writing");
		}
		n++;
	}

	if (close(even_in[INPUT]) == -1) exit_error("error closing"); 
	if (close(odd_in[INPUT]) == -1) exit_error("error closing"); 

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

	if (read_child(even_out[OUTPUT], R_e, n/2) == -1) 
		exit_error("read_child failed");

	if(close(even_out[OUTPUT]) == -1) 
		exit_error("error closing");

	// read from odd child
	float complex *R_o = malloc(sizeof(float complex) * (n/2));
	if (R_o == NULL) 
		exit_error("malloc failed");

	if (read_child(odd_out[OUTPUT], R_o, n/2) == -1) 
		exit_error("read_child failed");

	if(close(odd_out[OUTPUT]) == -1) 
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
		fprintf(stdout, "%f %f*i\n", crealf(R[i]), cimagf(R[i]));
	}

	free(R);
	return EXIT_SUCCESS;
}