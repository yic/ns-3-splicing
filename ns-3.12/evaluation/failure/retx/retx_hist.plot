set term postscript eps color enh
set out 'hijacking-sprint-delivery.eps'
set bar 1.000000
set boxwidth 0.7 absolute
set style fill solid 1.00 border -1
set style rectangle back fc lt -3 fillstyle solid 1.00 border -1
set style histogram cluster gap 1 title offset character 0, 0, 0
set style data histograms
set datafile missing '-'
#set key bmargin center horizontal Left reverse enhanced autotitles columnhead nobox
set key left top vertical Left reverse enhanced autotitles columnhead nobox
set yrange [0:130] noreverse nowriteback
set ylabel "Percent of Points" font "Helvetica, 25"
set xlabel "Number of Retx" font "Helvetica, 25"
set size 0.7, 0.7

plot\
        'retx-10-20-0.01-count' using ($1 / 2652000) title 'test' col fs empty

