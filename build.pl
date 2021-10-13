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

RunCommandInConanEnv("cmake ..");

my $makeCmd = "make";

if ($^O eq "linux") {
  my $nproc = `nproc`;
  $makeCmd = "make -j$nproc";
} elsif ($^O eq "MSWin32") {
  $makeCmd = "msbuild qcounties.vcxproj";
}

RunCommandInConanEnv($makeCmd);

# QT_QPA_PLATFORM_PLUGIN_PATH = $CONAN_QT_ROOT/res/archdatadir/plugins
# FONTCONFIG_PATH=/etc/fonts
# DYLD_LIBRARY_PATH on Mac OS
