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
    my $dir = "results/result-$slices-$retx";

    if (-d $dir) {
        opendir(DIR, "$dir");

        my @retx_array, @tmp, $giveup, $sum, $total;

        while (my $file = readdir(DIR)) {
            if ($file =~ /^result-\d+-\d+$/) {
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

                if ($sum != 50) {
                    print STDERR "File $file error\n";
                }
                $total += $sum;

                close(FILE);
            }
        }

        foreach my $i (0 .. $retx) {
            print "$i $retx_array[$i] " . ($retx_array[$i] / $total) . "\n";
        }
        print "unreachable $retx_array[$retx + 1] " . ($retx_array[$retx + 1] / $total) . "\n";
    }
    else {
        print STDERR "Directory $dir does not exist\n";
    }
}
