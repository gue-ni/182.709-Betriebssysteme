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

char *prog;

void prog_usage(char *myprog)
{
	fprintf(stderr, "Usage: %s [-p PORT] [ -o FILE | -d DIR ] URL\n", myprog);
	exit(EXIT_FAILURE);
}

void POST(FILE *socket, request *rq)
{
	fprintf(socket, "POST %s HTTP/1.1\r\n"
					, rq->resource);
	fflush(socket);
}

void GET(FILE *socket, request *rq)
{
	fprintf(socket, "GET %s HTTP/1.1\r\n"
					"Host: %s\r\n"
					"Connection: close\r\n"
					"\r\n", rq->resource, rq->hostname);
	fflush(socket);
}

int check_protocol(char *url){

	return memcmp(url, "http://", 7);
}

FILE* create_socket(struct addrinfo *ai)
{
	int sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
	if (sockfd < 0)
	{
		EXIT_ERROR("Failed to open socket", prog);
	}

	if ((connect(sockfd, ai->ai_addr, ai->ai_addrlen)) < 0)
	{
		EXIT_ERROR("Failed to connect", prog);
	}

	return fdopen(sockfd, "r+");
}

// returns 0 if it does not start with HTTP/1.1, else HTTP code
int check_response(char *buf){
		
	if (memcmp(buf, "HTTP/1.", 7) != 0) // TODO change this to 1.1
	{
		return 0;
	} else {
		buf += 8;
	}

	int status_code = strtol(buf, NULL, 10);
	
	if (status_code < 600 && status_code >= 100)
	{
		return status_code;
	} else {
		return 0;
	}
}

void parse_url(request *get, char *url)
{
	char *rs = NULL;
	char *hn = NULL;
	
	if (check_protocol(url))
	{
		EXIT_ERROR("Does not start with http://", prog);
	}

	url += 7; // remove http://

	int c;
	for (int i = 0; i < strlen(url); ++i)
	{
		c = url[i];

		if ( c == '/' || c == '&' || c == ';' ||  c == '?' 
			          || c == ':' || c == '@' || c == '&' ){

			hn = malloc((i + 1) * sizeof(char)); // leave space for null byte
		
			if (hn == NULL)
				exit(EXIT_FAILURE);

			strncpy(hn, url, i);
			
			rs = malloc(strlen(url) - i + 1 * sizeof(char));

			if (rs == NULL)
				EXIT_ERROR("malloc failed", prog);

			strcpy(rs, url += i);
			break;
		}
	}	

	if (hn == NULL)
		hn = url;

	if (rs == NULL)
		EXIT_ERROR("problem with url", prog);

	get->resource = rs;
	get->hostname = hn;
}

FILE* parse_dir(char *dir, char *r){
	char *resource;
	char *directory;

	if (strcmp(r, "/") == 0)
	{
		resource = malloc(sizeof(char) * 11);

		if (resource == NULL)
			EXIT_ERROR("malloc failed", prog);

		strcpy(resource, "/index.html");

	} else {
		resource = malloc(sizeof(char) * strlen(r));

		if (resource == NULL)
			exit(EXIT_FAILURE);

		strcpy(resource, r);
	}

	if ((char) dir[strlen(dir) - 1] == '/')
	{
		directory = calloc(sizeof(char) * strlen(dir), 1);

		if (directory == NULL)
			exit(EXIT_FAILURE);

		strncpy(directory, dir, strlen(dir) - 1);
	} else {
		//strcpy(directory, dir);
		directory = dir;
	}

	char *path = malloc(sizeof(char) * (sizeof(*resource) + sizeof(*directory) - 1));
	
	if (path == NULL)
		exit(EXIT_FAILURE);

	strcat(path, directory);
	strcat(path, resource);

	free(resource);
	free(directory);

	return fopen(path, "w");
}

int main(int argc, char *argv[])
{
	int c, out_opt = 1;

	char *port = "80";
	char *outfile = NULL;
	char *directory = NULL;
	char *url = NULL;

	// reads in command line arguments
	while( (c = getopt(argc, argv, "p:o:d:h")) != -1 ){
		switch( c ){
			case 'p':
				port = optarg;
				break;

			case 'o':
				outfile = optarg;
				out_opt += 1;
				break;

			case 'd':
				directory = optarg;
				out_opt += 2;
				break;

			case 'h':
				prog_usage(argv[0]);
				break;

			default:
				prog_usage(argv[0]);
				break;
		}
	}
	prog = argv[0];
	url = argv[optind];

	if (out_opt > 3)
		prog_usage(argv[0]);

	if (url == NULL)
		prog_usage(argv[0]);

	request *rq = malloc(sizeof(request));
	if (rq == NULL)
		exit(EXIT_FAILURE);

	parse_url(rq, url);

	printf("hn: %s\nrs: %s\n", rq->hostname, rq->resource);

	struct addrinfo hints, *ai;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo(rq->hostname, port, &hints, &ai);

	FILE *fp, *sockfp = create_socket(ai);

	// chose output opt3ion
	switch ( out_opt ){
		case (1):
			fp = fdopen(out_opt, "w"); // stdout
			break;

		case (2):
			fp = fopen(outfile, "w");
			break;

		case (3):
			fp = parse_dir(directory, rq->resource);
			break;

		default:
			assert(0);
	}

	if(fp == NULL) 
	{
		EXIT_ERROR("Failed to open file", argv[0]);
	}

	GET(sockfp, rq);
	free(rq->hostname);
	free(rq->resource);
	free(rq);
	fflush(sockfp);
    
	char buf[1024];
	int fl = 1, header = 1, status_code;

    while (fgets(buf, sizeof(buf), sockfp) != NULL){

    	if (fl)
    	{
    		status_code = check_response(buf);

    		if ( status_code == 0) 
    		{
    			fprintf(stderr, "%s Protocol error!\n", argv[0]);
    			exit(2);

    		} else if (status_code != 200){
	    		fprintf(stderr, "%s ERROR %s", argv[0], buf+9);
	    		exit(3);

    		} else {
    			fl = 0;
    			continue;
    		}
    	}

    	if (header)
    	{
    		if (memcmp(buf, "\r\n", 2) == 0) // check for end of header
    		{
    			header = 0;
    			continue;
    		} else {
    			continue;
    		}
    	}

		fputs(buf, fp);
		memset(buf, 0, sizeof(buf));
    } 

   	freeaddrinfo(ai);
    fclose(sockfp);
    fclose(fp);

	return EXIT_SUCCESS;
}


