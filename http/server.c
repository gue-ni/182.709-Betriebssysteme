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
	fprintf(stderr, "Usage: %s [-p PORT] [-i INDEX] DOC_ROOT\n", myprog);
	exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	int c;
	char *index = "index.html";
	char *port = "8080";

	// reads in command line arguments
	while( (c = getopt(argc, argv, "p:i:h")) != -1 ){
		switch( c ){
			case 'p':
				port = optarg;
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
	printf("%s\n", port);


	struct addrinfo hints, *ai;
	memset(&hints, 0, sizeof hints);


	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int res = getaddrinfo( NULL, port, &hints, &ai);
	if(res != 0) 
	{
		// error
	}

	int sockfd = socket(ai->ai_family, ai->ai_socktype,ai->ai_protocol);

	if(sockfd < 0) 
	{
		// error
	}

	if(bind(sockfd, ai->ai_addr, ai->ai_addrlen) < 0) 
	{
		// error

	}

	if (listen(sockfd, 10) < 0)
	{
		// error
	}

	int new_socket; 

	if ((new_socket = accept(sockfd, ai->ai_addr, &ai->ai_addrlen)) < 0){
		// error
	}

	char buf[1024];

    while (read(new_socket, buf, sizeof(buf)) > 0){
		fputs(buf, stdout);


    }

	freeaddrinfo(ai);

	return EXIT_SUCCESS;
}


