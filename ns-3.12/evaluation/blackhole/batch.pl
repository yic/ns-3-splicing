#!/usr/bin/perl

my $slices, $retx, $min, $max, $round;
&print_usage;

my $node_num = 52;

foreach my $i (0 .. $node_num - 1) {
    foreach my $j ($min .. $max) {
        if ($i != $j) {
            system("./run.pl $slices $retx $i $j $round");
        }
    }
}

sub print_usage {
    if (@ARGV != 5) {
        print STDERR "Usage: $0 slices retx min max round\n";
        exit(-1);
    }

    $slices = $ARGV[0];
    $retx = $ARGV[1];
    $min = $ARGV[2];
    $max = $ARGV[3];
    $round = $ARGV[4];
}

