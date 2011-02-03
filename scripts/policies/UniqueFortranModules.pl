#!/usr/bin/perl

# NOTE: Unlike most other policy checkers, this one is not recursive by
#       default. If you want recursive behavior then invoke it with the
#       "--recursive" switch.

my $desc = <<EOF;
Since FORTRAN tests are run in parallel, test cases that declare modules
must use module names which are unique within the test's directory. The
following module declarations are in conflict:
EOF
BEGIN {push @INC, $1 if $0 =~ /(.*)\//}

use strict;
use FileLister;

# If invoked with "--terse" then produce output that looks like
# compiler error messages.  This is useful for IDEs that understand
# such messages.
my $terse = 0;
if (grep {$_ eq "--terse"} @ARGV) {
  @ARGV = grep {$_ ne "--terse"} @ARGV;
  $terse = 1;
}

# Process each specified file and/or directory, provided the file has a
# name indicative of FORTRAN source code.
my %decls; # list of source files declaring each module
my %uses;  # list of sources using each module
my $files = FileLister->new("--recursive=0", @ARGV);
while (my $filename = $files->next_file) {
  next unless $filename =~ /\.(f..)$/;
  my($dir) = $filename =~ /(.*)\//;

  if (open FILE, "<", $filename) {
    my $line_num = 0;
    while (<FILE>) {
      $line_num++;
      if (/^\s*module\s+([_a-zA-Z]\w*)/i && lc($1) ne "procedure") {
	my $key = lc "$1\000$dir";
	$decls{$key} ||= [];
	push @{$decls{$key}}, "$filename:$line_num";
      } elsif (/^\s*use\s+([_a-zA-Z]\w*)/i) {
	my $key = lc "$1\000$dir";
	$uses{$key} ||= [];
	push @{$uses{$key}}, "$filename:$line_num";
      }
    }
  }
  close FILE;
}

# Show results sorted by module name
my $nfail = 0;
for my $key (sort keys %decls) {
  my($module,$dir) = split "\000", $key;
  next unless @{$decls{$key}} > 1;
  print $desc if 0 == $nfail++ && !$terse;

  if ($terse) {
    print "$_: error: duplicate module declaration\n" for @{$decls{$key}};
    print "$_: warning: ambiguous module use\n" for @{$uses{$key}};
  } else {
    print "  module \"$module\":\n";
    print "    declarations:\n";
    print "      $_\n" for @{$decls{$key}};
    if ($uses{$key}) {
      print "    uses:\n";
      print "      $_\n" for @{$uses{$key}};
    }
  }
}

exit($nfail>0 ? 1 : 0);
