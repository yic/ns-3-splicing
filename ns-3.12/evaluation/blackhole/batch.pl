#!/usr/bin/perl

my $slices, $retx, $round;
&print_usage;

my %pairs = ();
my $attacker, $victim;

foreach $i (0 .. $round - 1) {
    $victim = int(rand(52));
    $attacker = int(rand(52));

    while ($victim == $attacker || exists($pairs{$victim}{$attacker})) {
        $victim = int(rand(52));
        $attacker = int(rand(52));
    }

    $pairs{$victim}{$attacker} = 1;

    system("./run.pl $slices $retx $victim $attacker $i");
}

sub print_usage {
    if (@ARGV != 3) {
        print STDERR "Usage: $0 slices retx round\n";
        exit(-1);
    }

    $slices = $ARGV[0];
    $retx = $ARGV[1];
    $round = $ARGV[2];
}

