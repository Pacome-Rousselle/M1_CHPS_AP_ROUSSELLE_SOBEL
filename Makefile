CC=gcc

CFLAGS=-g3

OFLAGS=-march=native -O1

all: sobel

sobel: sobel.c
	$(CC) -DBASELINE=1 $(CFLAGS) $(OFLAGS) $< common.c -o $@ -lm

clean:
	rm -Rf *~ sobel
	rm -f in/input.raw out/output.raw

#check OBHPC project for correct syntax on compilers