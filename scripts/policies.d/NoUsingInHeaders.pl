#!/usr/bin/perl
my $desc = <<EOF;
Header files should not have a "using namespace" directive in an outer scope
since this causes unexpected behavior for code that uses those headers. The
following header files violate this policy:
EOF

BEGIN {push @INC, $1 if $0 =~ /(.*)\//}
use strict;
use FileLister;

my $nfail=0;
my $files = FileLister->new();
while (my $filename = $files->next_file) {
  if ($filename =~ /\.(h|hh)$/ && open FILE, "<", $filename) {
    while (<FILE>) {
      if (/^\b(using\s+namespace\s+[a-z_A-Z]\w*)/) {
	print $desc unless $nfail++;
	print "  $filename ($1)\n";
	last;
      }
    }
    close FILE;
  }
}

# This is only a warning for now (exit with 128-255)
exit($nfail>0 ? 128 : 0);
