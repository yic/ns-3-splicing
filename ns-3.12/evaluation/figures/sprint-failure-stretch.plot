set term postscript eps color enh
set output 'sprint-failure-stretch.eps'

unset grid
set style line 1 lt 2 lw 3 pt 1 ps 2 lc rgb "red"
set style line 2 lt 1 lw 3 pt 4 ps 2 lc rgb "blue"

set key inside right bottom vertical enhanced nobox
set key noinvert samplen 6 spacing 2.5 width 3 height 1.5 font "Helvetica, 18"
set xtics font "Helvetica, 18"
set ytics font "Helvetica, 18"
set ylabel 'Percent of Flows (%)' font "Helvetica, 20"
set yrange[0:105]

set multiplot
set size 1,0.5

set origin 0,0.5
set xlabel 'Path Stretch with 1% Link Failure Probability' font "Helvetica, 21"
plot [1:4]\
'sprint-failure-ndn-stretch-0.01.dat' using ($1):($0 / $2 * 100) title '' with line ls 1,\
'sprint-failure-ndn-stretch-0.01.point' using ($1):($2 * 100) title 'NDN' with linespoints ls 1,\
'sprint-failure-splicing-stretch-0.01.dat' using ($1):($0 / $2 * 100) title '' with line ls 2,\
'sprint-failure-splicing-stretch-0.01.point' using ($1):($2 * 100) title 'Path Splicing' with linespoints ls 2

set origin 0,0
set xlabel 'Path Stretch with 10% Link Failure Probability' font "Helvetica, 21"
plot [1:4]\
'sprint-failure-ndn-stretch-0.1.dat' using ($1):($0 / $2 * 100) title '' with line ls 1,\
'sprint-failure-ndn-stretch-0.1.point' using ($1):($2 * 100) title 'NDN' with linespoints ls 1,\
'sprint-failure-splicing-stretch-0.1.dat' using ($1):($0 / $2 * 100) title '' with line ls 2,\
'sprint-failure-splicing-stretch-0.1.point' using ($1):($2 * 100) title 'Path Splicing' with linespoints ls 2

unset multiplot
