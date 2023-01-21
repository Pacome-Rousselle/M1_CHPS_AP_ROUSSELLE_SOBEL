#!/bin/bash
make CC=gcc
./cvt_vid.sh v2r in/input.mp4 in/input.raw

sudo cpupower -c 1 frequency-set -g performance
./gccsobelbase in/input.raw out/output.raw BASE > dat/gccsobelbase.dat

for sobel in $(ls sobelopt*); do
sudo cpupower -c 1 frequency-set -g performance
./$sobel in/input.raw out/output.raw $sobel > dat/gcc$sobel.dat
done

./cvt_vid.sh r2v out/output.raw out/output.mp4

make clean
###########
make CC=clang
./cvt_vid.sh v2r in/input.mp4 in/input.raw

sudo cpupower -c 1 frequency-set -g performance
./clangsobelbase in/input.raw out/output.raw BASE > dat/clangsobelbase.dat

for sobel in $(ls sobelopt*); do
sudo cpupower -c 1 frequency-set -g performance
./$sobel in/input.raw out/output.raw $sobel> dat/clang$sobel.dat
done

./cvt_vid.sh r2v out/output.raw out/output.mp4

make clean

for plot in $(ls plot/*.gp); do
gnuplot $plot
done

# mplayer out/output.mp4