# @author: Jakob Maier 11809618
# @programm name: myexpand
#
#

CC = gcc
# DEFS = -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L
DEFS = -D_DEFAULT_SOURCE -D_POSIX_C_SOURCE=200809L
CFLAGS = -Wall -g -std=c99 -pedantic $(DEFS)

OBJECTS = main.o

.PHONY: all clean
all: myexpand 

myexpand: $(OBJECTS) 
	$(CC) $(LDFLAGS) -o $@ $^
	rm -f *.o 

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

main.o: main.c

clean:
	rm -f *.o myexpand


