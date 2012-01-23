#!/usr/bin/perl

my $slices, $retx;
&print_usage;

&collect;

sub print_usage {
    if (@ARGV != 2) {
        print STDERR "Usage: $0 slices retx\n";
        exit(-1);
    }

    $slices = $ARGV[0];
    $retx = $ARGV[1];
}

sub collect {
    my ($probability) = @_;
    my $dir = "results/result-$slices-$retx";

    if (-d $dir) {
        opendir(DIR, "$dir");

        my $received, $giveup;

        while (my $file = readdir(DIR)) {
            if ($file =~ /^result-\d+-\d+$/) {
                $received = `grep "received reply #1" $dir\/$file | wc -l`;
                $giveup = `grep "give up request #1" $dir\/$file | wc -l`;

                chomp($received);
                chomp($giveup);

                if ($received + $giveup != 50) {
                    print STDERR "File $file error\n";
                }

                print "$received\n";
            }
        }
    }
    else {
        print STDERR "Directory $dir does not exist\n";
    }
}
