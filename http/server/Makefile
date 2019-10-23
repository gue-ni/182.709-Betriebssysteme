# @author: Jakob Maier 11809618
# @programm name: server
#
#

CC = gcc
# DEFS = -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L
DEFS = -D_DEFAULT_SOURCE -D_POSIX_C_SOURCE=200809L
CFLAGS = -Wall -g -std=c99 -pedantic $(DEFS)

OBJECTS = main.o

.PHONY: all clean
all: server 

server: $(OBJECTS) 
	$(CC) $(LDFLAGS) -o $@ $^ 

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

main.o: main.c

clean:
	rm -rf *.o server


