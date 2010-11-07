#!/usr/bin/perl
my $desc = <<EOF;
All header files found in a source tree should be used somewhere in that source
tree. Unused header files are characteristic of deleted code that should be
removed.  This policy only considers #include directives that don't specify a
directory since all ROSE include directories are specified with the "-I" switch
of the compiler.  A separate policy checks that header file names are unique so
that the "-I" switch behaves reasonably.  The following header files exist
but are unused:
EOF

BEGIN {push @INC, $1 if $0 =~ /(.*)\//}
use strict;
use FileLister;
my $warning = " (warning)";	# non-empty means issue warnings rather than errors, but see below

# Create an index of all header files.
# Each key is the base name of the header file;
# Each value is an array of header filenames that map to this key.
my %index;
push @{$index{lc((/([^\/]+)$/)[0])}||=[]}, $_ for grep {/\.(h|hh)$/} FileLister->new(@ARGV)->all_files;

# Look for #include statements in all source files and delete the matching entry from %index.
my $files = FileLister->new(@ARGV);
while (my $file = $files->next_file) {
  next unless $file =~ /\.(h|hh|c|C|cpp)$/; # look only at C/C++ source code
  if (open FILE, "<", $file) {
    while (<FILE>) {
      # Consider only #includes that do NOT have a directory component
      delete $index{lc $1} if /^\s*#\s*include\s*["<]([^\/]*?)[>"]/;
    }
    close FILE;
  }
}

# Report failures
my @remaining = map {@$_} values %index;
$warning = "" if @remaining > 346; # as of 2010-10-18 there are 346 violations; do not allow more!
print $desc if @remaining;
print "  $_$warning\n" for sort @remaining;

exit(@remaining ? ($warning?128:1) : 0);
