#!/usr/bin/perl

my $probability;
&print_usage;

&run;

sub print_usage {
    if (@ARGV != 1) {
        print STDERR "Usage: $0 probability\n";
        exit(-1);
    }

    $probability = $ARGV[0];
}

sub run {
    my $failure_file = "../../data/sprint/failures-$probability";

    if (!(-f $failure_file)) {
        print STDERR "$failure_file does not exist.\n";
        exit;
    }

    open(FAILURE, "< $failure_file");
    my $i = 0;

    while (my $line = <FAILURE>) {
        chomp($line);

        my $dir = "result-$probability";
        mkdir($dir);

        chdir("../..") or die($!);
        if ($line ne "") {
            system("./build/debug/scratch/sprint-path-splicing-stretch --FailedLinks=\"$line\" >> evaluation/stretch/$dir/result-$i 2>&1");
        }
        else {
            system("./build/debug/scratch/sprint-path-splicing-stretch >> evaluation/stretch/$dir/result-$i 2>&1");
        }
        chdir("evaluation/stretch") or die($!);

        $i ++;
    }

    close(FAILURE);
}
