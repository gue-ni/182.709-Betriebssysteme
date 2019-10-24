/**
 * @author Michael Platzer <michael.platzer@tuwien.ac.at>
 * @date 2019-03-21
 *
 * @brief Demo program displaying the size of a PNG image
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h> // memcmp
#include <arpa/inet.h> // ntohl

#define ERROR_EXIT(...) { fprintf(stderr, "ERROR: " __VA_ARGS__); exit(EXIT_FAILURE); }

/**
 * @brief Memory representation of the PNG file header and 'IHDR' chunk
 *
 * For details about the PNG file format please visit:
 * https://en.wikipedia.org/wiki/Portable_Network_Graphics#File_format
 */
struct png_header {
    uint8_t file_header[8]; // 8 byte PNG file header

    // fields for the first chunk of the file (which must be the 'IHDR' chunk):
    uint32_t img_hdr_len,   // length of the chunk (13 data bytes for 'IHDR')
             img_hdr_type,  // chunk type (e.g. the string 'IHDR')
             width, height; // chunk data: starts with image width and height
    uint8_t depth, color, compression, filter, interlaced; // further data
    uint32_t img_hdr_crc;   // CRC checksum of the 'IHDR' chunk
};

int main()
{
    struct png_header buf;
    FILE *in;

    if ((in = fopen("cat.png", "r")) == NULL)
        ERROR_EXIT("fopen: %s\n", strerror(errno));

    if (fread(&buf, 1, sizeof(buf), in) < sizeof(buf))
        ERROR_EXIT("fread: %s\n", strerror(errno));

    // check 8 byte file header:
    if (memcmp(buf.file_header, "\x89PNG\r\n\x1A\n", 8) != 0)
        ERROR_EXIT("this is not a PNG file\n");

    printf("This PNG image has a size of %dx%d pixels.\n",
           ntohl(buf.width), ntohl(buf.height));
    fclose(in);
    return 0;
}
