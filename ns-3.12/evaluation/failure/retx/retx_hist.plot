set term postscript eps color enh
set out 'retx-hist.eps'
set bar 1.000000
set boxwidth 0.7 absolute
set style fill solid 1.00 border -1
set style rectangle back fc lt -3 fillstyle solid 1.00 border -1
set style histogram cluster gap 1 title offset character 0, 0, 0
set style data histograms
set datafile missing '-'
set key left top vertical Left reverse enhanced autotitles columnhead nobox
set xrange [-1:22]
set yrange [0:25] noreverse nowriteback
set ylabel "Percent of Points" font "Helvetica, 25"
set xlabel "Number of Retx" font "Helvetica, 25"
set size 0.7, 0.7
set xtics ("0" 0, "3" 3, "6" 6, "9" 9, "12" 12, "15" 15, "18" 18, "others" 21)

plot\
'retx-10-20-0.01-count' using ($1 / 26520) ti col fs,\
'retx-10-20-0.05-count' using ($1 / 26520) ti col fs,\
'retx-10-20-0.1-count' using ($1 / 26520) ti col fs
