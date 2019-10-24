/**
 * @author Michael Platzer <michael.platzer@tuwien.ac.at>
 * @date 2019-03-21
 *
 * @brief Demo program creating a Netpbm Portable PixMap (PPM) image
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#define ERROR_EXIT(...) { fprintf(stderr, "ERROR: " __VA_ARGS__); exit(EXIT_FAILURE); }

int main()
{
    FILE *out;
    const int width = 256, height = 256;

    if ((out = fopen("output.ppm", "w")) == NULL)
        ERROR_EXIT("fopen: %s\n", strerror(errno));

    // write file header (using fprintf since this is text):
    if (fprintf(out, "P6\n%d %d\n%d\n", width, height, 0xff) < 0)
        ERROR_EXIT("fprintf: %s\n", strerror(errno));

    // writing image data (using fwrite since this is binary data):
    int x, y;
    for (y = 0; y < height; y++)
        for (x = 0; x < width; x++) {
            // set RGB (red, green and blue) values for each pixel:
            uint8_t rgb[3] = { x & 0xff, y & 0xff, 0xff };
            if (fwrite(rgb, 1, sizeof(rgb), out) != sizeof(rgb))
                ERROR_EXIT("fwrite: %s\n", strerror(errno));
        }

    fclose(out);
    return 0;
}
