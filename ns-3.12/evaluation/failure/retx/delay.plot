set term postscript eps color enh

unset grid

set style line 1 lt 4 lw 1 pt 1 ps 1 lc rgb "black"
set style line 2 lt 5 lw 1 pt 2 ps 1 lc rgb "red"
set style line 3 lt 2 lw 1 pt 4 ps 1 lc rgb "blue"
set style line 4 lt 4 lw 3 pt 1 ps 1 lc rgb "black"
set style line 5 lt 5 lw 3 pt 2 ps 1 lc rgb "red"
set style line 6 lt 2 lw 3 pt 4 ps 1 lc rgb "blue"

set size 0.6, 0.6
set out 'delay.eps'
set multiplot

set origin 0, 0
set key right bottom nobox
set ylabel 'Percent of Points (%)' font "Helvetica, 20"
set xlabel 'Data Receive Time (ms)' font "Helvetica, 20"
set yrange[0:100]

plot [0:6000]\
'delay-10-20-0.01' using ($1):(($0 + 1) * 100 / $2) title 'slice=10, stretch=20, prob=0.01' with line ls 1,\
'delay-10-20-0.05' using ($1):(($0 + 1) * 100 / $2) title 'slice=10, stretch=20, prob=0.05' with line ls 2,\
'delay-10-20-0.1' using ($1):(($0 + 1) * 100 / $2) title 'slice=10, stretch=20, prob=0.1' with line ls 3

set origin 0.15, 0.2
set ylabel ''
set xlabel ''
set size 0.4, 0.35
set yrange[80:100]

plot [0:1000]\
'delay-10-20-0.01' using ($1):(($0 + 1) * 100 / $2) title '' with line ls 1,\
'delay-10-20-0.05' using ($1):(($0 + 1) * 100 / $2) title '' with line ls 2,\
'delay-10-20-0.1' using ($1):(($0 + 1) * 100 / $2) title '' with line ls 3
