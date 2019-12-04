#include <stdio.h>

// HTTP GET request-header
typedef struct {
	char *hostname;
	char *resource;
} request;

void EXIT_ERROR(char *error, char *prog);
void prog_usage(char *myprog);

