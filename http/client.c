/*
	@author: Jakob G. Maier <e11809618@student.tuwien.ac.at>
	@date: 2019-19-24
	@brief:
*/



/*

achtung: command injection mit &






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

int check_protocoll(char *url){
	return memcmp(url, "http://", 7);
}

void parse_url(request *get, char *url)
{
	char* resource = "/";
	char* hostname;
	
	if (check_protocoll(url))
	{
		printf("Does not start with http://\n");
		exit(EXIT_FAILURE);
	}

	url += 7; // remove http://

	int c;
	for (int i = 0; i < strlen(url); ++i)
	{
		c = url[i];

		if ( c == '/' || 
			 c == '&' || 
			 c == ';' || 
			 c == '?' ||
			 c == ':' ||
			 c == '@' ||
			 c == '&' ){

			hostname = malloc(i * sizeof(char));
			strncpy(hostname, url, i);

			resource = malloc(strlen(url) - i * sizeof(char));
			strcpy(resource, url += i);
			break;
		}
	}	

	if (hostname == NULL)
		hostname = url;

	//printf("hostname: %s\n", hostname);
	//printf("resource: %s\n", resource);

	get->resource = resource;
	get->hostname = hostname;
}

int main(int argc, char *argv[])
{
	int c;
	char *outfile = "NULL";
	char *directory = "NULL";
	char *port = "80";
	char *url = "localhost";

	// reads in command line arguments
	while( (c = getopt(argc, argv, "p:o:d:h")) != -1 ){
		switch( c ){
			case 'p':
				port = optarg;
				break;

			case 'o':
				outfile = optarg;
				break;

			case 'd':
				directory = optarg;
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

	request *rq = malloc(sizeof(request));

	parse_url(rq, url);

	struct addrinfo hints, *ai;
	memset(&hints, 0, sizeof hints);

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo(rq->hostname, port, &hints, &ai);

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

	FILE *sockfp = fdopen(sockfd, "r+");

	//out = get_out(outfile, directory);
	FILE *out = fopen("out.txt", "w");

	if(out == NULL) {
		fprintf(stderr, "fopen failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	GET(stdout, rq); // for debugging only
	GET(sockfp, rq);

	char buf[1024];
	
    while (fgets(buf, sizeof(buf), sockfp) != NULL){
		fputs(buf, stdout);
		fputs(buf, out);
		memset(buf, 0, sizeof(buf));
    } 
    

	printf("test\n");
    fclose(sockfp);
    fclose(out);

	return EXIT_SUCCESS;
}


