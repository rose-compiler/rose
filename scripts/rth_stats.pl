#!/usr/bin/perl
#
# Traverses directories to list status of each test.
# Does so by looking at *.passed and *.failed files.

use strict;

BEGIN {push @INC, "$1/policies" if $0 =~ /(.*)\//}
use FileLister;

my %status;
my $files = FileLister->new(@ARGV);
$files->{build} = 1; #include build tree
while (my $file = $files->next_file) {
  my($target,$ext) = $file =~ /([^\/]+)(?:\.([^\.\/]+))$/;
  if ($ext eq 'passed') {
    $status{$target} = -l $file ? "ignored" : "passed";
  } elsif ($ext eq 'failed') {
    $status{$target} ||= 'failed';
  }
}

# Organize tests into sorted tables based on status
my %tables = (passed=>[], failed=>[], ignored=>[]);
push @{$tables{$status{$_}}}, $_ for keys %status;
$tables{$_} = [sort @{$tables{$_}}] for keys %tables;

# Output, in column-major order like the "ls" command.
sub show_table {
  my($mesg,$aref) = @_;
  my $width = 130; # number of screen columns
  my $indent = "    ";
  my $sep = "  ";
  return unless @$aref;
  print $mesg, "\n";
  my $maxwidth = (sort {$a<=>$b} map {length} @$aref)[-1];
  my $ncols = int(($width-length $indent)/($maxwidth+ length $sep)); $ncols=1 unless $ncols;
  my $nrows = int((@$aref+$ncols-1) / $ncols);

  for (my $row=0; $row<$nrows; $row++) {
    print $indent;
    for (my $col=0; $col<$ncols; $col++) {
      my $idx = $col*$nrows + $row;
      last unless $idx <@$aref;
      print $aref->[$idx];
      print $sep, " " x ($maxwidth-length $aref->[$idx]) unless $col+1==$ncols;
    }
    print "\n";
  }
}

show_table "The following tests passed:", $tables{passed};
show_table "The following tests failed:", $tables{failed};
show_table "The following failures were ignored:", $tables{ignored};
