CFLAGS = -std=c99 -pedantic -Wall -D_DEFAULT_SOURCE -D_BSD_SOURCE -D_SVID_SOURCE -D_POSIX_C_SOURCE=200809L -g
LFLAGS = -pthread -lm
PROGS = supervisor generator  

all: $(PROGS)

install: all
	rm -f *.o

$(PROGS): % : %.o
	gcc -o $@ $< $(LFLAGS) 

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(PROGS)

