#!/usr/bin/perl
my $desc = <<EOF;
Header files which are part of ROSE should not include "rose.h", "sage3.h", or
"sage3basic.h".  If you need functionality from these three headers then
include the header(s) in the .C file instead.  The following header files
violate this policy:
EOF


BEGIN {push @INC, $1 if $0 =~ /(.*)\//}
use strict;
use FileLister;

my $nfail=0;
my $files = FileLister->new();
while (my $filename = $files->next_file) {
  if ($filename=~/\.(h|hh|code2|macro)$/ && open FILE, "<", $filename) {
    while (<FILE>) {
      if (/^#\s*include\s*["<](rose|sage3|sage3basic)\.h[>"]/) {
	print $desc unless $nfail++;
        printf "  %1s (%1s.h)\n", $filename, $1;
        last;
      }
    }
    close FILE;
  }
}

# This is only a warning for now (exit with 128-255)
exit($nfail>0 ? 128 : 0);
