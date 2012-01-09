#!/usr/bin/perl

my $slices;
&print_usage;

my @probabilities = (0.01, 0.02, 0.03, 0.04, 0.05, 0.06, 0.07, 0.08, 0.09, 0.1);

foreach my $p (@probabilities) {
    &collect($p);
}

sub print_usage {
    if (@ARGV != 1) {
        print STDERR "Usage: $0 slices\n";
        exit(-1);
    }

    $slices = $ARGV[0];
}

sub collect {
    my ($probability) = @_;
    my $dir = "result-$slices-$probability";

    if (-d $dir) {
        opendir(DIR, "$dir");

        my $received, $giveup, $total_received, $total_giveup;
        $total_received = $total_giveup = 0;

        while (my $file = readdir(DIR)) {
            if ($file =~ /^result-\d+-\d+$/) {
                $received = `grep "received reply #1" $dir\/$file | wc -l`;
                $giveup = `grep "give up request #1" $dir\/$file | wc -l`;

                chomp($received);
                chomp($giveup);

                if ($received + $giveup != 2652) {
                    print STDERR "File $file error\n";
                }
                else {
                    $total_received += $received;
                    $total_giveup += $giveup;
                }
            }
        }

        print "$slices $probability $total_received $total_giveup\n";
    }
    else {
        print STDERR "Directory $dir does not exist\n";
    }
}
