#include "forkFFT2.h"
#ifndef childIO_H__
#define childIO_H__
void read_child(int fd, float complex *R, int n);
//void complex_mult(complex *result, complex x, complex y);
//void print_complex(complex c);
//void parse_complex(char *buf, complex *result);
void parse_mult_complex_2(char *buf, float complex *result, int n);
//void parse_mult_complex(char *buf, complex *result, int n);
void close_all(int *fd);
void create_child(int *P, int *R, int *P1, int *R1);
//void create_complex(complex *c, float a, float b);
#endif