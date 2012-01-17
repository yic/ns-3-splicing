set term postscript eps color enh

unset grid
set style line 1 lt 2 lw 1 pt 1 ps 1 lc -1
set style line 2 lt 4 lw 1 pt 2 ps 1 lc 1
set style line 3 lt 5 lw 1 pt 4 ps 1 lc 3
set style line 4 lt 2 lw 3 pt 1 ps 1 lc -1
set style line 5 lt 4 lw 3 pt 2 ps 1 lc 1
set style line 6 lt 5 lw 3 pt 4 ps 1 lc 3

set size 0.6, 0.6
set out 'retx.eps'
set multiplot

set origin 0,0
set key bottom right nobox
set xlabel 'Number of Retx' font "Helvetica, 20"
set ylabel 'Percent of Points (%)' font "Helvetica, 20"
set yrange[0:100]

plot [-1:21]\
'x' using ($1):($3 * 100) title '' with line ls 1
