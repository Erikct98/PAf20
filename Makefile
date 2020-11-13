CC     = bspcc
CFLAGS = -std=c99 -Wall -O3
LFLAGS = -lm

BASEFILES      = sieve.h
OBJEDUPACK     = edupack/bspedupack.o
OBJSIEVESEQ    = sequentialSieve.o
OBJSIEVEPAR    = parallelSieve.o
OBJSIEVEPARGAP = parallelSieveGap.o
OBJSIEVEPARODD = parallelSieveOdd.o
OBJSIEVEPARODDG = parallelSieveGapAndOdd.o
OBJSIEVEPARODDGC = parallelSieveOddGapIdentical.o
OBJSIEVEPARDAVID = parallelSieveDavid.o

all: sieveSeq sievePar sieveParGap sieveParOdd sieveParDavid sieveParOddG sieveParOddGC

sequentialSieve.o: sieve.h
parallelSievePar.o: sieve.h
parallelSieveParGap.o: sieve.h
parallelSieveParOdd.o: sieve.h
parallelSieveGapAndOdd.o: sieve.h
parallelSieveOddGapIdentical.o: sieve.h
parallelSieveDavid.o: sieve.h

sieveSeq: $(OBJSIEVESEQ) $(OBJEDUPACK)
	$(CC) $(CFLAGS) -fsanitize=leak -o sieveSeq $(OBJSIEVESEQ) $(OBJEDUPACK) $(LFLAGS)

sievePar: $(OBJSIEVEPAR) $(OBJEDUPACK)
	$(CC) $(CFLAGS) -o sievePar $(OBJSIEVEPAR) $(OBJEDUPACK) $(LFLAGS)

sieveParGap: $(OBJSIEVEPARGAP) $(OBJEDUPACK)
	$(CC) $(CFLAGS) -o sieveParGap $(OBJSIEVEPARGAP) $(OBJEDUPACK) $(LFLAGS)

sieveParOdd: $(OBJSIEVEPARODD) $(OBJEDUPACK)
	$(CC) $(CFLAGS) -o sieveParOdd $(OBJSIEVEPARODD) $(OBJEDUPACK) $(LFLAGS)

sieveParOddG: $(OBJSIEVEPARODDG) $(OBJEDUPACK)
	$(CC) $(CFLAGS) -o sieveParOddG $(OBJSIEVEPARODDG) $(OBJEDUPACK) $(LFLAGS)

sieveParOddGC: $(OBJSIEVEPARODDGC) $(OBJEDUPACK)
	$(CC) $(CFLAGS) -o sieveParOddGC $(OBJSIEVEPARODDGC) $(OBJEDUPACK) $(LFLAGS)

sieveParDavid: $(OBJSIEVEPARDAVID) $(OBJEDUPACK)
	$(CC) $(CFLAGS) -o sieveParDavid $(OBJSIEVEPARDAVID) $(OBJEDUPACK) $(LFLAGS)

.PHONY: clean

clean:
	rm -f $(OBJSIEVESEQ) $(OBJSIEVEPAR) $(OBJSIEVEPARGAP) $(OBJSIEVEPARODD) $(OBJSIEVEPARODDG) $(OBJSIEVEPARODDGC) $(OBJSIEVEDAVID) $(OBJEDUPACK) sieveSeq sievePar sieveParGap sieveParOdd sieveParDavid sieveParOddG sieveParOddGC

