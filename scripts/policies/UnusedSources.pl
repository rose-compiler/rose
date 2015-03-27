#!/usr/bin/perl
# DO NOT DISABLE without first checking with a ROSE core developer

# Number of failures allowed
my $limit = 1540; # ONLY DECREASE THIS VALUE!

my $desc = <<EOF;
All source files should be mentioned in the build system (makefiles).
A source file that is not mentioned is probably not used (in which case
it should be removed) or not distributed (in which case the makefile
may need to be fixed).  This policy checker allows $limit failures
only because we have not yet had time to fix all of them, and you have
introduced new failures beyond the limit.  Please scan the following
list for any files you own and fix the build system in one of three
ways:
   (1) Fix the build system by adding a target that uses the file.
   (2) Add the file to EXTRA_DIST or similar so it is distributed.
   (3) Mention the file in some rule or variable to indicate your
       acknowlegement that you intentionally do not use the file.

After fixing your files (and perhaps some others if you're feeling
generous), rerun this policy checker (e.g., "make check-policies"
in the "src" build directory), look at how many failures are still
present, and adjust the limit downward at the top of the
src/policies/UnusedSources.pl perl script. We want the limit to
eventually reach zero, which will make future failures much easier
to spot.  Sorry for any inconvenience.
EOF

BEGIN {push @INC, $1 if $0 =~ /(.*)\//}
use strict;
use FileLister;
my $warning = " (warning)";	# non-empty means issue warnings rather than errors, but see below

# Create an index of all source files.
# Each key is the base name of the file;
# Each value is an array of filenames that map to this key.
my %index;
push @{$index{lc((/([^\/]+)$/)[0])}||=[]}, $_ for grep {/\.(h|hh|hpp|c|C|cpp)$/} FileLister->new(@ARGV)->all_files;

# Look for file names in makefiles (Makefile.am and CMakeList.txt) and remove those that we find
# from the %index.
my $files = FileLister->new(@ARGV);
while (my $file = $files->next_file) {
  next unless $file =~ /\/(Makefile\.am|Makefile_variables|CMakeList\.txt|make_rule\.inc|Makefile-.*\.inc)$/;
  if (open FILE, "<", $file) {
    while (<FILE>) {
      s/#.*//;
      s/([-a-zA-Z_0-9\.]+)/delete $index{lc $1}/ge;
    }
    close FILE;
  }
}

# Report failures
my @remaining = map {@$_} values %index;
$warning = "" if @remaining > $limit;
print $desc if @remaining;
print "  $_$warning\n" for sort @remaining;

print "Number of remaining unused source files (limit=$limit): " . @remaining . ".\n";


exit(@remaining ? ($warning?128:1) : 0);
