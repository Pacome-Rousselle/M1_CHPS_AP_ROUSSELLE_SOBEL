#!/bin/bash
make
./cvt_vid.sh v2r in/input.mp4 in/input.raw

./base in/input.raw out/output.raw
./sobel in/input.raw out/output.raw

for file in $(basename -s .raw out/*.raw); do
./cvt_vid.sh r2v out/$file.raw out/$file.mp4
done

for plot in $(ls plot/*.gp); do
gnuplot $plot
done

mplayer out/output.mp4

make clean