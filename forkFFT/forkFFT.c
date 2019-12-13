/*
	@author: Jakob G. Maier <e11809618@student.tuwien.ac.at>
	@date: 2019-12-11 
	@brief: forkFFT 



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

	// TODO check out what's going wrong here
//	memset(buf, 0, sizeof(buf));
	memset(buffer[0], 0, sizeof(buffer[0]));
	memset(buffer[1], 0, sizeof(buffer[1]));

	int n = 0;
	while (fgets(buf, sizeof(buf), stdin) != NULL && strcmp(buf, "\n") != 0){

		if (n % 2 == 0){
			strncpy(buffer[EVEN], buf, 10);
		}else{
			strncpy(buffer[ODD], buf, 10);
		}

		if ( n == 1){ 
			pid1 = fork();
			check_error(pid1);
			if (pid1 == 0){ // child 1
				n = 0;
				create_child(even_P, even_R, odd_P, odd_R);
			} 

			pid2 = fork();
			check_error(pid2);
			if (pid2 == 0){ // child 2 
				n = 0;
				create_child(odd_P, odd_R, even_P, even_R);
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

	complex *R_e = malloc(sizeof(complex) * (n/2)); 
	if (R_e == NULL)
		exit_error("malloc failed");

	read_child(even_R[OUTPUT], R_e, n/2);
	if(close(even_R[OUTPUT]) == -1) 
		exit_error("error closing");

	complex *R_o = malloc(sizeof(complex) * (n/2));
	if (R_o == NULL)
		exit_error("malloc failed");

	read_child(odd_R[OUTPUT], R_o, n/2);
	if(close(odd_R[OUTPUT]) == -1) exit_error("error closing");
	
	complex *R = malloc(sizeof(complex) * n);
	if (R == NULL)
		exit_error("malloc failed");

	complex tmp, exp;
	float x;
	for (int k = 0; k < n/2; k++){
		x = (-(2 * PI) / n) * k;
		exp.a = cos(x);
		exp.b = sin(x);

		complex_mult(&tmp, R_o[k], exp);			
			
		R[k].a = R_e[k].a + tmp.a;
		R[k].b = R_e[k].b + tmp.b;
		R[k+n/2].a = R_e[k].a - tmp.a;	
		R[k+n/2].b = R_e[k].b - tmp.b;	
	}
	
	free(R_e);
	free(R_o);

	for (int i = 0; i < n; i++){
		print_complex(R[i]);
	}

	free(R);
	return EXIT_SUCCESS;
}

