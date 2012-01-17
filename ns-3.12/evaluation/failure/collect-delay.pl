#!/usr/bin/perl

my $slices, $retx, $prob;
&print_usage;

my %shortest_latency, %current_latency;
my @array;
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

        my $src, $dst, $rtt, $retx;

        while (my $file = readdir(DIR)) {
            if ($file =~ /^result-\d+$/) {
                %shortest_latency = ();
                %shortest_weight = ();
                %current_latency = ();
                %current_weight = ();

                open(FILE, "< $dir/$file");

                while (my $line = <FILE>) {
                    chomp($line);

                    if ($line =~ /Node (\d+) received reply #0 from 192.168.(\d+).2 after \+(.+)ns \(retx=(\d+)\)/) {
                        $src = $1 - 52;
                        $dst = $2;
                        $rtt = $3 / 1000000;
                        $retx = $4;

                        if ($retx > 0) {
                            print STDERR "error: $line\n";
                        }

                        $shortest_latency{$src}{$dst} = $rtt;
                    }
                    elsif ($line =~ /Node (\d+) received reply #1 from 192.168.(\d+).2 after \+(.+)ns \(retx=(\d+)\)/) {
                        $src = $1 - 52;
                        $dst = $2;
                        $rtt = $3 / 1000000;
                        $retx = $4;

                        $current_latency{$src}{$dst} = $shortest_latency{$src}{$dst} * $retx + $rtt;

#                        print "$src $dst $current_latency{$src}{$dst}\n";
                        push(@array, $current_latency{$src}{$dst});
                    }
                }

                close(FILE);
            }
        }

        my @sorted = sort {$a <=> $b} @array;
        my $num = @array;
        foreach my $k (@sorted) {
            print "$k $num\n";
        }
    }
    else {
        print STDERR "Directory $dir does not exist\n";
    }
}
