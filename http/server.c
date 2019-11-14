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
#include <time.h>

void prog_usage(char *myprog)
{
	fprintf(stderr, "Usage: %s [-p PORT] [-i INDEX] DOC_ROOT\n", myprog);
	exit(EXIT_FAILURE);
}

int listen_socket(struct addrinfo *ai)
{
	int sockfd = socket(ai->ai_family, ai->ai_socktype,ai->ai_protocol);

	if(sockfd < 0) 
		exit(EXIT_FAILURE);

	int optval = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

	if(bind(sockfd, ai->ai_addr, ai->ai_addrlen) < 0) 
		exit(EXIT_FAILURE);

	if (listen(sockfd, 10) < 0)
		exit(EXIT_FAILURE);

	 return sockfd;
}

void respond_error(FILE *socket, int code, char* reason_phrase)
{
	fprintf(socket, "HTTP/1.1 %d %s\r\n", code, reason_phrase);
	fflush(socket);
}

long get_file_size(FILE *f)
{
	long size;
	fseek(f, 0, SEEK_END);
	size  = ftell(f);
	fseek(f, 0, SEEK_SET);	
	return size;
}

void rfc822_time(char *time_buf)
{
	time_t t;
	time(&t);
	struct tm *info;
	info = gmtime(&t);
	strftime(time_buf, sizeof(time_buf),"%a, %d %b %g %T GMT", info);
}

void respond_ok(FILE *socket, int content_length)
{
	char time_buf[30] = {0};
	
	time_t t;
	time(&t);
	struct tm *info;
	info = gmtime(&t);
	strftime(time_buf, sizeof(time_buf),"%a, %d %b %g %T GMT", info);
	
	// why doesn't this work?
	//rfc822_time(time_buf);

	fprintf(socket, 
			"HTTP/1.1 200 OK\r\n"
			"Date: %s\r\n"
			"Content-Length: %d\r\n"
			"Connection: close\r\n"
			"\r\n"
			,time_buf ,content_length);
	fflush(socket);
}

int parse_path(char *path, FILE **resource)
{
	path += 1;
	printf("Path: %s\n", path);

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

//			memset(path, 0, sizeof(path)); causes error
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

	fprintf(stderr, "Server started, listening on %s\n", port);
	fprintf(stderr, "%s\n", "########################################");

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

	int sockfd = listen_socket(ai);

	int new_socket_fd; 
	FILE *connection = NULL;
	FILE *resource = NULL;

	while (1){
		if ((new_socket_fd = accept(sockfd, ai->ai_addr, &ai->ai_addrlen)) < 0){
				// error
				exit(EXIT_FAILURE);
		}

		connection = fdopen(new_socket_fd, "r+");
		resource = NULL;

		char buf[512];
		char f_buf[1024];

	   	if(fgets(buf, sizeof(buf), connection) == NULL)
	   		fprintf(stderr, "%s\n", "error");

	   	fprintf(stderr, "\n%s\n", buf);

	   	char *path = NULL; // would cause error otherwise

	   	int status_code; 
	   	long content_length;	
	   	status_code = parse_request(buf, &path);
	   	status_code = parse_path(path, &resource);

		printf("\n");
		switch(status_code){
			case (200):
				content_length = get_file_size(resource);				

				respond_ok(connection, content_length);
				respond_ok(stderr, content_length);

				while(fgets(f_buf, sizeof(f_buf), resource) != NULL)
				{
					fputs(f_buf, connection);
					fputs(f_buf, stdout);
					memset(f_buf, 0, sizeof(f_buf));
				}
				fflush(connection);
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
		fclose(connection);
		if (resource != NULL)
			fclose(resource);
		printf("\n########################################\n");			
		memset(buf, 0, sizeof(buf));
	}

	freeaddrinfo(ai);

	return EXIT_SUCCESS;
}


