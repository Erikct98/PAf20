CC     = bspcc
CFLAGS = -std=c99 -Wall -O3
LFLAGS = -lm

BASEFILES      = chess.h
OBJEDUPACK     = edupack/bspedupack.o
OBJRECURSIVE    = recursive.o

all: recursive

recursive.o: $(BASEFILES)

recursive: $(OBJRECURSIVE) $(OBJEDUPACK)
	$(CC) $(CFLAGS) -fsanitize=leak -o recursive $(OBJRECURSIVE) $(OBJEDUPACK) $(LFLAGS)

.PHONY: clean

clean:
	rm -f $(OBJEDUPACK) $(OBJRECURSIVE) recursive

