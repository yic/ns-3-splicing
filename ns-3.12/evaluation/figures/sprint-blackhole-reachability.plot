set term postscript eps color enh
set output 'sprint-blackhole-reachability.eps'
set border 3 front linetype -1 linewidth 1.000
set boxwidth 0.50 absolute
set style fill solid 1.00 border -1
set grid nopolar
set grid noxtics nomxtics ytics nomytics noztics nomztics nox2tics nomx2tics noy2tics nomy2tics nocbtics nomcbtics
set grid layerdefault linetype 0 linewidth 1.000, linetype 0 linewidth 1.000
set key outside right top vertical Left reverse enhanced autotitles columnhead nobox
set key invert samplen 4 spacing 3 width 3 height 0 font "Helvetica, 20"
set style histogram rowstacked title offset character 0, 0, 0
set style data histograms
set xtics border in scale 1,0.5 nomirror offset character 0, 0, 0 font "Helvetica, 25"
set xtics ("NDN" 1.00000 -1, "Path Splicing" 2.00000 -1, "IP" 3.00000 -1)
set ylabel "Percentage of Flows (%)" font "Helvetica, 25"
set yrange [0.00000:100.000] noreverse nowriteback
plot 'sprint-blackhole-reachability.dat' using ($2 * 100) fill pattern 5 lc 3 lt -1, '' using ($3 * 100) fill pattern 4 lc 2 lt -1, '' using ($4 * 100) fill pattern 1 lc 1 lt -1
#plot 'sprint-blackhole-reachability.dat' using ($2 * 100) lc 3 lt -1, '' using ($3 * 100) lc 2 lt -1, '' using ($4 * 100) lc 1 lt -1
