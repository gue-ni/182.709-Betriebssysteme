#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#define BILLION 1E9

int main( int argc, char *argv[])
{

	struct timespec start, stop;
	double accum;

	clock_gettime(CLOCK_REALTIME, &start);

	int fac = 0;
	for (int e = 0; e < 1E6; e++){
		for (int i = 1; i < 100; i++){
			fac *= i;
		}

	}

	clock_gettime(CLOCK_REALTIME, &stop);

	accum = ( stop.tv_sec - start.tv_sec )
          + ( stop.tv_nsec - start.tv_nsec )
            / BILLION;
    printf( "%lf\n", accum );


    return(EXIT_SUCCESS);
}


