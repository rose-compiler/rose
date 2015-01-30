#!/usr/bin/perl
# DO NOT DISABLE without first checking with a ROSE core developer.


my $desc = "ROSE uses a scripts/stringify.pl perl script to scan all of ROSE library
source code and generate a strings.h file that has functions that convert enum
constants to strings. However, in order for this to not have a race condition,
the cmake file needs to list \"generate_stringify\" as a dependency. The
following source files appear to use stringify but apparently lack the
dependency on generate_stringify:\n";

BEGIN {push @INC, $1 if $0 =~ /(.*)\//}
use strict;
use FileLister;
use Policies;

my $policy_name = (split "/", $0)[-1];

# If no files or directories are specified and the CWD is the top of the ROSE source tree, then check
# only the "src" directory.
@ARGV = "src" if !@ARGV && -d "scripts/policies";

my @usingFiles; # files that use "stringify.h"

# Find the files that use stringify.h
my $files = FileLister->new(@ARGV);
while (my $filename = $files->next_file) {
    if ($filename =~ /(\.C|cpp)$/ && !is_disabled($filename) && open FILE, "<", $filename) {
	my $nlines = 0;
	while (<FILE>) {
	    if (/^\s*#\s*include\s*["<]stringify\.h[>"]/ && !/\bPOLICY_OK\b/) {
		push @usingFiles, $filename;
		last;
	    } elsif (++$nlines > 100) {
		last; # for speed, we only check the top of each file
	    }
	}
	close FILE;
    }
}

# For each using file, find its CMakeLists.txt file. If we can't find the cmakelist file then
# just ignore it.
my %cmakefiles; # hash indexed by cmake file whose value is an array of using file names
for my $filename (@usingFiles) {
    my @components = split "/", $filename;
    my $basename = pop @components;
    while (@components) {
	my $cmakefile = join "/", @components, "CMakeLists.txt";
	if (open FILE, "<", $cmakefile) {
	    my $cmake_mentions_source = 0;
	    while (<FILE>) {
		s/#.*//; # remove comments
		if (/$basename/) {
		    $cmake_mentions_source = 1;
		    last;
		}
	    }
	    if ($cmake_mentions_source) {
		$cmakefiles{$cmakefile} ||= [];
		push @{$cmakefiles{$cmakefile}}, $filename;
		last;
	    }
	}
	pop @components;
    }
    if (!@components){
	print STDERR "$policy_name: warning: cannot find CMakeList.txt for $filename\n";
    }
}

# Each CMakeLists.txt must have an add_dependencies(.... generate_stringify). For simplicity,
# we'll only check for the word "generate_stringify".
my $nfail = 0;
for my $cmakefile (sort keys %cmakefiles) {
    if (open FILE, "<", $cmakefile) {
	my $has_generate_stringify;
	while (<FILE>) {
	    s/#.*//; # remove comments
	    if (/\bgenerate_stringify\b/) {
		$has_generate_stringify = 1;
		last;
	    }
	}
	if (!$has_generate_stringify) {
	    if (0 == $nfail++) {
		print $desc;
	    }
	    print "  $cmakefile: missing generate_stringify dependency for\n";
	    for my $filename (@{$cmakefiles{$cmakefile}}) {
		print "    $filename\n";
	    }
	}
    }
}
