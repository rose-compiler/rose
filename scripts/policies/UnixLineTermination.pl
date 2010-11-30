#!/usr/bin/perl
my $desc = <<EOF;
ROSE developers predominantly use Unix-like systems for development. These
systems use the ASCII LF (line feed, '\\n') character for line termination.
Some other operating systems insert a CR (carriage return, '\\r') in front of
the LF.  Insertion of the extra CR causes problems for revision management
software and some Unix-based text editors which developers then have to work
around and/or correct.  To convert a file from any mixture of CR+LF or
LF-only line endings to LF-only line endings, use:
  * the unix command "dos2unix"
  * vim 7.0 and later commands ':update', ':setlocal ff=unix'.
  * in XEmacs use the "M-x set-buffer-line-coding-system RET" and
    enter "undecided-unix".
The following source files have CR+LF line termination:
EOF
BEGIN {push @INC, $1 if $0 =~ /(.*)\//}

use strict;
use FileLister;

my $nfail=0;
my $files = FileLister->new();
while (my $filename = $files->next_file) {
  next if $filename =~ /\.(pdf|rtf|vimsession)$/;
  if (-T $filename && open FILE, "<", $filename) {
    my @content = <FILE>;
    close FILE;
    my $n_lines = @content;
    my $n_crlf = grep {/\r\n/} @content;
    if ($n_crlf) {
      print $desc unless $nfail++;
      printf "  %6d (%4d%%) lines in %1s\n", $n_crlf, 100*($n_crlf/$n_lines), $filename;
    }
  }
}
exit($nfail>0 ? 1 : 0);
