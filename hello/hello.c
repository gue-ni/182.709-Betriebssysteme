#include "hello.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  


void say_hello(const char *name)
{
	printf("Hello %s!\n", name);
}

void usage(char *myprog)
{
	fprintf(stderr, "Usage: %s [-a file] file\n", myprog);
	exit(EXIT_FAILURE);
}


