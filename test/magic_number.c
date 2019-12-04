#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h> // memcmp
#include <arpa/inet.h> // ntohl

#define ERROR_EXIT(...) { fprintf(stderr, "ERROR: " __VA_ARGS__); exit(EXIT_FAILURE); }

typedef struct {
    uint8_t header[6];
    uint16_t log_screen_width;
    uint16_t log_screen_height;
    uint8_t logical_screen_descriptor;
    uint8_t background_color_index;
    uint8_t pixel_aspect_ratio;
} gif_header;

int main()
{
    gif_header buf;
    FILE *in;

    in = fopen("file.gif", "r");

    if (fread(&buf, 1, sizeof(buf), in) < sizeof(buf))
        {
        ERROR_EXIT("fread: %s\n", strerror(errno));
    }

    fwrite(&buf, 1, sizeof(buf), stdout);
    fclose(in);

    return 0;
}
