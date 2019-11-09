#include <stdio.h>

#define ERROR_EXIT(...) { fprintf(stderr, "ERROR: " __VA_ARGS__); exit(EXIT_FAILURE); }

// HTTP request-header
typedef struct {
	char *hostname;
	char *resource;
} request;

// HTTP response-header
typedef struct {
	int http_err_code;
	char *http_err;
	char *date;
	int con_len;
	char *con_stat;
} response_header;

typedef struct {
	response_header *header;
	char *file_buffer;
} response;


void prog_usage(char *myprog);

// make GET request for file on socket with host  
//void GET(FILE *socket, char *host, char *file);
void GET(FILE *socket, request *rq);