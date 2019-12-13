#include "forkFFT.h"
#ifndef childIO_H__
#define childIO_H__
void read_child(int fd, complex *R, int n);
void complex_mult(complex *result, complex x, complex y);
void debug(char *msg);
void check_error(int v);
void print_complex(complex c);
void print_complex_err(complex c);
void parse_complex(char *buf, complex *result);
void parse_mult_complex(char *buf, complex *result, int n);
void close_all(int *fd);
void exit_error(char *msg);
void create_child(int *P, int *R, int *P1, int *R1);
void create_complex(complex *c, float a, float b);
#endif