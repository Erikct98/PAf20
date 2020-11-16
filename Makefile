CC     = bspcc
CFLAGS = -std=c99 -Wall -O3
LFLAGS = -lm

BASEFILES      = chess.h
OBJEDUPACK     = edupack/bspedupack.o
OBJRECURSIVE   = recursive.o
OBJPARBASIC    = BasicParallel.o

all: recursive BasicParallel

recursive.o: $(BASEFILES)
BasicParallel.o: $(BASEFILES)

recursive: $(OBJRECURSIVE) $(OBJEDUPACK)
	$(CC) $(CFLAGS) -fsanitize=leak -o recursive $(OBJRECURSIVE) $(OBJEDUPACK) $(LFLAGS)

BasicParallel: $(OBJPARBASIC) $(OBJEDUPACK)
	$(CC) $(CFLAGS) -fsanitize=leak -o BasicParallel $(OBJPARBASIC) $(OBJEDUPACK) $(LFLAGS)

.PHONY: clean

clean:
	rm -f $(OBJEDUPACK) $(OBJRECURSIVE) $(OBJPARBASIC) recursive BasicParallel

