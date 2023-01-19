set terminal png enhanced font "arial,15" fontscale 1.0 size 2000, 1000 
set output "graphs/opti.png"
set title "title"

set datafile separator ";"
set grid
set auto x

set ylabel "MiB/s" textcolor "black"

set style data histograms
set style histogram errorbars gap 0 lw 2
set style fill solid border -1
set boxwidth 0.5 absolute

set yrange [0:3000]
set xtic rotate by -45 scale 0 

plot "dat/gcc.dat" u  6:7:xtic(1) t "gcc" lc "green",\
     "dat/clang.dat" u  6:7:xtic(1) t "clang" lc "yellow"