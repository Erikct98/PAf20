CC     = bspcc
CFLAGS = -std=c99 -Wall -O3
LFLAGS = -lm

OBJSIEVESEQ   = sequentialSieve.o
OBJSIEVEERIK  = parallelSieveErik.o
OBJSIEVEDAVID = parallelSieveDavid.o
BASEFILES    = sieve.h
OBJEDUPACK    = edupack/bspedupack.o
OBJSIEVEODD   = parallelSieveCantEven.o

all: sieveErik sieveDavid sieveSeq sieveOdd

parallelSieveErik.o: sieve.h
parallelSieveDavid.o: sieve.h
parallelSieveCantEven.o: sieve.h

sieveSeq: $(OBJSIEVESEQ) $(OBJEDUPACK)
	$(CC) $(CFLAGS) -o sieveSeq $(OBJSIEVESEQ) $(OBJEDUPACK) $(LFLAGS)

sieveErik: $(OBJSIEVEERIK) $(OBJEDUPACK)
	$(CC) $(CFLAGS) -o sieveErik $(OBJSIEVEERIK) $(OBJEDUPACK) $(LFLAGS)

sieveDavid: $(OBJSIEVEDAVID) $(OBJEDUPACK)
	$(CC) $(CFLAGS) -o sieveDavid $(OBJSIEVEDAVID) $(OBJEDUPACK) $(LFLAGS)

sieveOdd: $(OBJSIEVEODD) $(OBJEDUPACK)
	$(CC) $(CFLAGS) -o sieveOdd $(OBJSIEVEODD) $(OBJEDUPACK) $(LFLAGS)

.PHONY: clean

clean:
	rm -f $(OBJSIEVEERIK) $(OBJSIEVEDAVID) $(OBJSIEVESEQ) $(OBJEDUPACK) $(OBJSIEVEODD) sieveErik sieveDavid sieveSeq sieveOdd

