#!/usr/bin/perl

my $slices, $retx, $probability;
&print_usage;

&run;

sub print_usage {
    if (@ARGV != 3) {
        print STDERR "Usage: $0 slices retx probability\n";
        exit(-1);
    }

    $slices = $ARGV[0];
    $retx = $ARGV[1];
    $probability = $ARGV[2];
}

sub run {
    my $failure_file = "../data/sprint/failures-$probability";

    if (!(-f $failure_file)) {
        print STDERR "$failure_file does not exist.\n";
        exit;
    }

    open(FAILURE, "< $failure_file");
    my $i = 0;

    while (my $line = <FAILURE>) {
        chomp($line);

        my $dir = "result-$slices-$retx-$probability";
        mkdir($dir);

        my $seed = time();
        system("echo Seed=$seed > $dir/result-$i");
        chdir("..") or die($!);
        if ($line ne "") {
            system("./build/debug/scratch/sprint-path-splicing --SliceNumber=$slices --RetxNumber=$retx --FailedLinks=\"$line\" --RngSeed=$seed >> evaluation/$dir/result-$i 2>&1");
        }
        else {
            system("./build/debug/scratch/sprint-path-splicing --SliceNumber=$slices --RetxNumber=$retx --RngSeed=$seed >> evaluation/$dir/result-$i 2>&1");
        }
        chdir("evaluation") or die($!);

        $i ++;
    }

    close(FAILURE);
}
