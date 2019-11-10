CC = gcc
#DEFS = -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L
DEFS = -D_DEFAULT_SOURCE -D_POSIX_C_SOURCE=200809L
CFLAGS = -Wall -g -std=c99 -pedantic $(DEFS)

OBJECTS = main.o hello.o

.PHONY: all clean
all: hello

hello: $(OBJECTS) 
	$(CC) $(LDFLAGS) -o $@ $^ 

%.o: %.c hello.h
	$(CC) $(CFLAGS) -c -o $@ $<

#main.o: main.c hello.h
#hello.o: hello.c hello.h

clean:
	rm -rf *.o hello


