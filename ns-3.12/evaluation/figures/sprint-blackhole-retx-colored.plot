set term postscript eps color enh
set output 'sprint-blackhole-retx-colored.eps'
set border 3 front linetype -1 linewidth 1.000
set boxwidth 1.0 absolute
set style fill solid 1.00 noborder
set grid nopolar
set grid noxtics nomxtics ytics nomytics noztics nomztics nox2tics nomx2tics noy2tics nomy2tics nocbtics nomcbtics
set grid layerdefault linetype 0 linewidth 1.000, linetype 0 linewidth 1.000
set key inside right top vertical Left reverse enhanced autotitles columnhead nobox
set key invert samplen 4 spacing 3 width 3 height 0 font "Helvetica, 20"
set style histogram cluster gap 1 title offset character 0, 0, 0
set style data histograms
set xlabel "Number of Retransmission" font "Helvetica, 25"
set xtics border in scale 1,0.5 nomirror offset character 0, 0, 0 font "Helvetica, 20"
set xtics ("5" 4, "10" 9, "15" 14, "20" 19)
set ylabel "Percentage of Flows (%)" font "Helvetica, 25"
set yrange [0:2.5] noreverse nowriteback
plot 'sprint-blackhole-retx.dat' using 2 ti col lt -1 lc 1, '' using 3 ti col lt -1 lc 3
#plot 'sprint-blackhole-retx.dat' using 2 ti col fill pattern 1 lt -1 lc 1, '' using 3 ti col lt -1 lc 3 fill pattern 4
