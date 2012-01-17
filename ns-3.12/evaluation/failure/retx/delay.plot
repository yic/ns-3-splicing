set term postscript eps color enh

unset grid
set style line 1 lt 6 lw 2 pt 1 ps 1 lc -1
set style line 2 lt 2 lw 2 pt 2 ps 1 lc 1
set style line 3 lt 3 lw 2 pt 3 ps 1 lc 3
set style line 4 lt 4 lw 2 pt 4 ps 1 lc 4
set style line 5 lt 5 lw 2 pt 5 ps 1 lc 5
set style line 6 lt 6 lw 2 pt 6 ps 1 lc 7

set size 0.6, 0.6
set out 'delay.eps'

set origin 0,0
set key bottom right nobox
set xlabel 'Delay (ms)' font "Helvetica, 20"
set ylabel 'Percent of Points (%)' font "Helvetica, 20"
set yrange[0:100]

plot [0:6000]\
'delay-10-20-0.01' using ($1):($0 / $2 * 100) title 'slice=10, delay=20, prob=0.01' with line ls 1,\
'delay-10-20-0.05' using ($1):($0 / $2 * 100) title 'slice=10, delay=20, prob=0.05' with line ls 3,\
'delay-10-20-0.1' using ($1):($0 / $2 * 100) title 'slice=10, delay=20, prob=0.1' with line ls 5
#'delay-5-20-0.01' using ($1):($0 / $2 * 100) title 'slice=5, delay=20, prob=0.01' with line ls 2,\
#'delay-5-20-0.05' using ($1):($0 / $2 * 100) title 'slice=5, delay=20, prob=0.05' with line ls 4,\
#'delay-5-20-0.1' using ($1):($0 / $2 * 100) title 'slice=5, delay=20, prob=0.1' with line ls 6
