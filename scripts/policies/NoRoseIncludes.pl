#!/usr/bin/perl
# DO NOT DISABLE without first checking with a ROSE core developer

# Please do not increase this limit! Only decrease it.
my $allowedFailures = 14;




my $desc = <<EOF;
The following files must not be included in ROSE public header files because
they pollute the user's global name space with symbols whose names do not begin
with "ROSE", "Rose", or "rose":
   rose.h
   sage3.h
   sage3basic.h
   rose_config.h

An alternative is to include these (except rose.h) in .C files instead since
that would prevent any global symbols from leaking into user programs via
ROSE public header files.

The following header files violate this policy. Some of these violations may have
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

# If no files or directories are specified and the CWD is the top of the ROSE source tree, then check
# only the "src" directory.
@ARGV = "src" if !@ARGV && -d "scripts/policies";

my $nfail=0;
my $files = FileLister->new(@ARGV);
while (my $filename = $files->next_file) {
    # utility_functionsImpl.C is misnamed--it's actually a header file.
    if (($filename=~/\.(h|hh|hpp|code2|macro)$/ || $filename=~/\/utility_functionsImpl.C$/) &&
        !is_disabled($filename) && open FILE, "<", $filename) {
	while (<FILE>) {
	    if (/^\s*#\s*include\s*["<]((rose|sage3|sage3basic|rose_config)\.h)[>"]/ && !/\bPOLICY_OK\b/) {
		print $desc unless $nfail++;
		printf "  %1s (%1s)\n", $filename, $1;
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
