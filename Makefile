CC     = bspcc
CFLAGS = -std=c99 -Wall -O3
LFLAGS = -lm

OBJSIEVESEQ   = sequentialSieve.o
OBJSIEVEERIK  = parallelSieveErik.o
OBJSIEVEDAVID = parallelSieveDavid.o
OBJEDUPACK    = edupack/bspedupack.o


all: sieveErik sieveDavid sieveSeq

sieveSeq: $(OBJSIEVESEQ) $(OBJEDUPACK)
	$(CC) $(CFLAGS) -o sieveSeq $(OBJSIEVESEQ) $(OBJEDUPACK) $(LFLAGS)

sieveErik: $(OBJSIEVEERIK) $(OBJEDUPACK)
	$(CC) $(CFLAGS) -o sieveErik $(OBJSIEVEERIK) $(OBJEDUPACK) $(LFLAGS)

sieveDavid: $(OBJSIEVEDAVID) $(OBJEDUPACK)
	$(CC) $(CFLAGS) -o sieveDavid $(OBJSIEVEDAVID) $(OBJEDUPACK) $(LFLAGS)


.PHONY: clean

clean:
	rm -f $(OBJSIEVEERIK) $(OBJSIEVEDAVID) $(OBJSIEVESEQ) $(OBJEDUPACK) sieveErik sieveDavid sieveSeq

