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



void parse_url(request *get_request, char *url)
{
	get_request->resource = "/";
	get_request->host = url;
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

//	printf("argc: %d, optind: %d\n", argc, optind);
	printf("%s%s\n", outfile, directory);
	// GET positional argument from cli	
	printf("optind: %d\n", optind);
	url = argv[optind];
	
	//TODO remove
	//printf("url: %s\nport: %s\n=====================\n", url, port);


	struct addrinfo hints, *ai;
	memset(&hints, 0, sizeof hints);

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo(url, port, &hints, &ai);


	int sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
	if (sockfd < 0)
	{
		// error
	}

	
	if ((connect(sockfd, ai->ai_addr, ai->ai_addrlen)) < 0)
	{
		// error
	}

	FILE *sockfp = fdopen(sockfd, "r+");

	//out = get_out(outfile, directory);
	FILE *out = fopen("out.txt", "w");

	if(out == NULL) {
		fprintf(stderr, "fopen failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}


	fputs("Hello\n", out);	
	fputs("World!\n", out);




	request *rq = malloc(sizeof(request));

	parse_url(rq, url);

	GET(stdout, rq);
	GET(sockfp, rq);

	char buf[1024];

    while (fgets(buf, sizeof(buf), sockfp) != NULL){
//		printf("\n================================\nRead %ld bytes\n================================\n", sizeof(buf));
		fputs(buf, stdout);
		memset(buf, 0, sizeof(buf));


    }



    fclose(sockfp);
    fclose(out);

	return EXIT_SUCCESS;
}


