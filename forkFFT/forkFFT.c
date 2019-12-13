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
#include "childIO.h"

int main(int argc, char *argv[])
{
	int rec = 1;
	if (argc > 1){
		rec = 1;
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
		exit_error("Did not read anything");
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

	//fprintf(stderr, "(%d) reading from even\n", getpid());	
	complex *R_e = malloc(sizeof(complex) * (n/2)); 
	read_child(even_R[OUTPUT], R_e, n/2);
	close(even_R[OUTPUT]);

	//fprintf(stderr, "(%d) reading from odd\n", getpid());	
	complex *R_o = malloc(sizeof(complex) * (n/2));
	read_child(odd_R[OUTPUT], R_o, n/2);
	close(odd_R[OUTPUT]);

	if (0 && n > 1){
		if (1)fprintf(stderr, "(%d) print even:\n", getpid());
		for (int i = 0; i < n/2; i++){
			if (1) print_complex_err(R_e[i]);
		}
	}

	if (0 && n > 1){
		fprintf(stderr, "(%d) print odd:\n", getpid());
		for (int i = 0; i < n/2; i++){
			if (1) print_complex_err(R_o[i]);
		} 
	}

	complex *R = malloc(sizeof(complex) * n);
	
	complex tmp, exp;
	float x;
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
		if (0) fprintf(stderr, "(%d) writing calculated output: %f %f*i\n", getpid(), R[i].a, R[i].b);
		print_complex(R[i]);
	}

	free(R);
	return EXIT_SUCCESS;
}

