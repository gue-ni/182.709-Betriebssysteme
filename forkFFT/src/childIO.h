#include "forkFFT.h"
#ifndef childIO_H__
#define childIO_H__
void read_child(int fd, float complex *R, int n);
void parse_mult_complex_2(char *buf, float complex *result, int n);
void close_all(int *fd);
void create_child(int *P, int *R, int *P1, int *R1);
#endif