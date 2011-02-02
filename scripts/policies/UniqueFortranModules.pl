#!/usr/bin/perl
my $desc = <<EOF;
Since FORTRAN tests are run in parallel, test cases that declare modules
must use unique modules names.  The following FORTRAN source files declare
conflicting modules:
EOF
BEGIN {push @INC, $1 if $0 =~ /(.*)\//}

use strict;
use FileLister;

# Process each specified file and/or directory, provided the file has a
# name indicative of FORTRAN source code.
my %modules; # list of source files declaring each module
my $files = FileLister->new(@ARGV);
while (my $filename = $files->next_file) {
  next unless $filename =~ /\.(f..)$/;

  if (open FILE, "<", $filename) {
    my $line_num = 0;
    while (<FILE>) {
      $line_num++;
      if (/^\s*module\s+([_a-zA-Z]\w*)/i && lc($1) ne "procedure") {
	my $module = lc $1;
	$modules{$module} ||= [];
	push @{$modules{$module}}, "$filename:$line_num";
      }
    }
  }
  close FILE;
}

# Show results sorted by module name
my $nfail = 0;
for my $module (sort keys %modules) {
  next unless @{$modules{$module}} > 1;
  print $desc if 0 == $nfail++;
  print "    module \"$module\" declared at:\n";
  print "        $_\n" for @{$modules{$module}};
}

# 0=success; 128=warning; 1==error
exit($nfail>0 ? 128 : 0);
