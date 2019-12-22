#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

int main( int argc, char *argv[])
{

	struct timespec start, stop;
	double accum;
	double sum;


	long int fac;

	for (int j = 0; j < 15; j++){
		clock_gettime(CLOCK_REALTIME, &start);

		for (int e = 0; e < 1E7; e++){
			fac = 1;
			for (int i = 1; i < 10; i++){
				fac *= i;
			}

		}

		clock_gettime(CLOCK_REALTIME, &stop);

		accum = ( stop.tv_sec - start.tv_sec )
	          + ( stop.tv_nsec - start.tv_nsec )
	            / 1E9;

	    printf("%lf\n", accum);

	    sum += accum;




	}


	
    printf( "average: %lf\n", sum / 15);

    return(EXIT_SUCCESS);
}


