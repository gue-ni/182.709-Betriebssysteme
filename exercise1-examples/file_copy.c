/**
 * @author Michael Platzer <michael.platzer@tuwien.ac.at>
 * @date 2019-03-21
 *
 * @brief Demo program reading a text file line by line
 *        and writing each line to another file
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define ERROR_EXIT(...) { fprintf(stderr, "ERROR: " __VA_ARGS__); exit(EXIT_FAILURE); }

int main()
{
    char buffer[1024];
    FILE *in, *out;

    if ((in = fopen("input.txt", "r")) == NULL)
        ERROR_EXIT("fopen input: %s\n", strerror(errno));

    if ((out = fopen("output.txt", "w")) == NULL)
        ERROR_EXIT("fopen output: %s\n", strerror(errno));

    while (fgets(buffer, sizeof(buffer), in) != NULL) {
        if (fputs(buffer, out) == EOF)
            ERROR_EXIT("fputs: %s\n", strerror(errno));
    }

    if (ferror(in))
        ERROR_EXIT("fgets: %s\n", strerror(errno));

    fclose(in);
    fclose(out);
    return 0;
}

