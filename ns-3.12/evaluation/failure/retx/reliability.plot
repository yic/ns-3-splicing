set term postscript eps color enh

unset grid
set style line 1 lt 6 lw 2 pt 1 ps 1 lc -1
set style line 2 lt 2 lw 2 pt 2 ps 1 lc 1
set style line 3 lt 3 lw 2 pt 3 ps 1 lc 3
set style line 4 lt 4 lw 2 pt 4 ps 1 lc 4
set style line 5 lt 5 lw 2 pt 5 ps 1 lc 5
set style line 6 lt 6 lw 2 pt 6 ps 1 lc 7

set size 0.6, 0.6
set out 'reliability.eps'

set origin 0,0
set key top left nobox
set xlabel 'Probability of Link Failure (%)' font "Helvetica, 15"
set ylabel 'Fraction of Disconnected Pairs (%)' font "Helvetica, 15"
set yrange[0:35]

plot [0:10]\
'reliability' using ($1 * 100):($2 * 100) title 'slice=1' with line ls 1,\
'reliability' using ($1 * 100):($3 * 100) title 'slice=5, reliability=5' with line ls 3,\
'reliability' using ($1 * 100):($4 * 100) title 'slice=5, reliability=20' with line ls 5,\
'reliability' using ($1 * 100):($5 * 100) title 'slice=10, reliability=5' with line ls 2,\
'reliability' using ($1 * 100):($6 * 100) title 'slice=10, reliability=20' with line ls 4
