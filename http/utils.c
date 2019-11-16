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

void EXIT_ERROR(char *error, char *prog)
{
	fprintf(stderr, "%s: ERROR: %s\n", prog, error);
	exit(EXIT_FAILURE);
}












