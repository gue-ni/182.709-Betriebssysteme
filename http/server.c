/*
	@author: Jakob G. Maier <e11809618@student.tuwien.ac.at>
	@date: 2019-19-24
	@brief:

	TODO:
	add signal handling
	signal handling should work, add clean up
	improve memory safety
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
#include <signal.h>

int run;

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

int parse_path(char *path, FILE **resource, char *index, char *doc_root)
{
	// TODO prepend dir
	// TODO / == index.html
	// TODO fix buffer overflow
//	printf("Parsing path!\n"); 

	char *file_path;
	char *full_path;

	if (doc_root[strlen(doc_root) - 1] != '/')
	{
		printf("does not end with /: %s, %c\n", doc_root, doc_root[strlen(doc_root) - 1]);
		char *tmp = malloc(sizeof(*doc_root) + 1);
		strcpy(tmp, doc_root);
		strcat(tmp, "/");
		doc_root = tmp;
	}
	
	if (memcmp(path, "\000", 1) == 0){
		file_path = index;

	} else {
		file_path = path;
	}
	full_path = malloc(sizeof(char) * (sizeof(doc_root) + sizeof(file_path)));
	if (full_path == NULL)
	{
		exit(EXIT_FAILURE);
	}

	memset(full_path, 0, sizeof(*full_path));
	strcat(full_path, doc_root);
	strcat(full_path, file_path);

	free(doc_root);

	printf("Opening file: %s\n", full_path);
	if ( (*resource = fopen(full_path, "r") ) != NULL)
	{
		free(full_path);
		return 200;
	} else {
		free(full_path);
		return 404;
	}
}

int parse_request(char *rq, char **path, FILE **resource)
{
//	printf("Parsing Request!\n");

	// request type not implemented
	if (memcmp(rq, "GET", 3) != 0){
		return 501;
	}

	rq += 5; // cut away "GET /"

	int c;
	for (int i = 0; i < strlen(rq); ++i)
	{
		c = rq[i];
		if(c == ' ')
		{
			strncpy(*path, rq, i);
			rq += (++i);
			break;
		}
	}

	if (memcmp(rq, "HTTP/1.1", 8) != 0)
		return 400;
	
	//return parse_path(*path, resource); // returns either 200 or 404
	return 0; 
}

void exit_immediatly(int num)
{
	write(STDOUT_FILENO, "\nexit\n", 7);
	exit(EXIT_SUCCESS);
}

void complete_request(int num)
{
	write(STDOUT_FILENO, "\ncomplete request\n", 18);
	run = 0;
}

int main(int argc, char *argv[])
{
	signal(SIGINT, exit_immediatly);
	signal(SIGTERM, exit_immediatly);

	int c;
	char *port = "8080";
	char *index = "index.html";
	char *doc_root = "";

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
	if (optind < argc)
		doc_root = argv[optind];

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
	int connection_fd, status_code, fl; 

  	long content_length;	

	FILE *connection = NULL, *resource = NULL;

	char *path = malloc(sizeof(char) * 64); 

	char buf[1024], h_buf[256], f_buf[1024];

	run = 1;

	while (run){
		signal(SIGINT, exit_immediatly);
		signal(SIGTERM, exit_immediatly);

		if ((connection_fd = accept(sockfd, ai->ai_addr, &ai->ai_addrlen)) < 0){
				// error
				exit(EXIT_FAILURE);
		}

		signal(SIGINT, complete_request);
		signal(SIGTERM, complete_request);

		connection = fdopen(connection_fd, "r+");
		resource = NULL;

		fl = 1;
	   	while(fgets(h_buf, sizeof(h_buf), connection) != NULL)
	   	{
//	   		fprintf(stderr, "%s", h_buf);

	   		if (fl){
	   			memcpy(buf, h_buf, sizeof(h_buf)); // is this correct 
	   			fl = 0;
	   		}

	   		if (memcmp(h_buf, "\r\n", 2) == 0) // end of header
	   			break;
	   	}

	   	status_code = parse_request(buf, &path, &resource);
	   	if (status_code == 0)
	   	{
	   		status_code = parse_path(path, &resource, index, doc_root);
	   	}

		switch(status_code){
			case (200):
				content_length = get_file_size(resource);				

				respond_ok(connection, content_length);
				respond_ok(stderr, content_length); // TODO remove

				while(fgets(f_buf, sizeof(f_buf), resource) != NULL)
				{
					fputs(f_buf, connection);
//					fputs(f_buf, stdout); // TODO remove
					memset(f_buf, 0, sizeof(f_buf));
				}
				fflush(connection);
				break;

			case (400):
				respond_error(stderr, status_code, "Bad Request");  // TODO remove
				respond_error(connection, status_code, "Bad Request");
				break;

			case (404):
				respond_error(stderr, status_code, "Not found"); // TODO remove
				respond_error(connection, status_code, "Not found");
				break;

			case (501):
				respond_error(stderr, status_code, "Not implemented"); // TODO remove
				respond_error(connection, status_code, "Not implemented");
				break;

			default:
				assert(0);
				break;
		}

		fclose(connection);
		if (resource != NULL)
			fclose(resource);

		memset(buf, 0, sizeof(buf));
		memset(h_buf, 0, sizeof(h_buf));
		memset(f_buf, 0, sizeof(f_buf));	
		memset(path, 0, sizeof(*path));

//		printf("\n########################################\n");			
	}

	freeaddrinfo(ai);
	free(path);

	return EXIT_SUCCESS;
}


