set term postscript eps color enh
set output 'sprint-failure-delay.eps'

unset grid
set style line 1 lt 3 lw 2 pt 1 ps 1.5 lc rgb "red"
set style line 2 lt 1 lw 2 pt 4 ps 1.5 lc rgb "blue"

set key inside right bottom vertical enhanced nobox
set key noinvert samplen 6 spacing 2.2 width 3 height 0 font "Helvetica, 16"
set ylabel 'Percent of Flows (%)' font "Helvetica, 20"
set yrange[0:105]

set multiplot
set size 1,0.5

set origin 0,0.5
set xlabel 'Search Time (ms) with 1% Link Failure Probability' font "Helvetica, 20"
plot [1:6000]\
'sprint-failure-ndn-delay-0.01.dat' using ($1):($0 / $2 * 100) title '' with line ls 1,\
'sprint-failure-ndn-delay-0.01.point' using ($1):($2 * 100) title 'NDN' with linespoints ls 1,\
'sprint-failure-splicing-delay-0.01.dat' using ($1):($0 / $2 * 100) title '' with line ls 2,\
'sprint-failure-splicing-delay-0.01.point' using ($1):($2 * 100) title 'Path Splicing' with linespoints ls 2

set origin 0,0
set xlabel 'Search Time (ms) with 10% Link Failure Probability' font "Helvetica, 20"
plot [1:6000]\
'sprint-failure-ndn-delay-0.1.dat' using ($1):($0 / $2 * 100) title '' with line ls 1,\
'sprint-failure-ndn-delay-0.1.point' using ($1):($2 * 100) title 'NDN' with linespoints ls 1,\
'sprint-failure-splicing-delay-0.1.dat' using ($1):($0 / $2 * 100) title '' with line ls 2,\
'sprint-failure-splicing-delay-0.1.point' using ($1):($2 * 100) title 'Path Splicing' with linespoints ls 2

unset multiplot
