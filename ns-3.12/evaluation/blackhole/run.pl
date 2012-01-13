#!/usr/bin/perl

my $slices, $retx, $victim, $attacker, $round;
&print_usage;

&run;

sub print_usage {
    if (@ARGV != 5) {
        print STDERR "Usage: $0 slices retx victim attacker round\n";
        exit(-1);
    }

    $slices = $ARGV[0];
    $retx = $ARGV[1];
    $victim = $ARGV[2];
    $attacker = $ARGV[3];
    $round = $ARGV[4];
}

sub run {
    mkdir("results");
    my $dir = "results/result-$slices-$retx-$victim-$attacker";
    mkdir($dir);
    my $seed;

    foreach my $i (0 .. $round - 1) {
        $seed = time();
        system("echo Seed=$seed > $dir/result-$i");
        chdir("../..") or die($!);
        system("./build/debug/scratch/sprint-path-splicing-blackhole --SliceNumber=$slices --RetxNumber=$retx --VictimId=$victim --AttackerId=$attacker --RngSeed=$seed >> evaluation/blackhole/$dir/result-$i 2>&1");
        chdir("evaluation/blackhole") or die($!);
    }
}
