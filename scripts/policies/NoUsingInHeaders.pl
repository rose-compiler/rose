#!/usr/bin/perl
my $desc = <<EOF;
Header files should not have a "using namespace" directive in an outer scope
since this causes unexpected behavior for code that uses those headers. The
following header files violate this policy:
EOF

BEGIN {push @INC, $1 if $0 =~ /(.*)\//}
use strict;
use FileLister;
my $warning = "warning ";	# non-empty means issue warnings rather than errors

my $nfail=0;
my $files = FileLister->new();
while (my $filename = $files->next_file) {
  if ($filename =~ /\.(h|hh|hpp)$/ && open FILE, "<", $filename) {
    while (<FILE>) {
      if (/^\b(using\s+namespace\s+[a-z_A-Z]\w*)/) {
	print $desc unless $nfail++;
	print "  $filename ($warning$1)\n";
	last;
      }
    }
    close FILE;
  }
}

exit($nfail>0 ? ($warning?128:1) : 0);
