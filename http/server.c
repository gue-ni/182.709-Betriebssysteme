/*
	@author: Jakob G. Maier <e11809618@student.tuwien.ac.at>
	@date: 2019-11-17 
	@brief: A simple http server
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
#include <assert.h>
#include <time.h>
#include <signal.h>

#define B_BUFFER 1024
#define S_BUFFER 256

int run = 1;
struct addrinfo hints, *ai;
char *prog;

void prog_usage(char *prog_name)
{
	fprintf(stderr, "Usage: %s [-p PORT] [-i INDEX] DOC_ROOT\n", prog_name);
	exit(EXIT_FAILURE);
}

int listen_socket(struct addrinfo *ai)
{
	int socket_fd = socket(ai->ai_family, ai->ai_socktype,ai->ai_protocol);

	if(socket_fd < 0){ 
		freeaddrinfo(ai);
		EXIT_ERROR("create socket failed", prog);
	}

	int optval = 1;
	setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

	if(bind(socket_fd, ai->ai_addr, ai->ai_addrlen) < 0){
		freeaddrinfo(ai);
		EXIT_ERROR("bind socket failed", prog);
	}

	if (listen(socket_fd, 10) < 0){
		freeaddrinfo(ai);
		EXIT_ERROR("listen socket failed", prog);
	}

	 return socket_fd;
}

void respond_error(FILE *socket, int code, char* reason_phrase)
{
	fprintf(socket, "HTTP/1.1 %d %s\r"
					"\n\r\n", 
					code, reason_phrase);
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

size_t rfc822_time(char *buf, size_t size)
{
	time_t t;
	time(&t);
	struct tm *info = gmtime(&t);
	return strftime(buf, size,"%a, %d %b %g %T GMT", info);
}

void respond_ok(FILE *socket, int content_length)
{
	char time_buf[30] = {0};
	rfc822_time(time_buf, 30);

	int code = 200;
	char *reason_phrase = "OK";

	fprintf(socket, 
			"HTTP/1.1 %d %s\r\n"
			"Date: %s\r\n"
			"Content-Length: %d\r\n"
			"Connection: close\r\n"
			"\r\n",
			code, reason_phrase, time_buf ,content_length);
	fflush(socket);
}

int parse_path(char *path, FILE **resource, char *index, char *doc_root)
{
	char *file_path;

	if (doc_root[strlen(doc_root) - 1] == '/')
		doc_root[strlen(doc_root) - 1] = '\0';

	if (memcmp(path, "\0", 1) == 0){
		file_path = index;
	} else {
		file_path = path;
	}

	size_t fp_size = strlen(doc_root) + strlen("/") + strlen(file_path) + 1; // one for \0 byte

	char *full_path = calloc(fp_size * sizeof(char), 1);

	strcpy(full_path, doc_root);

	if (strlen(doc_root) > 0) // current dir could be doc_root
		strcat(full_path, "/");

	strcat(full_path, file_path);

	int status_code;

	if ( (*resource = fopen(full_path, "r") ) != NULL)
	{
		status_code = 200;
	} else {
		status_code = 404;
	}

	free(full_path);
	return status_code;
}

int parse_request(char *rq, char **path, FILE **resource)
{
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
	
	return 0; 
}

void exit_immediatly(int num)
{
	freeaddrinfo(ai);
	exit(EXIT_SUCCESS);
}

void complete_request(int num)
{
	run = 0;
}

int main(int argc, char *argv[])
{
	signal(SIGINT, exit_immediatly);
	signal(SIGTERM, exit_immediatly);

	int c;
	char *port = "8080";
	char *index = "index.html";
	char *doc_root = "\0";

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

	prog = argv[0];

	if (optind < argc)
		doc_root = argv[optind];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int res = getaddrinfo(NULL, port, &hints, &ai);
	if(res != 0) 
	{
		EXIT_ERROR("problem with getaddrinfo", prog);	
	}

	int socket_fd = listen_socket(ai);
	int connection_fd; 

	FILE *connection = NULL, *resource = NULL;

  	long content_length;	

	char buf[B_BUFFER], h_buf[S_BUFFER], f_buf[B_BUFFER];

	while (run){
		signal(SIGINT, exit_immediatly);
		signal(SIGTERM, exit_immediatly);

		if ((connection_fd = accept(socket_fd, ai->ai_addr, &ai->ai_addrlen)) < 0){
				exit(EXIT_FAILURE);
		}

		signal(SIGINT, complete_request);
		signal(SIGTERM, complete_request);

		connection = fdopen(connection_fd, "r+");
		resource = NULL;

		int fl = 1;
	   	while(fgets(h_buf, sizeof(h_buf), connection) != NULL)
	   	{
	   		if (fl){ // we only care about the first line
	   			memcpy(buf, h_buf, S_BUFFER); 
	   			fl = 0;
	   		}

	   		if (memcmp(h_buf, "\r\n", 2) == 0) // check for end of header
	   			break;
	   	}

	   	size_t ps = 1 + strlen(buf);
	   	char *path = calloc(ps * sizeof(char), sizeof(char)); 

	   	// returns 0, 400 or 501
	   	int status_code = parse_request(buf, &path, &resource);

	   	if (status_code == 0) // request looks ok, try to open requested file
	   		status_code = parse_path(path, &resource, index, doc_root);

		switch(status_code){
			case (200):
				content_length = get_file_size(resource);				

				respond_ok(connection, content_length);

				while(fgets(f_buf, sizeof(f_buf), resource) != NULL)
				{
					fputs(f_buf, connection);
					memset(f_buf, 0, B_BUFFER);
				}
				fflush(connection);
				break;

			case (400):
				respond_error(connection, status_code, "Bad Request");
				break;

			case (404):
				respond_error(connection, status_code, "Not found");
				break;

			case (501):
				respond_error(connection, status_code, "Not implemented");
				break;

			default:
				assert(0);
				break;
		}

		fclose(connection);

		free(path);

		if (resource != NULL)
			fclose(resource);

		memset(buf,   0, B_BUFFER);
		memset(h_buf, 0, S_BUFFER);
		memset(f_buf, 0, B_BUFFER);	
	}

	freeaddrinfo(ai); 
	return EXIT_SUCCESS;
}


