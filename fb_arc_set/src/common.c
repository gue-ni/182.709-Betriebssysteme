/**
 * @file common.c
 * @author Jakob G. Maier <e11809618@student.tuwien.ac.at>
 * @date
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "common.h"

void exit_error(char *p, char *msg)
{
    fprintf(stderr, "%s %s\n", p, msg);
    exit(EXIT_FAILURE);
}