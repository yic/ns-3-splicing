#!/usr/bin/perl

my $slices;
&print_usage;

&collect;

sub print_usage {
    if (@ARGV != 1) {
        print STDERR "Usage: $0 slices\n";
        exit(-1);
    }

    $slices = $ARGV[0];
}

sub collect {

}
