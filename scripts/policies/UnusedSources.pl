#!/usr/bin/perl
my $desc = <<EOF;
All source files should be mentioned in makefiles.  A source file that
is not mentioned in a makefile is probably not used. As of 2010-10-18
there are 1480 violations; we will allow no more! Future work will
reduce this number to zero or something reasonable. These source files
are not mentioned in any makefile:
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
  next unless $file =~ /\/(Makefile\.am|CMakeList\.txt)$/;
  if (open FILE, "<", $file) {
    while (<FILE>) {
      s/#.*//;
      s/([a-zA-Z_0-9\.]+)/delete $index{lc $1}/ge;
    }
    close FILE;
  }
}

# Report failures
my @remaining = map {@$_} values %index;
$warning = "" if @remaining > 1480; # as of 2010-10-18 there are 1480 violations; do not allow more!
print $desc if @remaining;
print "  $_$warning\n" for sort @remaining;



exit(@remaining ? ($warning?128:1) : 0);
