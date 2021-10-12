#!/usr/bin/perl

use strict;

my $csv = "counties.csv";
die "Refuse to overwrite existing $csv" if -e $csv;

my $svg = "resources/Usa_counties_large.svg";
die "$svg does not exist" unless -f $svg;

open(my $SVG, '<', "$svg") or die("Failed to open $svg");
chomp(my @contents = <$SVG>);
close $SVG;

open(my $CSVFILE, '>', "$csv") or die("Failed to open $csv");
foreach my $line(@contents) {
  print $CSVFILE "\"$1\",0\n" if $line =~ m/id="([^"]*,[^"]*)"/;
}
close $CSVFILE;

