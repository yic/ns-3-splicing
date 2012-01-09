#!/usr/bin/perl

my $slices, $probability, $round;
&print_usage;

&run;

sub print_usage {
    if (@ARGV != 3) {
        print STDERR "Usage: $0 slices probability round\n";
        exit(-1);
    }

    $slices = $ARGV[0];
    $probability = $ARGV[1];
    $round = $ARGV[2];
}

sub run {
    my $dir = "result-$slices-$probability";
    mkdir($dir);
    my $seed;

    foreach my $i (0 .. $round - 1) {
        $seed = time();
        chdir("..") or die($!);
        system("./build/debug/scratch/sprint-path-splicing --FailureProbability=$probability --RngSeed=$seed > evaluation/$dir/result-$i-$seed 2>&1");
        chdir("evaluation") or die($!);
    }
}
