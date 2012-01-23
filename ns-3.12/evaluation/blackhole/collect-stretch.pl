#!/usr/bin/perl

my $slices, $retx;
&print_usage;

my %latency, %weight;
&read_latency;
&read_weight;

my %shortest_latency, %shortest_weight, %current_latency, %current_weight;
my @array;
&collect;

sub print_usage {
    if (@ARGV != 2) {
        print STDERR "Usage: $0 slices retx\n";
        exit(-1);
    }

    $slices = $ARGV[0];
    $retx = $ARGV[1];
}

sub read_latency {
    open(LAT, "< ../../data/sprint/latency.orig");

    while (my $line = <LAT>) {
        chomp($line);

        if ($line =~ /(\d+)\s+(\d+)\s+(.+)/) {
            $latency{$1}{$2} = $3;
            $latency{$2}{$1} = $3;
        }
        else {
            print STDERR "wrong format: $line\n";
        }
    }

    close(LAT);
}

sub read_weight {
    open(WEIGHT, "< ../../data/sprint/weight.orig");

    while (my $line = <WEIGHT>) {
        chomp($line);

        if ($line =~ /(\d+)\s+(\d+)\s+(\d+)/) {
            $weight{$1}{$2} = $3;
            $weight{$2}{$1} = $3;
        }
        else {
            print STDERR "wrong format: $line\n";
        }
    }

    close(WEIGHT);
}

sub collect {
    my $dir = "results/result-$slices-$retx";

    if (-d $dir) {
        opendir(DIR, "$dir");

        my $src, $dst, $path, @path_array;

        while (my $file = readdir(DIR)) {
            if ($file =~ /^result-\d+-\d+$/) {
                %shortest_latency = ();
                %shortest_weight = ();
                %current_latency = ();
                %current_weight = ();

                open(FILE, "< $dir/$file");

                while (my $line = <FILE>) {
                    chomp($line);

                    if ($line =~ /Node (\d+) received reply #0 from 192.168.(\d+).2 .+ path: (.+)/) {
                        $src = $1 - 52;
                        $dst = $2;
                        $path = $3;
                        @path_array = split("-", $path);

                        foreach my $i (0 .. @path_array - 2) {
                            if ($path_array[$i] < 52 and $path_array[$i + 1] < 52) {
                                $shortest_latency{$src}{$dst} += $latency{$path_array[$i]}{$path_array[$i + 1]};
                                $shortest_weight{$src}{$dst} += $weight{$path_array[$i]}{$path_array[$i + 1]};
                            }
                        }

#                        print "$src $dst $shortest_latency{$src}{$dst} $shortest_weight{$src}{$dst}\n";
                    }
                    elsif ($line =~ /Node (\d+) received reply #1 from 192.168.(\d+).2 .+ path: (.+)/) {
                        $src = $1 - 52;
                        $dst = $2;
                        $path = $3;
                        @path_array = split("-", $path);

                        foreach my $i (0 .. @path_array - 2) {
                            if ($path_array[$i] < 52 and $path_array[$i + 1] < 52) {
                                $current_latency{$src}{$dst} += $latency{$path_array[$i]}{$path_array[$i + 1]};
                                $current_weight{$src}{$dst} += $weight{$path_array[$i]}{$path_array[$i + 1]};
                            }
                        }

#                        print "$src $dst $current_latency{$src}{$dst} $current_weight{$src}{$dst}\n";
                        my $latency_stretch = $current_latency{$src}{$dst} / $shortest_latency{$src}{$dst};
                        my $weight_stretch = $current_weight{$src}{$dst} / $shortest_weight{$src}{$dst};
#                        print "$src $dst $latency_stretch $weight_stretch\n";
                        push(@array, $weight_stretch);
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
