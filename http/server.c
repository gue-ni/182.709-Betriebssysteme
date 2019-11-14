/*
	@author: Jakob G. Maier <e11809618@student.tuwien.ac.at>
	@date: 2019-19-24
	@brief:
*/
#include "utils.h"
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
#include <assert.h>

void prog_usage(char *myprog)
{
	fprintf(stderr, "Usage: %s [-p PORT] [-i INDEX] DOC_ROOT\n", myprog);
	exit(EXIT_FAILURE);
}

FILE* listen_socket(struct addrinfo *ai)
{
	int sockfd = socket(ai->ai_family, ai->ai_socktype,ai->ai_protocol);

	if(sockfd < 0) 
	{
		// error
	}

	int optval = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

	if(bind(sockfd, ai->ai_addr, ai->ai_addrlen) < 0) 
	{
		// error

	}

	if (listen(sockfd, 10) < 0)
	{
		// error
	}

	int new_socket_fd; 

	if ((new_socket_fd = accept(sockfd, ai->ai_addr, &ai->ai_addrlen)) < 0){
		// error
	}

	return fdopen(new_socket_fd, "r+");
}

void respond_error(FILE *socket, int code, char* reason_phrase)
{
	fprintf(socket, "HTTP/1.1 %d %s\r\n\r\n", code, reason_phrase);
	fflush(socket);
}

void respond_ok(FILE *socket)
{
	fprintf(socket, "HTTP/1.1 200 OK\r\n\r\n");
	fflush(socket);
}

int parse_path(char *path, FILE **resource)
{
	if ( (*resource = fopen(path, "r")) != NULL)
	{
		return 200;
	} else {
		return 404;
	}
}

int parse_request(char *rq, char **path)
{
	if (memcmp(rq, "GET", 3) != 0)
		return 501;

	rq += 4;

	int c;
	for (int i = 0; i < strlen(rq); ++i)
	{
		c = rq[i];
		if(c == ' ')
		{
			*path = malloc(i * sizeof(char));
			if (*path == NULL)
				fprintf(stderr, "%s\n", "error");

			strncpy(*path, rq, i);
			rq += (++i);
			break;
		}
	}


	if (memcmp(rq, "HTTP/1.1", 8) != 0)
		return 400;
	
	return 0;
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
				prog_usage(argv[0]);
				exit(EXIT_FAILURE);
				break;

			default:
				prog_usage(argv[0]);
				exit(EXIT_FAILURE);
				break;
		}
	}

	printf("Port: %s\n", port);


	struct addrinfo hints, *ai;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int res = getaddrinfo(NULL, port, &hints, &ai);
	if(res != 0) 
	{
		fprintf(stderr, "%s\n", "error");
	}


		int sockfd = socket(ai->ai_family, ai->ai_socktype,ai->ai_protocol);

	if(sockfd < 0) 
	{
		// error
	}

	int optval = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

	if(bind(sockfd, ai->ai_addr, ai->ai_addrlen) < 0) 
	{
		// error

	}

	if (listen(sockfd, 10) < 0)
	{
		// error
	}

	int new_socket_fd; 
	FILE *connection;
	FILE *resource;

	while (1){
		if ((new_socket_fd = accept(sockfd, ai->ai_addr, &ai->ai_addrlen)) < 0){
				// error
		}
 
		connection = fdopen(new_socket_fd, "r+");
		resource = NULL;

		char buf[512];

	   	if(fgets(buf, sizeof(buf), connection) == NULL)
	   		fprintf(stderr, "%s\n", "error");

	   	fprintf(stderr, "Request: %s\n", buf);

	   	char *path = NULL; // would cause error otherwise

	   	int status_code = 200;	
//	   	status_code = parse_request(buf, &path);
	   	//status_code = parse_path(path, &resource);
	   	//fprintf(stderr, "Status code: %d\n", status_code);

		printf("Responding with:\n");
		switch(status_code){
			case (200):
				respond_ok(connection);
				respond_ok(stderr);
				break;

			case (400):
				respond_error(stdout, status_code, "Bad Request\n");
				respond_error(connection, status_code, "Bad Request");
				break;

			case (404):
				respond_error(stdout, status_code, "Not found\n");
				respond_error(connection, status_code, "Not found");
				break;

			case (501):
				respond_error(stdout, status_code, "Not implemented\n");
				respond_error(connection, status_code, "Not implemented");
				break;

			default:
				assert(0);
				break;
		}
		//printf("Closing\n");	
		//fclose(connection);
		printf("\n########################################\n");			
		memset(buf, 0, sizeof(buf));

	}


	freeaddrinfo(ai);

	return EXIT_SUCCESS;
}


