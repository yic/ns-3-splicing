#!/usr/bin/perl

my $slices, $retx, $victim, $attacker;
&print_usage;

&run;

sub print_usage {
    if (@ARGV != 4) {
        print STDERR "Usage: $0 slices retx victim attacker\n";
        exit(-1);
    }

    $slices = $ARGV[0];
    $retx = $ARGV[1];
    $victim = $ARGV[2];
    $attacker = $ARGV[3];
}

sub run {
    mkdir("results");
    my $dir = "results/result-$slices-$retx";
    mkdir($dir);
    my $seed;

    $seed = time();
    system("echo Seed=$seed Victim=$victim Attacker=$attacker > $dir/result-$victim-$attacker");
    chdir("../..") or die($!);
    system("./build/debug/scratch/sprint-path-splicing-blackhole --SliceNumber=$slices --RetxNumber=$retx --VictimId=$victim --AttackerId=$attacker --RngSeed=$seed >> evaluation/blackhole/$dir/result-$victim-$attacker 2>&1");
    chdir("evaluation/blackhole") or die($!);
}
