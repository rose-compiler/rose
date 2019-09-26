#!/usr/bin/perl
# DO NOT DISABLE without first checking with a ROSE core developer

# Please do not increase this limit! Only decrease it.
my $allowedFailures = 31;




my $desc = <<EOF;
The "rose.h" header is intended to be for users that don't want to figure out
the correct minimal set of header files, but just want all declarations with
no hassles. Since rose.h includes all declarations, using it within the
ROSE library source code increases header coupling unnecessarily and slows down
compile times. For a small prototype project this may be acceptable, but
if we allowed ROSE developers to do this.... well, let's just say there's
over a thousand translation units that need to be copiled to create librose.

The following files violate this policy. Some of these violations may have
existed prior to this check, and you have triggered this failure by introducing
at least one new violation. Our goal is to ultimately eliminate all of these
violations so the list is more relevant to you.  It does not matter which
violation(s) you fix to get back below the threshold -- if you have time and
are feeling generous, fix a few! Thank you.

Fix your code by replacing rose.h with sage3basic.h followed by any additional
headers that are necessary. Please don't include your headers directly in
sagebasic.h only for convenience; a programming best practice is to minimize
header coupling.
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
    if ($filename =~ /\.(C|cpp)$/ && $filename !~ /\/utility_functionsImpl.C$/ &&
             !is_disabled($filename) && open FILE, "<", $filename) {
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
