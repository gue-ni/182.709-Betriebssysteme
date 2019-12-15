CC = gcc
CFLAGS = -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L -g
LFLAGS = -lm -lrt -pthread

PROGS = supervisor generator  
DEPS = util 

.PHONY: all clean

all: $(PROGS)

$(PROGS): % : %.o $(DEPS:=.o)
	$(CC) -o $@ $^ $(LFLAGS)

%.o: %.c $(DEPS:=.h)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf *.o $(PROGS)