#!/usr/bin/perl
# DO NOT DISABLE without first checking with a ROSE core developer
my $desc = <<EOF;
We are not distributing the Tup build system yet, but this repo has Tup-related
files.
EOF


use strict;
my $warning;
my $nfail=0;

sub checkdir {
  my($dir) = @_;
  return if $dir =~ /\/CxxFrontend\/EDG\//;
  opendir DIR, $dir or return;
  my @entries = readdir DIR;
  close DIR;

  # Build the index hash. Each key is the lower-case name; each value is an array of names with this key
  for my $entry (@entries) {
    next if $entry =~ /^\.\.?$/;
    next if $entry =~ /^\.git$/; # Git can handle its own problems
    next if $entry =~ /^_build/; # typical names for build directories
    next if $entry =~ /^build_tree$/; # Jenkins' build directory
    checkdir("$dir/$entry") if -d "$dir/$entry" && ! -d "$dir/$entry/include_staging";

    if ($entry eq "Tupfile" || $entry eq "Tupfile.ini" || $entry eq "tup.config" || $entry eq "Tuprules.tup") {
	print $desc if 0 == $nfail;
	print "  $dir/$entry\n";
	++$nfail;
    }
  }
}

checkdir ".";
exit($nfail > 0 ? 1 : 0);
