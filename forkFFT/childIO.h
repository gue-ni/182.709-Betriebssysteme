/**
 * @file childIO.h
 * @author Jakob G. Maier <e11809618@student.tuwien.ac.at>
 * @date 2019-12-11 
 * @brief header file for childIO.h
 */ 
#include "forkFFT.h"
#ifndef childIO_H__
#define childIO_H__

int read_child(int fd, float complex *R, int n);
void close_both_ends(int *fd);
void create_child(int *P, int *R);
#endif /* childIO_H__ */