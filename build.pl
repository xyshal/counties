#!/usr/bin/perl

use File::Path;
use strict;

my $param = shift;

sub RunCommandInConanEnv
{
  my $cmd = shift or die;
  print "$cmd\n";
  my $activateScript = ($^O eq "MSWin32") ? "activate.bat" : "activate.sh";
  unless (-f $activateScript) {
    system("conan install ..") == 0 or die $!;
  }
  if ($^O eq "MSWin32") {
    system("cmd /c \"CALL activate.bat && CALL activate_build.bat && $cmd\"") == 0 or die $!;
  } else {
    system("bash -c \"source activate.sh && $cmd\"") == 0 or die $!;
  }
}

my $buildDir = "build";
if (-e $buildDir && $param =~ /clean/) {
  rmtree $buildDir if -e $buildDir;
}
unless (-e $buildDir) { mkdir $buildDir or die $!; }
chdir $buildDir or die $!;

if ($^O eq "MSWin32") {
  RunCommandInConanEnv("cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release");
  RunCommandInConanEnv("ninja");
} else {
  RunCommandInConanEnv("cmake ..");
  my $buildCmd = "make";
  if ($^O eq "linux") {
    my $nproc = `nproc`;
    $buildCmd .= " -j$nproc";
  }
  RunCommandInConanEnv($buildCmd);
}

# TODO: Fix this on Mac OS and Windows at some point
if ($^O eq "linux") {
  system("test/bin/test") == 0 or die "Test failures";
}
