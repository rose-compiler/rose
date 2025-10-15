#!/usr/bin/perl
# DO NOT DISABLE without first checking with a ROSE core developer
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
use Policies;
my $warning = " (warning)";	# non-empty means issue warnings rather than errors, but see below

# Create an index of all header files.
# Each key is the base name of the header file;
# Each value is an array of header filenames that map to this key.
my %index;
for my $file (FileLister->new(@ARGV)->all_files()) {
    # utility_functionsImpl.C is misnamed -- it's actually a header file.
    next unless $file =~ /\.(h|hh|hpp)$/ || $file =~ /\/utility_functionsImpl.C$/;
    next if $file =~ /\b(tests|projects|src\/AstNodes)\//;
    my($basename) = $file =~ /([^\/]+)$/;
    my($key) = lc $basename;
    $index{$key} = [] unless exists $index{$key};
    push @{$index{$key}}, $file;
}

# Look for #include statements in all source files and delete the matching entry from %index.
my $files = FileLister->new(@ARGV);
while (my $file = $files->next_file) {
  next unless $file =~ /\.(h|hh|hpp|c|cc|C|cpp|[fF]\w*)$/; # look only at C/C++/Fortran source code
  next if $file =~ /\b(tests|projects)\//; # skip test and project directories
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
      } elsif (/\b__TH[I]S_HEADER_IS_USED__\b/) { # The [I] is to make sure we don't match this particular occurrence
	# If a header file really is used but just not included in any source code, then you may indicate so in the
	# header by adding a line that matches this pattern.
	my($basename) = $file = ~ /([^\/]+)$/;
	delete $index{lc $basename};
      }
    }
    close FILE;
  }
}

# Report failures
my @remaining = map {@$_} values %index;

# Max number of permissible unused headers
#my $max_unused_headers = 281; # 2010-11-06
#my $max_unused_headers = 199; # 2014-07-31
my $max_unused_headers = 143; # 2023-03-27

$warning = "" if @remaining > $max_unused_headers;

print $desc if @remaining;
print "  $_$warning\n" for sort @remaining;

# DQ (11/20/2010): Added the total file could so we can measure progress/failure.
#$size = @remaining;
#print "Remaining file count: $size.\n";
print "Number of remaining unused header files: " . @remaining . " (max allowed: $max_unused_headers).\n";

exit(@remaining ? ($warning?128:1) : 0);
