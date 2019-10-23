/*
	@author: Jakob G. Maier 11809618
	@date: 18.10.19
	@details:
	@brief:
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  

void usage(char *myprog)
{
	fprintf(stderr, "Usage: %s [-p PORT] [-o FILE | -d DIR ] URL\n", myprog);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	int c;
	char *outfile = "NULL";
	char *directory = "NULL";
	int port = 80;

	// reads in command line arguments
	while( (c = getopt(argc, argv, "p:o:d:")) != -1 ){
		switch( c ){
			case 'p':
				port = (int) strtol(optarg, NULL, 0);
				break;

			case 'o':
				outfile = optarg;
				break;

			case 'd':
				directory = optarg;
				break;

			case 'h':
				usage(argv[0]);
				exit(EXIT_FAILURE);
				break;

			default:
				usage(argv[0]);
				exit(EXIT_FAILURE);
				break;
		}
	}

	printf("%s\n", outfile);
	printf("%s\n", directory);
	printf("%d\n", port);


	return EXIT_SUCCESS;
}


