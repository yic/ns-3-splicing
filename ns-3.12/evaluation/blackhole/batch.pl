#!/usr/bin/perl

my $slices, $retx, $start, $end;
&print_usage;

foreach $i (0 .. 51) {
    foreach $j ($start .. $end) {
        if ($i != $j) {
            system("./run.pl $slices $retx $i $j");
        }
    }
}

sub print_usage {
    if (@ARGV != 4) {
        print STDERR "Usage: $0 slices retx start end\n";
        exit(-1);
    }

    $slices = $ARGV[0];
    $retx = $ARGV[1];
    $start = $ARGV[2];
    $end = $ARGV[3];
}

