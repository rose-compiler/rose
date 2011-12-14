#! /usr/bin/perl

use strict;
use warnings;
use Cwd;

my $buildInfoLog       = "polyrose-build-info.log";
my $buildErrorLog      = "polyrose-build-errors.log";

my $svn                = "svn";
my $svnUsername        = "";
my $svnPassword        = "";
my $svnRepoUrl         = "https://barista.cse.ohio-state.edu/svn/plutorose/trunk";
my @svnCheckOutArgs    = ( );

# The directory where the SVN repo would be checked out and build
# If left blank, CWD/builds is automatically used for this
my $baseDir            = "";
my $correctnessTest    = "tests/correctness/run-correctness.pl";
my $regressionTest     = "tests/regression/polybenchs/run-regression.pl";
my $checkOutSubDir     = "latest";
my $svnCheckOutDir     = "";
if ($baseDir eq "") {
  $svnCheckOutDir = getcwd . "/builds/$checkOutSubDir";
}
else {
  $svnCheckOutDir = "$baseDir/$checkOutSubDir";
}

sub ZeroPrefixFill
{
  my $number = shift(@_);
  if ($number < 10) {
    return sprintf("0%d", $number);
  }
  else {
    return $number;
  }
}

sub RenameExistingFile
{
  my $file = shift(@_);
  if (-e $file) {
    my $modTime = (stat($file))[9];
    my ($year, $mon, $mday, $hour, $min, $sec) =
      (localtime($modTime))[5, 4, 3, 2, 1, 0];
    $year += 1900;
    $mon += 1;
    $mon = ZeroPrefixFill($mon);
    $mday = ZeroPrefixFill($mday);
    $hour = ZeroPrefixFill($hour);
    $min = ZeroPrefixFill($min);
    $sec = ZeroPrefixFill($sec);

    my $oldFile =  $file . "_" . "$year-$mon-$mday" .
                   "_" . "$hour-$min-$sec";
    
    print "### Moving existing $file to $oldFile\n";
    rename($file, $oldFile);
  }
}

# Creates a directory and its parent directories as required
sub CreateDir
{
  my $dir = shift(@_);
  use File::Path qw(mkpath);
  mkpath($dir);
}

sub WriteInfoLog
{
  my $log = shift(@_);
  print INFO_LOG $log;
  print $log;
}

sub WriteErrorLog
{
  my $log = shift(@_);
  print ERROR_LOG $log;
}

# Rename existing log files
RenameExistingFile($buildInfoLog);
RenameExistingFile($buildErrorLog);

# Create the base dir
CreateDir($baseDir);
# Rename the existing check out directory if it already exists
RenameExistingFile($svnCheckOutDir);
# Create the new check out directory
CreateDir($svnCheckOutDir);

open INFO_LOG, ">$buildInfoLog"
  or die("FATAL:  Could not open the $buildInfoLog file for writing!\n");

open ERROR_LOG, ">$buildErrorLog"
  or die("FATAL:  Could not open the $buildErrorLog file for writing!\n");

# Add the username and password arguments only if they are non-empty
if ($svnUsername ne "" && $svnPassword ne "") {
  push(@svnCheckOutArgs, "--username $svnUsername");
  push(@svnCheckOutArgs, "--password $svnPassword");
}

# Get the info about the repository from the server and write it
# into the log
my $command = "$svn info $svnRepoUrl";
my $output = `$command`;
if ($? != 0) {
  my $errorLog = "### Failed Command: $command\n### Details:\n$output";
  WriteErrorLog($errorLog);
  my $infoLog = "Getting the repo info failed for $svnRepoUrl, aborting...";
  WriteInfoLog($infoLog);
  exit(1);
}
else {
  WriteInfoLog(">> $command\n\n$output\n");
}

# Check out the SVN repo
$command = "$svn co $svnRepoUrl $svnCheckOutDir @svnCheckOutArgs 2>&1";
$output = `$command`;
if ($? != 0) {
  my $errorLog = "### Failed Command: $command\n### Details:\n$output";
  WriteErrorLog($errorLog);
  my $infoLog = "Checking out the repo failed for $svnRepoUrl, aborting...";
  WriteInfoLog($infoLog);
  exit(1);
}
else {
  WriteInfoLog(">> $command\n\nChecked out $svnRepoUrl successfully\n");
}

# Build the checked out copy
my $presentDir = getcwd;
chdir($svnCheckOutDir);
$command = "./install.sh 2>&1";
$output = `$command`;
if ($? != 0) {
  my $errorLog = "### Failed Command: $command### Details:\n$output";
  WriteErrorLog($errorLog);
  my $infoLog = "Building the checked out copy failed, aborting...";
  WriteInfoLog($infoLog);
  chdir($presentDir);
  exit(1);
}
else {
  WriteInfoLog(">> $command\n\nBuild of checked out copy completed successfully\n");
}
chdir($presentDir);

# Run the regression test
$command = "perl $svnCheckOutDir/$regressionTest 2>&1";
$output = `$command`;
if ($? != 0) {
  my $errorLog = "### Failed Command: $command### Details:\n$output";
  WriteErrorLog($errorLog);
  my $infoLog = "Executing the Regression test failed...";
  WriteInfoLog($infoLog);
}
else {
  WriteInfoLog(">> $command\n\nRegression test executed successfully\n");
}

# Run the correctness test
$command = "perl $svnCheckOutDir/$correctnessTest 2>&1";
$output = `$command`;

if ($? != 0) {
  my $errorLog = "### Failed Command: $command### Details:\n$output";
  WriteErrorLog($errorLog);
  my $infoLog = "Executing the correctness test failed, aborting...";
  WriteInfoLog($infoLog);
}
else {
  WriteInfoLog(">> $command\n\nCorrectness test executed successfully\n");
}

# TODO - Tar all the log files and email the results probably
# with a summary of the results

