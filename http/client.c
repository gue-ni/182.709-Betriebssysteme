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


typedef struct {
	char *host;
	char *resource;
} request;

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

void GET(FILE *socket, char *host, char *file)
{
	fprintf(socket, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", file, host);
	fflush(socket);
}

void parse_url(request *get_request, char *url)
{
	get_request->resource = "/";
	get_request->host = url;
}

int main(int argc, char *argv[])
{
	int c;
	char *outfile = NULL;
	char *directory = NULL;
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
				usage(argv[0]);
				exit(EXIT_FAILURE);
				break;

			default:
				usage(argv[0]);
				exit(EXIT_FAILURE);
				break;
		}
	}

//	printf("argc: %d, optind: %d\n", argc, optind);

	// GET positional argument from cli	
	url = argv[optind];
	
	//TODO remove
	//printf("url: %s\nport: %s\n=====================\n", url, port);


	struct addrinfo hints, *ai;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	getaddrinfo(url, port, &hints, &ai);
	int sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
	connect(sockfd, ai->ai_addr, ai->ai_addrlen);
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

	GET(stdout, rq->host, rq->resource);
	GET(sockfp, rq->host, rq->resource);


	//char *buf = malloc(1024 * sizeof(char));
	char buf[1024];

    /*while (fgets(buf, sizeof(buf), sockfp) != NULL){
		fputs(buf, stdout);
        fputs(buf, out);
        break;
        //break;


    }*/

//	char buffer[2048];
    int x;
    //int y = 0;

	while ((x = recv(sockfd, buf, sizeof(buf), 0)) > 0) {
		printf("\n================================\nRead %d bytes\n================================\n", x);
	    fputs(buf, stdout);
	    //memcpy(buffer+y, buf, x);
	    //y = y+x;
	    memset(buf, 0, 1024);
	
	}


    //fputs(buffer, out);






    
    /*

    int pos; cnt;
    for (pos = 0; pos < sizeof(buf);){
    	cnt = read(sockfpd, buf + pos, sizeof(buf) - pos);

    	if 
    }*/

    fclose(sockfp);
    fclose(out);
//    free(buf);



	return EXIT_SUCCESS;
}


