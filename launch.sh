#!/bin/bash
make
./cvt_vid.sh v2r in/input.mp4 in/input.raw
./sobel in/input.raw out/output.raw
./cvt_vid.sh r2v out/output.raw out/output.mp4
mplayer out/output.mp4 
make clean