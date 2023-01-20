set terminal png enhanced font "arial,15" fontscale 1.0 size 2000, 1000 
set output "graphs/opticlang.png"
set title "Comparaison de la bande passante du kernel sobel avec clang"
set key autotitle columnhead
set datafile separator ";"
set grid
set auto x

set ylabel "MiB/s" textcolor "black"

set style data histograms
set style histogram errorbars gap 0 lw 2
set style fill solid border -1
set boxwidth 0.5 absolute

set yrange [0:2200]
set xtic rotate by -45 scale 0 

plot "dat/clangsobeloptbase.dat" u  6:7:xtic(1) t "-01" lc "red",\
     "dat/clangsobelopt2.dat" u  6:7:xtic(1) t "-02" lc "green",\
     "dat/clangsobelopt3.dat" u  6:7:xtic(1) t "-03" lc "blue",\
     "dat/clangsobeloptfast.dat" u  6:7:xtic(1) t "-Ofast" lc "yellow",\
     "dat/clangsobeloptunroll.dat" u  6:7:xtic(1) t "-funroll-loops" lc "purple"