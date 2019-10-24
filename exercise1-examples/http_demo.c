/**
 * @author Michael Platzer <michael.platzer@tuwien.ac.at>
 * @date 2019-03-21
 *
 * @brief Demo program for sending a simple HTTP request
 */

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

#define ERROR_EXIT(...) { fprintf(stderr, "ERROR: " __VA_ARGS__); exit(EXIT_FAILURE); }

int main()
{
    struct addrinfo hints, *ai;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // create Internet Protocol (IP) socket
    hints.ai_socktype = SOCK_STREAM; // use TCP as transport protocol

    int res = getaddrinfo("neverssl.com", "http", &hints, &ai);
    if (res != 0)
        ERROR_EXIT("getaddrinfo: %s\n", gai_strerror(res));

    int sockfd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
    if (sockfd < 0)
        ERROR_EXIT("socket: %s\n", strerror(errno));

    if (connect(sockfd, ai->ai_addr, ai->ai_addrlen) < 0)
        ERROR_EXIT("connect: %s\n", strerror(errno));

    FILE *sockfile = fdopen(sockfd, "r+");
    if (sockfile == NULL)
        ERROR_EXIT("fdopen: %s\n", strerror(errno));

    // send a GET request to the server:
    if (fputs("GET / HTTP/1.1\r\nHost: neverssl.com\r\n\r\n", sockfile) == EOF)
        ERROR_EXIT("fputs: %s\n", strerror(errno));
    if (fflush(sockfile) == EOF)
        ERROR_EXIT("fflush: %s\n", strerror(errno));

    char buf[1024];

    // print the reply:
    while (fgets(buf, sizeof(buf), sockfile) != NULL)
        fputs(buf, stdout);

    fclose(sockfile);
    return 0;
}
