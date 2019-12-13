CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L -g
LFLAGS = -lm

PROGS = forkFFT2 test #forkFFT
DEPS = childIO 

.PHONY: all clean

all: $(PROGS)

$(PROGS): % : %.o $(DEPS:=.o)
	$(CC) -o $@ $^ $(LFLAGS)

%.o: %.c $(DEPS:=.h)
	$(CC) $(CFLAGS) -c -o $@ $<

forkFFT.o: forkFFT.c forkFFT.h
forkFFT2.o: forkFFT2.c forkFFT2.h
childIO.o: childIO.c forkFFT2.h
test.o: test.c

clean:
	rm -rf *.o $(PROGS)
