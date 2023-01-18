set terminal png enhanced font "arial,15" fontscale 1.0 size 1000, 1000 
set output "graphs/plot.png"
set title "title"

set datafile separator ";"
set grid
set auto x

set ylabel "MiB/s" textcolor "black"

set style data histogram
set style fill solid border -1
set boxwidth 0.5 absolute

set yrange [0:500]
set xtic rotate by -45 scale 0 

plot "dat/plot.dat" u  6:xtic(1) t "lorem" lc "red"