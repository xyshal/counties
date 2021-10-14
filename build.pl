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
    my $path = ($^O eq "darwin") ? "../conanfile_mac.txt" : "..";
    system("conan install $path") == 0 or die $!;
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

RunCommandInConanEnv("cmake ..");

my $makeCmd = "make";

if ($^O eq "linux") {
  my $nproc = `nproc`;
  $makeCmd = "make -j$nproc";
} elsif ($^O eq "MSWin32") {
  $makeCmd = "msbuild qcounties.vcxproj";
}

RunCommandInConanEnv($makeCmd);

