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
push @{$index{lc((/([^\/]+)$/)[0])}||=[]}, $_ for grep {/\.(h|hh|hpp)$/} FileLister->new(@ARGV)->all_files;

# Look for #include statements in all source files and delete the matching entry from %index.
my $files = FileLister->new(@ARGV);
while (my $file = $files->next_file) {
  next unless $file =~ /\.(h|hh|hpp|c|C|cpp|[fF]\w*)$/; # look only at C/C++/Fortran source code
  if (open FILE, "<", $file) {
    while (<FILE>) {
      my($path,$name);
      if ((($path,$name) = /^\s*#\s*include\s*["<](.*?)([^\/]*?)[>"]/) || # C/C++
	  (($path,$name) = /^\s*include\s*["'](.*?)([^\/]*?)['"]/)) {     # Fortran

	next unless exists $index{lc $name};

	if ($path eq "") {
	  delete $index{lc $name}; # just an optimized version of what would happen for the else case.
	} else {
	  # Remove header files from the index if the header file base name matches (which it will since
	  # that's how the index is organized) and any directories specified in the #include directive
	  # also match.
	  my @non_matching_headers;
	  my @include_parts = reverse split "/", lc($path.$name);
	  for my $header (@{$index{lc $name}}) {
	    my @header_parts = reverse split "/", lc $header;
	    my $does_match = 1;
	    for (my $i=0; $does_match && $i<@include_parts && $i<@header_parts; $i++) {
	      $does_match = $include_parts[$i] eq $header_parts[$i];
	    }
	    push @non_matching_headers, $header unless $does_match;
	    #printf STDERR "%-32s %2s %-32s\n", $path.$name, ($does_match?"==":"!="), $header;
	  }
	  if (@non_matching_headers) {
	    $index{lc $name} = \@non_matching_headers;
	  } else {
	    delete $index{lc $name};
	  }
	}
      }
    }
    close FILE;
  }
}

# Report failures
my @remaining = map {@$_} values %index;

# Lowered the number of unused headers by removing unused headers from: ./tests/CompileTests/OvertureCode
# $warning = "" if @remaining > 281; # as of 2010-11-06 there are 281 violations; do not allow more!
$warning = "" if @remaining > 195; # as of 2010-11-06 there are 281 violations; do not allow more!

print $desc if @remaining;
print "  $_$warning\n" for sort @remaining;

# DQ (11/20/2010): Added the total file could so we can measure progress/failure.
#$size = @remaining;
#print "Remaining file count: $size.\n";
print "Number of remaining unused header files: " . @remaining . ".\n";

exit(@remaining ? ($warning?128:1) : 0);
