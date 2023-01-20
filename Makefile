INCLUDE = common.c kernels.c
CFLAGS=-g3

OFLAGS=-march=native -O1

MATHS =  -llapack -lblas -lm

all: sobel base

sobel: sobel.c
ifeq ($(CC),gcc)
	$(CC) -DBASELINE=1 $(CFLAGS) $(OFLAGS) $< $(INCLUDE) -o $@optbase $(MATHS)
	$(CC) -DBASELINE=1 $(CFLAGS) $(OFLAGS) -funroll-loops $< $(INCLUDE) -o $@optunroll $(MATHS)
	$(CC) -DBASELINE=1 $(CFLAGS) -march=native -O2 $< $(INCLUDE) -o $@opt2 $(MATHS)
	$(CC) -DBASELINE=1 $(CFLAGS) -march=native -O3 $< $(INCLUDE) -o $@opt3 $(MATHS)
	$(CC) -DBASELINE=1 $(CFLAGS) -march=native -Ofast $< $(INCLUDE) -o $@optfast $(MATHS)
else
ifeq ($(CC),clang)
	$(CC) -DBASELINE=1 $(CFLAGS) $(OFLAGS) $< $(INCLUDE)  -o $@optbase $(MATHS)
	$(CC) -DBASELINE=1 $(CFLAGS) $(OFLAGS) -funroll-loops $< $(INCLUDE)  -o $@optunroll $(MATHS)
	$(CC) -DBASELINE=1 $(CFLAGS) -march=native -O2 $< $(INCLUDE)  -o $@opt2 $(MATHS)
	$(CC) -DBASELINE=1 $(CFLAGS) -march=native -O3 $< $(INCLUDE)  -o $@opt3 $(MATHS)
	$(CC) -DBASELINE=1 $(CFLAGS) -march=native -Ofast $< $(INCLUDE)  -o $@optfast $(MATHS)
else
	@echo "ERROR: no compiler specified using CC. Possible values for CC: gcc, clang"
endif
endif

base: sobelbase.c
ifeq ($(CC),gcc)
	$(CC) -DBASELINE=1 $(CFLAGS) $(OFLAGS) $< common.c -o $@ $(MATHS)
	$(CC) -DBASELINE=1 $(CFLAGS) $(OFLAGS) -funroll-loops $< common.c -o $@unroll $(MATHS)
	$(CC) -DBASELINE=1 $(CFLAGS) -march=native -O2 $< common.c -o $@2 $(MATHS)
	$(CC) -DBASELINE=1 $(CFLAGS) -march=native -O3 $< common.c -o $@3 $(MATHS)
	$(CC) -DBASELINE=1 $(CFLAGS) -march=native -Ofast $< common.c -o $@fast $(MATHS)
else
ifeq ($(CC),clang)
	$(CC) -DBASELINE=1 $(CFLAGS) $(OFLAGS) $< common.c -o $@ $(MATHS)
	$(CC) -DBASELINE=1 $(CFLAGS) $(OFLAGS) -funroll-loops $< common.c -o $@unroll $(MATHS)
	$(CC) -DBASELINE=1 $(CFLAGS) -march=native -O2 $< common.c -o $@2 $(MATHS)
	$(CC) -DBASELINE=1 $(CFLAGS) -march=native -O3 $< common.c -o $@3 $(MATHS)
	$(CC) -DBASELINE=1 $(CFLAGS) -march=native -Ofast $< common.c -o $@fast $(MATHS)
else
	@echo "ERROR: no compiler specified using CC. Possible values for CC: gcc, clang"
endif
endif

clean:
	rm -Rf *~ sobelopt* base* sobelbase
	rm -f in/input.raw out/*.raw