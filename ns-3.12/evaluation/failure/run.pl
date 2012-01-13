#!/usr/bin/perl

my $slices, $retx, $probability, $round;
&print_usage;

&run;

sub print_usage {
    if (@ARGV != 4) {
        print STDERR "Usage: $0 slices retx probability round\n";
        exit(-1);
    }

    $slices = $ARGV[0];
    $retx = $ARGV[1];
    $probability = $ARGV[2];
    $round = $ARGV[3];
}

sub run {
    my $dir = "result-$slices-$retx-$probability";
    mkdir($dir);
    my $seed;

    foreach my $i (0 .. $round - 1) {
        $seed = time();
        system("echo Seed=$seed > $dir/result-$i");
        chdir("../..") or die($!);
        system("./build/debug/scratch/sprint-path-splicing --SliceNumber=$slices --RetxNumber=$retx --FailureProbability=$probability --RngSeed=$seed >> evaluation/failure/$dir/result-$i 2>&1");
        chdir("evaluation/failure") or die($!);
    }
}
