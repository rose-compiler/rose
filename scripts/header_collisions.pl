#!/usr/bin/perl
# Looks for ROSE header files that might conflict with system header files.

use File::Basename;
use strict;

# Parse command-line switches
my $test_against_self;		# look for conflicts within ROSE itself?
for (@ARGV) {
  if ($_ eq "--self") {
    $test_against_self = 1;
  } else {
    die "usage: $0 [--self]\n";
  }
}

# Get the list of system header files
my %headers = map {((fileparse $_)[0], $_)} split '\n', `find /usr/include -maxdepth 1 -name '*.h'`;

# Get the list of headers in the CWD and below
for (sort split '\n', `find . \\( -name '_build*' -prune \\) -o -name '*.h'`) {
  my($basename) = fileparse $_;
  print "$_ conflicts with $headers{$basename}\n" if $headers{$basename};
  $headers{$basename} ||= $_ if $test_against_self;
}
