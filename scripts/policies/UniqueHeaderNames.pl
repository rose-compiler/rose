#!/usr/bin/perl
my $desc = <<EOF;
C/C++ header files should have names that are unique across the entire ROSE
source tree.  The uniqueness ensures that an #include directive will always
include the intended header regardless of the order of the search path.
Additionally, no ROSE header file should have the same name as a system
header file for the same reason, and so that code is not as sensitive to
whether the #include used quotes or angle brackets.  This policy makes the
name comparison in a case-insensitive manner since not all file systems
distinguish character case in file names.  The following header files violate
this policy:
EOF

BEGIN {push @INC, $1 if $0 =~ /(.*)\//}
use strict;
use FileLister;
my $warning = " (warning)";	# non-empty means issue warnings rather than errors

# Create an index of all header files.
# Each key is the base name of the header file;
# Each value is an array of header filenames that map to this key.
my %index;

# System headers
for my $dir (qw(/usr/include /usr/local/include)) {
  push @{$index{lc((/([^\/]+)$/)[0])}||=[]}, $_ for glob "$dir/*.h";
}

# Headers in ROSE
push @{$index{lc((/([^\/]+)$/)[0])}||=[]}, $_ for grep {/\.(h|hh)/} FileLister->new()->all_files;


# Report failures
my $nfail = 0;
for my $key (sort keys %index) {
  next if 1==@{$index{$key}};
  print $desc unless $nfail++;
  print "  $key$warning\n";
  print "    $_\n" for @{$index{$key}};
}

# This is only a warning for now (exit with 128-255)
exit($nfail>0 ? ($warning?128:1) : 0);
