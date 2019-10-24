/*
	@author: Jakob G. Maier <e11809618@student.tuwien.ac.at>
	@date: 2019-19-24
	@brief:
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  

void usage(char *myprog)
{
	fprintf(stderr, "Usage: %s [-p PORT] [-i INDEX] DOC_ROOT\n", myprog);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	int c;
	char *index = "index.html";
	int port = 8080;

	// reads in command line arguments
	while( (c = getopt(argc, argv, "p:i:h")) != -1 ){
		switch( c ){
			case 'p':
				port = (int) strtol(optarg, NULL, 0);
				break;

			case 'i':
				index = optarg;
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

	printf("%s\n", index);
	printf("%d\n", port);

	return EXIT_SUCCESS;
}


