#!/usr/bin/perl

use strict;

my $color = shift;
$color = "blue" if not defined $color;

my $csv = "counties.csv";
my $input = "Usa_counties_large.svg";
foreach my $f( ($csv, $input) ) {
  die "Can't find $f" unless -e $f;
}

my $outputName = "counties";
my $svg = "$outputName.svg";
my $png = "$outputName.png";

foreach my $file( ($svg, $png) ) {
  unlink $file if -e $file;
}

open(my $fh, '<', "$csv") or die("Failed to open $csv");
chomp(my @contents = <$fh>);
close $fh;

# Throw the results in a hash.  This is not especially performant, but
# whatever; it's fast enough.
my %counties;
foreach my $line(@contents) {
  if ( $line =~ m/"([^"]*)",([0-9]*)/ ) {
    $counties{$1} = $2 if $2 != 0;
  }
}

# Open the original svg
open(my $INPUT, '<', "$input") or die("Failed to open $input");
chomp(my @contents = <$INPUT>);
close $INPUT;

# Add a fill, if our county was defined in the csv
open(my $OUTPUT, '>', "$svg") or die("Failed to open $svg");
foreach my $line(@contents) {
  if ( $line =~ m/id="([^"]*,[^"]*)"/ ) {
    my $county = $1;
    $line =~ s/\/>/ fill="$color" \/>/ if defined $counties{$county};
  }
  print $OUTPUT "$line\n";
}
close $OUTPUT;

my $inkscape = ($^O eq "MSWin32") ? "\"C:\\Program Files\\Inkscape\\inkscape.exe\""
                                  : "inkscape";

unlink $png if -e $png;
system("$inkscape -z -e $png -w 2400 $svg") == 0
  or warn("failed to save the png: $!");

