# @file Makefile
# @author Jakob G. Maier <e11809618@student.tuwien.ac.at>
# Makefile for generator & supervisor
CC 	   = gcc
CFLAGS = -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L -g
LFLAGS = -lrt -pthread

PROGS  = supervisor generator 
DEPS   = common

.PHONY: all clean 

all: $(PROGS)
	rm -f exercise_3.tgz
	tar -cvzf exercise_3.tgz supervisor.c generator.c common.c common.h Makefile

$(PROGS): %: %.o $(DEPS:=.o)
	$(CC) -o $@ $^ $(LFLAGS)

%.o: %.c $(DEPS:=.h)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf *.o *.tgz $(PROGS)