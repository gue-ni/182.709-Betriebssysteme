CC = gcc
# DEFS = -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L
DEFS = -D_DEFAULT_SOURCE -D_POSIX_C_SOURCE=200809L
CFLAGS = -Wall -g -std=c99 -pedantic $(DEFS)

PROGS = client server

.PHONY: all clean
all: $(PROGS)

$(PROGS): % : %.o
	$(CC) $(LDFLAGS) -o $@ $^
	rm -f *.o

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o $(PROGS)
