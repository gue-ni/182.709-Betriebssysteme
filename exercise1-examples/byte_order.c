/**
 * @author Michael Platzer <michael.platzer@tuwien.ac.at>
 * @date 2019-03-21
 *
 * @brief Demo program for understanding byte order
 *
 * Execute this program, then view the content of 'data' with hexdump:
 *
 *     hexdump -C data
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define ERROR_EXIT(...) { fprintf(stderr, "ERROR: " __VA_ARGS__); exit(EXIT_FAILURE); }

int main()
{
    FILE *out;

    if ((out = fopen("data", "w")) == NULL)
        ERROR_EXIT("fopen: %s\n", strerror(errno));

    int i = 80;
    fwrite(&i, 1, sizeof(int), out);
    fclose(out);
}
