#!/bin/bash
make CC=gcc
./cvt_vid.sh v2r in/input.mp4 in/input.raw

rm -f dat/basegcc.dat
for base in $(ls base*); do
./$base in/input.raw out/output.raw $base >> dat/basegcc.dat
done

./sobel in/input.raw out/output.raw > dat/gcc.dat

./cvt_vid.sh r2v out/output.raw out/output.mp4
make clean

make CC=clang
./cvt_vid.sh v2r in/input.mp4 in/input.raw

rm -f dat/baseclang.dat
for base in $(ls base*); do
./$base in/input.raw out/output.raw $base >> dat/baseclang.dat
done

./sobel in/input.raw out/output.raw > dat/clang.dat

./cvt_vid.sh r2v out/output.raw out/output.mp4

make clean

for plot in $(ls plot/*.gp); do
gnuplot $plot
done

# mplayer out/output.mp4