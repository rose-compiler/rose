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
my $warning = "warning ";	# non-empty means these are warnings rather than errors

my $nfail=0;
my $files = FileLister->new();
while (my $filename = $files->next_file) {
  if ($filename=~/\.(h|hh|hpp|code2|macro)$/ && open FILE, "<", $filename) {
    while (<FILE>) {
      if (/^#\s*include\s*["<](rose|sage3|sage3basic)\.h[>"]/) {
	print $desc unless $nfail++;
        printf "  %1s (%1s%1s.h)\n", $filename, $warning, $1;
        last;
      }
    }
    close FILE;
  }
}

exit($nfail>0 ? ($warning ? 128 : 1) : 0);
