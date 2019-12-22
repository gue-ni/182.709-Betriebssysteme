#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "common.h"
#define SIZE 5

int main( int argc, char *argv[])
{
//	printf("sizeof circ_buf %ld\n", sizeof(struct circ_buf));

//	int a[SIZE];



	int j = 0;

	srand(getpid());

	j = 0;
	for (int i = 0; i < SIZE; i++){
		j = rand() % (i+1);
		printf("%d ", j);
	}
	printf("\n");

	for (int k = 0; k < SIZE; k++){
		printf("random between 0 and %d: ", k);
		for (int i = 0; i < 50; i++){
			j = rand() % (k+1);
			printf("%d ", j);
		}
		printf("\n");
	}
    return( EXIT_SUCCESS );
}


