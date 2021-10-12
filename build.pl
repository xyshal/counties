#!/usr/bin/perl

use File::Path;
use strict;

my $param = shift;

sub RunCommandInConanEnv
{
  my $cmd = shift or die;
  print "$cmd\n";
  unless (-f "activate.sh") {
    system("conan install ..") == 0 or die $!;
  }
  system("bash -c \"source activate.sh && $cmd\"") == 0 or die $!;
}

my $buildDir = "build";
if (-e $buildDir && $param =~ /clean/) {
  rmtree $buildDir if -e $buildDir;
}
unless (-e $buildDir) { mkdir $buildDir or die $!; }
chdir $buildDir or die $!;

RunCommandInConanEnv("cmake ..");

my $nproc = `nproc`;
RunCommandInConanEnv("make -j$nproc");

# QT_QPA_PLATFORM_PLUGIN_PATH = $CONAN_QT_ROOT/res/archdatadir/plugins
# FONTCONFIG_PATH=/etc/fonts

