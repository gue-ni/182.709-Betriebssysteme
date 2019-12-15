CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L -g
LFLAGS = -lm

PROGS = forkFFT
DEPS = childIO 

.PHONY: all clean install

all: $(PROGS)

install: all
	rm -f *.o

$(PROGS): % : %.o $(DEPS:=.o)
	$(CC) -o $@ $^ $(LFLAGS)

%.o: %.c $(DEPS:=.h)
	$(CC) $(CFLAGS) -c -o $@ $<

forkFFT.o: forkFFT.c forkFFT.h
childIO.o: childIO.c forkFFT.h

clean:
	rm -rf *.o $(PROGS)
