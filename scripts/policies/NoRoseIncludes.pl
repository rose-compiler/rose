#!/usr/bin/perl
# DO NOT DISABLE without first checking with a ROSE core developer

# Please do not increase this limit! Only decrease it.
my $allowedFailures = 47;




my $desc = <<EOF;
The "rose.h" header file should not be included into any ROSE library header
files.  Include "sage3basic.h" in the ROSE library .C file before including
the header file instead.

The "sage3.h" and "sage3basic.h" header files should not be included into any
ROSE library header file because they pollute the user's global namespace with
preprocessor symbols that don't begin with "ROSE_".  Instead, you may include
these files into ROSE library .C files before including the ROSE library header
file as long as the header will behave correctly in user code when a user
first includes <rose.h>.

The "rose_config.h" header should not be included into any ROSE library header
file because it pollutes the user's global namespace with preprocessor symobls
that don't begin with "ROSE_".  If the header needs configuration symbols then
include <rosePublicConfig.h> into the header instead; otherwise include
"rose_config.h" into the ROSE library .C file instead.

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

# If no files or directories are specified and the CWD is the top of the ROSE source tree, then check
# only the "src" directory.
@ARGV = "src" if !@ARGV && -d "scripts/policies";

my $nfail=0;
my $files = FileLister->new(@ARGV);
while (my $filename = $files->next_file) {
    if ($filename=~/\.(h|hh|hpp|code2|macro)$/ && !is_disabled($filename) && open FILE, "<", $filename) {
	while (<FILE>) {
	    if (/^\s*#\s*include\s*["<]((rose|sage3|sage3basic|rose_config)\.h)[>"]/ && !/\bPOLICY_OK\b/) {
		print $desc unless $nfail++;
		printf "  %1s (%1s)\n", $filename, $1;
		last;
	    }
	}
	close FILE;
    } elsif ($filename =~ /\.(C|cpp)$/ && !is_disabled($filename) && open FILE, "<", $filename) {
	while (<FILE>) {
	    if (/^\s*#\s*include\s*["<](rose\.h)[>"]/ && !/\bPOLICY_OK\b/) {
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
