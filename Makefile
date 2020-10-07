CC     = bspcc
CFLAGS = -std=c99 -Wall -O3
LFLAGS = -lm

BASEFILES      = sieve.h
OBJEDUPACK     = edupack/bspedupack.o
OBJSIEVESEQ    = sequentialSieve.o
OBJSIEVEPAR    = parallelSieve.o
OBJSIEVEPARGAP = parallelSieveGap.o
OBJSIEVEPARODD = parallelSieveOdd.o
OBJSIEVEPARDAVID = parallelSieveDavid.o

all: sieveSeq sievePar sieveParGap sieveParOdd sieveParDavid

sequentialSieve.o: sieve.h
parallelSievePar.o: sieve.h
parallelSieveParGap.o: sieve.h
parallelSieveParOdd.o: sieve.h
parallelSieveDavid.o: sieve.h

sieveSeq: $(OBJSIEVESEQ) $(OBJEDUPACK)
	$(CC) $(CFLAGS) -o sieveSeq $(OBJSIEVESEQ) $(OBJEDUPACK) $(LFLAGS)

sievePar: $(OBJSIEVEPAR) $(OBJEDUPACK)
	$(CC) $(CFLAGS) -o sievePar $(OBJSIEVEPAR) $(OBJEDUPACK) $(LFLAGS)

sieveParGap: $(OBJSIEVEPARGAP) $(OBJEDUPACK)
	$(CC) $(CFLAGS) -o sieveParGap $(OBJSIEVEPARGAP) $(OBJEDUPACK) $(LFLAGS)

sieveParOdd: $(OBJSIEVEPARODD) $(OBJEDUPACK)
	$(CC) $(CFLAGS) -o sieveParOdd $(OBJSIEVEPARODD) $(OBJEDUPACK) $(LFLAGS)

sieveParDavid: $(OBJSIEVEPARDAVID) $(OBJEDUPACK)
	$(CC) $(CFLAGS) -o sieveParDavid $(OBJSIEVEPARDAVID) $(OBJEDUPACK) $(LFLAGS)

.PHONY: clean

clean:
	rm -f $(OBJSIEVESEQ) $(OBJSIEVEPAR) $(OBJSIEVEPARGAP) $(OBJSIEVEPARODD) $(OBJSIEVEDAVID) $(OBJEDUPACK) sieveSeq sievePar sieveParGap sieveParOdd sieveParDavid

