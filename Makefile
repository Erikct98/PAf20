CC= bspcc
CFLAGS= -std=c99 -Wall -O3
LFLAGS= -lm

OBJSIEVE= parallelSieveErik.o
OBJEDUPACK = edupack/bspedupack.o


all: sieve

sieve: $(OBJSIEVE) $(OBJEDUPACK)
	$(CC) $(CFLAGS) -o sieve $(OBJSIEVE) $(OBJEDUPACK) $(LFLAGS)


.PHONY: clean

clean:
	rm -f $(OBJSIEVE) sieve

