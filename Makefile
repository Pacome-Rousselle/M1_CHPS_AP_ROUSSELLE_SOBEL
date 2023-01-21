INCLUDE = common.c kernels.c
CFLAGS=-g3

OFLAGS=-march=native -O1

MATHS =  -llapack -lblas -lm

all: sobel base

sobel: sobel.c
ifeq ($(CC),gcc)
	$(CC) -DBASELINE=1 $(CFLAGS) $(OFLAGS) -funroll-loops $< $(INCLUDE) -o $@optunroll $(MATHS)
	$(CC) -DBASELINE=1 $(CFLAGS) -march=native -O3 $< $(INCLUDE) -o $@opt3 $(MATHS)
else
ifeq ($(CC),clang)
	$(CC) -DBASELINE=1 $(CFLAGS) $(OFLAGS) -funroll-loops $< $(INCLUDE)  -o $@optunroll $(MATHS)
	$(CC) -DBASELINE=1 $(CFLAGS) -march=native -O3 $< $(INCLUDE)  -o $@opt3 $(MATHS)
else
	@echo "ERROR: no compiler specified using CC. Possible values for CC: gcc, clang"
endif
endif

base: sobelbase.c
ifeq ($(CC),gcc)
	$(CC) -DBASELINE=1 $(CFLAGS) $(OFLAGS) $< common.c -o gccsobel$@ $(MATHS)
else
ifeq ($(CC),clang)
	$(CC) -DBASELINE=1 $(CFLAGS) $(OFLAGS) $< common.c -o clangsobel$@ $(MATHS)
else
	@echo "ERROR: no compiler specified using CC. Possible values for CC: gcc, clang"
endif
endif

clean:
	rm -Rf *~ sobelopt* *sobelbase
	rm -f in/input.raw out/*.raw