#!/usr/bin/perl
# DO NOT DISABLE without first checking with a ROSE core developer

# Please do not increase this limit! Only decrease it.
my $allowedFailures = 15;

my $desc = <<EOF;
Header files should not have a "using namespace" directive in an outer scope
since this causes unexpected behavior for code that uses those headers.

The following files violate this policy. Some of these violations may have
existed prior to this check, and you have triggered this failure by introducing
at least one new violation. Our goal is to ultimately eliminate all of these
violations so the list is more relevant to you.  It does not matter which
violation(s) you fix to get back below the threshold -- if you have time and
are feeling generous, fix a few! Thank you.
EOF

BEGIN {push @INC, $1 if $0 =~ /(.*)\//}
use strict;
use FileLister;
use Policies;

my $nfail=0;
my $files = FileLister->new();
while (my $filename = $files->next_file) {
  next if $filename =~ /\b(tests|projects)\//; # skip test and project directories
  # utility_functionsImpl.C is misnamed -- it's actually a header file.
  if (($filename =~ /\.(h|hh|hpp)$/ || $filename =~ /\/utility_functionsImpl.C$/) &&
      !is_disabled($filename) && open FILE, "<", $filename) {
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

if ($nfail > $allowedFailures) {
    print <<EOF;
Your changes introduce a new violation of this policy.  Please fix it, and while
you're at it, fix any other violations in files that you "own".
EOF
    exit 1;
} elsif ($nfail < $allowedFailures) {
    print <<EOF;
Congratulations, you've improved the librose source quality by fixing violations of
this policy.  Now, please edit the top of $0
and replace the line
    my \$allowedFailures = $allowedFailures;
with
    my \$allowedFailures = $nfail;
Thank you.
EOF
    exit 1;
}

exit($nfail>0 ? 128 : 0);
