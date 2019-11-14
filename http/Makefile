#	@author: Jakob G. Maier <e11809618@student.tuwien.ac.at>
#	@date: 2019-19-24
#	@brief: Makefile for http server and client, makes all PROGS with includes DEPS(.h)

CC = gcc
# FÜR DIE ABGABE:
# DEFS = -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L
DEFS = -D_DEFAULT_SOURCE -D_POSIX_C_SOURCE=200809L
CFLAGS = -Wall -g -std=c99 -pedantic $(DEFS)

PROGS = client server
DEPS = utils

.PHONY: all clean

all: $(PROGS)
	rm -rf *.o

$(PROGS): % : %.o $(DEPS:=.o)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c $(DEPS:=.h)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf *.o $(PROGS)



