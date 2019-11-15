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
					"Connection: close\r\n\r\n", rq->resource, rq->hostname);
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
		printf("Failed to open socket\n");
		exit(EXIT_FAILURE);
	}

	if ((connect(sockfd, ai->ai_addr, ai->ai_addrlen)) < 0)
	{
		printf("Failed to connect\n");
		exit(EXIT_FAILURE);
	}

	return fdopen(sockfd, "r+");
}

// returns 0 if it does not start with HTTP/1.1, else HTTP code
int check_response(char *buf){
		
	if (memcmp(buf, "HTTP/1.", 7) != 0) // TODO change this to 1.1
	{

		//printf("Protocol error!\n");
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
	char *rs = "/";
	char *hn;
	
	if (check_protocol(url))
	{
		printf("Does not start with http://\n");
		exit(EXIT_FAILURE);
	}

	url += 7; // remove http://

	int c;
	for (int i = 0; i < strlen(url); ++i)
	{
		c = url[i];

		if ( c == '/' || c == '&' || c == ';' ||  c == '?' 
			          || c == ':' || c == '@' || c == '&' ){

			// maybe call free?
			hn = malloc(i * sizeof(char));
		
			if (hn == NULL)
				exit(EXIT_FAILURE);

			strncpy(hn, url, i);

			rs = malloc(strlen(url) - i * sizeof(char));

			if (rs == NULL)
				exit(EXIT_FAILURE);

			strcpy(rs, url += i);
			break;
		}
	}	

	if (hn == NULL)
		hn = url;

	get->resource = rs;
	get->hostname = hn;

	//free(hn);  // this does not work. why???
	//free(rs);

}

FILE* parse_dir(char *dir, char *r){
	char *resource;
	char *directory;

	if (strcmp(r, "/") == 0)
	{
		resource = malloc(sizeof(char) * 11);

		if (resource == NULL)
			exit(EXIT_FAILURE);

		strcpy(resource, "/index.html");

	} else {
		resource = malloc(sizeof(char) * strlen(r));

		if (resource == NULL)
			exit(EXIT_FAILURE);

		strcpy(resource, r);
	}

	directory = malloc(sizeof(char) * strlen(dir));

	if (directory == NULL)
		exit(EXIT_FAILURE);

	if ((char) dir[strlen(dir) - 1] == '/')
	{
		directory = malloc(sizeof(char) * strlen(dir) - 1);

		if (directory == NULL)
			exit(EXIT_FAILURE);

		strncpy(directory, dir, strlen(dir) - 1);
	} else {
		strcpy(directory, dir);
	}

	char *path = malloc(sizeof(char) * (strlen(resource) + strlen(directory)));
	
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
				out_opt = 2;
				break;

			case 'd':
				directory = optarg;
				out_opt = 3;
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

	url = argv[optind];

	if (url == NULL)
	{	
		prog_usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	request *rq = malloc(sizeof(request));
	if (rq == NULL)
		exit(EXIT_FAILURE);

	parse_url(rq, url);

	struct addrinfo hints, *ai;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo(rq->hostname, port, &hints, &ai);

	FILE *fp, *sockfp = create_socket(ai);

	// chose output option
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
		fprintf(stderr, "fopen failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	GET(sockfp, rq);
	free(rq);
	fflush(sockfp);
    
	char buf[1024];
	int fl = 1, header = 1, status_code;

    while (fgets(buf, sizeof(buf), sockfp) != NULL){

    	if (fl)
    	{
//    		fputs(buf, fp); // TODO remove
    		status_code = check_response(buf);

    		if ( status_code == 0) 
    		{
    			fprintf(stderr, "Protocol error!\n");
    			exit(2);

    		} else if (status_code != 200){
	    		fprintf(stderr, "%s", buf+9);
	    		exit(3);

    		} else {
    			fl = 0;
    			continue;
    		}
    	}

    	if (header)
    	{
 //   		fputs(buf, fp); // TODO remove
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

    fclose(sockfp);
    fclose(fp);

	return EXIT_SUCCESS;
}


