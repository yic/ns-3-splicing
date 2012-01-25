set term postscript eps color enh
set output 'sprint-failure-reliability.eps'

unset grid
set style line 1 lt 6 lw 3 pt 1 ps 2 lc rgb "blue"
set style line 2 lt 2 lw 3 pt 4 ps 2 lc rgb "red"
set style line 3 lt 1 lw 3 pt 6 ps 2 lc rgb "black"
set style line 4 lt 4 lw 3 pt 2 ps 2 lc rgb "blue'
set style line 5 lt 5 lw 3 pt 8 ps 2 lc rgb "red"

set key inside left top vertical enhanced nobox
set key noinvert samplen 6 spacing 3 width 3 height 0 font "Helvetica, 20"
set xlabel 'Probability of Link Failure (%)' font "Helvetica, 25"
set ylabel 'Fraction of Disconnected Pairs (%)' font "Helvetica, 25"
set yrange[0:35]

plot [0:10]\
'sprint-failure-reliability.dat' using ($1 * 100):($2 * 100) title '' with line ls 1,\
'sprint-failure-reliability-point.dat' using ($1 * 100):($2 * 100) title 'IP' with linespoints ls 1,\
'sprint-failure-reliability.dat' using ($1 * 100):($3 * 100) title '' with line ls 2,\
'sprint-failure-reliability-point.dat' using ($1 * 100):($3 * 100) title 'Path Splicing (s=5, r=5)' with linespoints ls 2,\
'sprint-failure-reliability.dat' using ($1 * 100):($4 * 100) title '' with line ls 3,\
'sprint-failure-reliability-point.dat' using ($1 * 100):($4 * 100) title 'Path Splicing (s=5, r=20)' with linespoints ls 3,\
'sprint-failure-reliability.dat' using ($1 * 100):($6 * 100) title '' with line ls 4,\
'sprint-failure-reliability-point.dat' using ($1 * 100):($6 * 100) title 'Path Splicing (s=10, r=20)' with linespoints ls 4,\
'sprint-failure-reliability.dat' using ($1 * 100):($7 * 100) title '' with line ls 5,\
'sprint-failure-reliability-point.dat' using ($1 * 100):($7 * 100) title 'NDN' with linespoints ls 5
