/*
	@author: Jakob G. Maier <e11809618@student.tuwien.ac.at>
	@date: 2019-19-24
	@brief:
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>

void usage(char *myprog)
{
	fprintf(stderr, "Usage: %s [-p PORT] [-o FILE | -d DIR ] URL\n", myprog);
	exit(EXIT_FAILURE);
}

FILE *get_out(char *file, char *dir)
{
	if (file)
	{
		return fopen(file, "w");

	} else if (dir) {

		return fopen(dir, "w");

	} else {

		return fdopen(STDOUT_FILENO, "w");
	}
}

int main(int argc, char *argv[])
{
	int c;
	char *outfile = "NULL";
	char *directory = "NULL";
	int port = 80;
	char *url = "localhost";

	// reads in command line arguments
	while( (c = getopt(argc, argv, "p:o:d:h")) != -1 ){
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

	if (optind > argc-2)
	{
		url = argv[optind];
	}

	printf("%s:%d, writing to outfile %s or directory %s\n", url, port, outfile, directory);

	//++++++++++++++++++++++++++++++++++++++++++++

	struct addrinfo hints, *ai;

	memset(&hints, 0, sizeof hints);

	hints.ai_family = AF_INET;hints.ai_socktype = SOCK_STREAM;
	int res = getaddrinfo(url, port, &hints, &ai);


	if(res != 0) 
	{
		// error

	}

	int sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);

	if(sockfd < 0) 
	{
		// error
	}

	if(connect(sockfd, ai->ai_addr, ai->ai_addrlen) < 0) 
	{
		// error
	}

	freeaddrinfo(ai);


	return EXIT_SUCCESS;
}


