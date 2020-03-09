#!/usr/bin/perl
# DO NOT DISABLE without first checking with a ROSE core developer
my $desc = <<EOF;
Any rose source file that includes "sage3basic.h" must do so before including
any other file.  The following files violate this policy:
EOF


BEGIN {push @INC, $1 if $0 =~ /(.*)\//}
use strict;
use FileLister;
use Policies;
my $warning = "";	# non-empty means these are warnings rather than errors; e.g., " (warning)"

my $nfail=0;
my $files = FileLister->new(@ARGV);
while (my $filename = $files->next_file) {
  if ($filename=~/\.(C|cpp)$/ && !is_disabled($filename) && open FILE, "<", $filename) {
    my $previous_includes = 0;
    while (<FILE>) {
      if (/^\s*#\s*include\s*["<](rosePublicConfig\.h|rose_config\.h)[>"]/) {
	# Including <rosePublicConfig.h> or <rose_config.h> doesn't count. Although it
	# disables the precompiled header support, it's typically used to enable or disable
	# entire source files. Doing so can end up being faster even than using precompiled
	# headers.
      } elsif (/^#\s*include\s*["<]sage3basic\.h[>"]/ && $previous_includes>0) {
	print $desc unless $nfail++;
	print "  ", $filename, $warning, "\n";
	last;
      } elsif (/^\#\s*include\s*["<"].*\.h[>"]/) {
	$previous_includes++;
      }
    }
    close FILE;
  }
}

exit($nfail>0 ? ($warning ? 128 : 1) : 0);
