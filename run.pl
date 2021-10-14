#!/usr/bin/perl

use File::Slurp;
use strict;

if ($^O eq "MSWin32") {
  system("build/bin/qcounties.exe");
} elsif ($^O eq "linux") {
  $ENV{'FONTCONFIG_PATH'} = "/etc/fonts";
  system("build/bin/qcounties.exe");
} elsif ($^O eq "darwin") {
  # TODO: Just copy the libs like on Windows or figure out rpath on Mac
  my $conanInfo = read_file("build/conanbuildinfo.cmake");
  my $qtPath = $1 if $conanInfo =~ /CONAN_QT_ROOT "(.*)"/;
  die "Couldn't find Qt path" unless defined $qtPath;
  $ENV{'DYLD_LIBRARY_PATH'} = "$qtPath/lib";
  system("build/bin/qcounties");
}

