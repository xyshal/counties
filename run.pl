#!/usr/bin/perl

use File::Slurp;
use strict;

if ($^O eq "MSWin32") {
  system("build/src/bin/qcounties.exe");
} elsif ($^O eq "linux") {
  $ENV{'FONTCONFIG_PATH'} = "/etc/fonts";
  system("build/src/bin/qcounties");
} elsif ($^O eq "darwin") {
  system("build/src/qcounties");
}

