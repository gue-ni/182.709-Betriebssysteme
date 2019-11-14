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
	fprintf(socket, "GET %s HTTP/1.1\r\n"
					"Host: %s\r\n"
					"Connection: close\r\n\r\n", rq->resource, rq->hostname);
	fflush(socket);
}







void RESPONSE_HEADER(FILE *socket, response_header *rsp){
	fprintf(socket, 
		"HTTP/1.1 %d %s\r\n" 
		"Date: %s\r\n"
		"Content-Length: %d\r\n"
		"Connection: close\r\n"
		"\r\n", 
		rsp->http_err_code, rsp->http_err, rsp->date, rsp->content_len);
	fflush(socket);

}







