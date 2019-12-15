/**
 * @file forkFFT.h
 * @author Jakob G. Maier <e11809618@student.tuwien.ac.at>
 * @date 2019-12-11 
 * @brief header file for forkFFT.c 
 */ 
#ifndef forkFFT_H__
#define forkFFT_H__

#define PI 3.141592654
#define OUTPUT 	0
#define INPUT 	1  
#define BUFSIZE (256)
#define EVEN 	0
#define ODD 	1

void exit_error(char *msg);
void check_error(int v);
#endif /* forkFFT_H__ */