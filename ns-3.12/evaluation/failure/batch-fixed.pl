#!/usr/bin/perl

my $slices, $retx, $min, $max;
&print_usage;

my @probabilities = (0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.07, 0.08, 0.09, 0.1);

foreach my $i ($min .. $max) {
    system("./run-fixed.pl $slices $retx $probabilities[$i]");
}

sub print_usage {
    if (@ARGV != 4) {
        print STDERR "Usage: $0 slices retx min max\n";
        exit(-1);
    }

    $slices = $ARGV[0];
    $retx = $ARGV[1];
    $min = $ARGV[2];
    $max = $ARGV[3];
}

