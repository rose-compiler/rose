#!/usr/bin/perl
my $desc = <<EOF;
Some developers find it annoying when source files contain exceedingly
long lines because either the lines wrap around and make indentation less
clear, or lines are not fully visible.  On the other hand, ROSE's long
names and C++'s verbosity conspires against having 80-column code. A
compromize is that source files should not contain lines more than 132
characters.  This allows two side-by-side views of source code on a 1600x1200
display at a reasonable font size. You may rerun this script as
"@{[$0=~/([^\/]+)$/]} --verbose" to see a list of long lines.
The following files contain lines that exceed this limit:
EOF
BEGIN {push @INC, $1 if $0 =~ /(.*)\//}

use strict;
use FileLister;
use Text::Tabs;

my $warning = " (warning)";	# if non-empty and policy violated, exit with a warning only
my $max_line_length = 132;	# change desc above also if you change this constant

my $verbose;
while ($ARGV[0] =~ /^-/) {
  if ($ARGV[0] eq "--") {
    shift @ARGV;
    last;
  } elsif ($ARGV[0] eq "--verbose") {
    $verbose = 1;
    shift @ARGV;
  } else {
    die "$0: unknown switch: $ARGV[0]\n";
  }
}

my $nfail=0;
my $files = FileLister->new(@ARGV);
while (my $filename = $files->next_file) {
  next unless $filename =~ /\.(h|hh|hpp|c|C|cpp)$/;  # look only at C/C++ source code for now (we can add more later)


  if (-T $filename && open FILE, "<", $filename) {
    my($nlong,$nlines);
    while (<FILE>) {
      if (0==$nlines++) {
	last if /^<\?xml /;	# skip XML files since nobody looks at these anyway
      }
      if (length(expand($_)) > $max_line_length) {
	$nlong++;
	print "$filename:$nlines: line length exceeds $max_line_length columns\n" if $verbose;
      }
    }
    close FILE;
    if ($nlong) {
      $nfail++;
      if (!$verbose) {
	print $desc if 1==$nfail;
	printf "  %6d (%4d%%) lines in %1s%1s\n", $nlong, 100*$nlong/$nlines, $filename, $warning;
      }
    }
  }
}

exit($nfail>0 ? ($warning ? 128 : 1) : 0);

