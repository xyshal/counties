#!/usr/bin/perl

use File::Path;
use strict;

my $param = shift;

sub RunCommandInConanEnv
{
  my $cmd = shift or die;
  print "$cmd\n";
  my $activateScript = ($^O eq "MSWin32") ? "conanbuild.bat" : "conanbuild.sh";
  unless (-f $activateScript) {
    system("conan install .. -of .") == 0 or die $!;
  }
  if ($^O eq "MSWin32") {
    system("cmd /c \"CALL conanbuild.bat && CALL conanbuild.bat && $cmd\"") == 0 or die $!;
  } else {
    system("bash -c \"source conanbuild.sh && $cmd\"") == 0 or die $!;
  }
}

my $buildDir = "build";
if (-e $buildDir && $param =~ /clean/) {
  rmtree $buildDir if -e $buildDir;
}
unless (-e $buildDir) { mkdir $buildDir or die $!; }
chdir $buildDir or die $!;

my $globalCmakeArgs = "-DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake";
if ($^O eq "MSWin32") {
  RunCommandInConanEnv("cmake .. -G Ninja $globalCmakeArgs");
  RunCommandInConanEnv("ninja");
} else {
  RunCommandInConanEnv("cmake .. $globalCmakeArgs");
  my $buildCmd = "make";
  if ($^O eq "linux") {
    my $nproc = `nproc`;
    $buildCmd .= " -j$nproc";
  }
  RunCommandInConanEnv($buildCmd);
}

system("test/test") == 0 or die "Test failure";
