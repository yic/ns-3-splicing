#!/usr/bin/perl

my $slices, $retx, $prob;
&print_usage;

&collect($prob);

sub print_usage {
    if (@ARGV != 3) {
        print STDERR "Usage: $0 slices retx probability\n";
        exit(-1);
    }

    $slices = $ARGV[0];
    $retx = $ARGV[1];
    $prob = $ARGV[2];
}

sub collect {
    my ($probability) = @_;
    my $dir = "result-$slices-$retx-$probability";

    if (-d $dir) {
        opendir(DIR, "$dir");

        my @retx_array, @tmp, $giveup, $sum, $total;

        while (my $file = readdir(DIR)) {
            if ($file =~ /^result-\d+$/) {
                open(FILE, "< $dir/$file");

                $sum = 0;
                while (my $line = <FILE>) {
                    chomp($line);

                    if ($line =~ /received reply #1 .+ \(retx=(\d+)\)/) {
                        $retx_array[$1] ++;
                        $sum ++;
                    }
                    elsif ($line =~ /give up request #1/) {
                        $retx_array[$retx + 1] ++;
                        $sum ++;
                    }
                }

                if ($sum != 2652) {
                    print STDERR "File $file error\n";
                }
                $total += $sum;

                close(FILE);
            }
        }

        my $k = 1;
        foreach my $i (0 .. $retx + 1) {
            foreach my $j (1 .. $retx_array[$i]) {
                print "$i $k " . ($k / $total) . "\n";
                $k ++;
            }
        }
    }
    else {
        print STDERR "Directory $dir does not exist\n";
    }
}
