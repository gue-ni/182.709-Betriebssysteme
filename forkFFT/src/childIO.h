#include "forkFFT.h"
#ifndef childIO_H__
#define childIO_H__
void read_child(int fd, float complex *R, int n);
void close_both_ends(int *fd);
void create_child(int *P, int *R);
#endif