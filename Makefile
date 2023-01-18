CC=gcc

CFLAGS=-g3

OFLAGS=-march=native -O1

all: sobel base

sobel: sobel.c
	$(CC) -DBASELINE=1 $(CFLAGS) $(OFLAGS) $< common.c -o $@ -lm

base: base.c
	$(CC) -DBASELINE=1 $(CFLAGS) $(OFLAGS) $< common.c -o $@ -lm

clean:
	rm -Rf *~ sobel base
	rm -f in/input.raw out/output.raw

#check OBHPC project for correct syntax on compilers