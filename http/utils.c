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



void GET(FILE *socket, request *rq)
{
	fprintf(socket, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", rq->resource, rq->host);
	fflush(socket);
}

void RESPONSE_HEADER(FILE *socket, response_header *rsp){
	fprintf(socket, 
		"HTTP/1.1 %d %s\r\n" 
		"Date: %s\r\n"
		"Content-Length: %d\r\n"
		"Connection: %s\r\n", 
		rsp->http_err_code, rsp->http_err, rsp->date, rsp->con_len, rsp->con_stat);
	fflush(socket);

}


void prog_usage(char *myprog)
{
	fprintf(stderr, "Usage: %s [-p PORT] [-i INDEX] DOC_ROOT\n", myprog);
	exit(EXIT_FAILURE);
}



int parse_request(char buf[])
{
	return 0;
}

