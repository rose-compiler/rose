#!/usr/bin/perl

# See Makefile.am.

use strict;

my $pop_srcdir = shift @ARGV;
my @target_rmods = @ARGV;
my %rmod_by_dir = map {(/([^\/]*)\.rmod/, $_)} @target_rmods; # map from base name to full name.

for my $target (@target_rmods) {
  my $target_f90 = ($target =~ /pop_modified_blddir\/(.*)\.rmod/)[0] . ".F90";
  open SRC, "$pop_srcdir/$target_f90" or die "$pop_srcdir/$target_f90: $!";
  my @deps;
  while (<SRC>) {
    push @deps, $rmod_by_dir{$1} if /^\s*use\s+(\w+)/i && $rmod_by_dir{$1};
  }
  close SRC;
  print "$target: ", join(" ",@deps), "\n" if @deps;
}
