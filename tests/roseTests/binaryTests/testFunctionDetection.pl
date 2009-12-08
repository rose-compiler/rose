#!/usr/bin/perl
#
# Runs the functionBoundaries tool on two versions of a binary file, the file specified on the command line and a stripped
# version thereof. We then compare the functions that were found.
use strict;
my $functionBoundaries = "./functionBoundaries";

sub find_functions {
  my($binary) = @_;
  my %retval;
  open FUNCS, "-|", "$functionBoundaries $binary" or die "cannot run $functionBoundaries";
  while (<FUNCS>) {
    my($id,$start,$size,$end,$reason,$kind,$name) =
      /^\s+(\d+)\s+(0x[\da-f]+)\s+(0x[\da-f]+)\s+(0x[\da-f]+)\s+([\.ECXSPGU]{7})\s+(\w+)\s*(.*)/;
    next unless $kind;
    $retval{$start} = {start=>$start, size=>$size, reason=>$reason, kind=>$kind, name=>$name, str=>$_};
  }
  close FUNCS || die "$functionBoundaries failed";
  return %retval;
}

sub stripped {
  my($binary) = @_;
  my($stripped) = "x-stripped";
  system "cp", $binary, $stripped and die "$binary: cannot copy file";
  system "strip", $stripped and die "$stripped: cannot strip binary";
  return $stripped;
}

for my $original (@ARGV) {
  my $stripped = stripped $original;
  my($original_size) = (stat $original)[7];
  my($stripped_size) = (stat $stripped)[7];
  if ($original_size == $stripped_size) {
    print "$original: appears to be stripped already\n";
    next;
  }

  my %f1 = find_functions $original;
  my %f2 = find_functions $stripped;
  unlink $stripped;

  print "original=", scalar(keys %f1), "  stripped=", scalar(keys %f2), "\n";
  my $note = "The following functions are missing from the stripped version:\n";
  for my $addr (sort keys %f1) {
    next if $f2{$addr};
    print $note, $f1{$addr}{str};
    $note="";
  }
}
