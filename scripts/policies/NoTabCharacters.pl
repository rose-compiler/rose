#!/usr/bin/perl
my $desc = <<EOF;
ROSE has adopted a policy of using only ASCII SPACE ('\\040') characters for
source code indentation. The use of ASCII HT (a.k.a., TAB, '\\011') characters
for indentation is fraught with dangers that lead to code which is not
indented in a sane manner when viewed by other developers.  Code editors
(XEmacs, vim, eclipse, etc) have varying capabilities for what happens when
a user presses the key labeled \"Tab\":

   XEmacs:  See docs/IDE-Hints/XEmacs
   vim:     [Somebody who uses vim, please add hints to $0]
   Eclipse: [Somebody who uses Eclipse, please add hints to $0]

If your editor is not capable of inserting an appropriate number of SPACE
characters when you press the \"Tab\" key, then you can convert TAB characters
to SPACE characters with the Unix \"expand\" command.

The following C/C++ source files contain TAB characters. They can be fixed by
running \"$0 --fix\".
EOF
BEGIN {push @INC, $1 if $0 =~ /(.*)\//}

use strict;
use FileLister;
use Policies;
use Text::Tabs;

sub usage {
  "usage: $0 [--fix] [FILES_OR_DIRECTORIES...]\n";
}

# Parse command-line switches
my $do_fix;
while (@ARGV>0) {
  if ($ARGV[0] eq '--') {
    shift @ARGV;
    last;
  } elsif ($ARGV[0] eq '--fix') {
    shift @ARGV;
    $do_fix = 1;
  } elsif ($ARGV[0] =~ /^-/) {
    die usage;
  } else {
    last;
  }
}

# If no file or directory arguments are specified, and the CWD is the top of
# the ROSE source tree, then look only at the "src" directory.
@ARGV = "src" if !@ARGV && -d "scripts/policies";

# Expand all tabs in a file. Original file is saved by appending "~" to the
# name.
sub fix {
  my($filename) = @_;
  open FILE, "<", $filename or return "$0: $filename: $!\n";
  my @lines = expand(<FILE>);
  close FILE;
  rename $filename, "$filename~"; # ignore failure
  open FILE, ">", $filename or return "$0: $filename: $!\n";
  print FILE @lines;
  close FILE;
  return undef;
}

# Process each specified file and/or directory, provided the file has a
# name indicative of C/C++ source code.
my $nfail=0;
my $files = FileLister->new(@ARGV);
while (my $filename = $files->next_file) {
  next unless $filename =~ /\.(h|hh|hpp|c|C|cpp|yy|ll?)$/;
  next if $filename =~ /\/EDG\//; # We don't own EDG sources and therefore can't enforce style constraints
  next if is_disabled($filename);

  if (open FILE, "<", $filename) {
    my $has_tabs;
    while (<FILE>) {
      if (/\t/) {
	$has_tabs = 1;
	last;
      }
    }
    close FILE;

    # Change tabs to spaces?
    if ($has_tabs) {
      if ($do_fix) {
	my $error = fix $filename;
	$has_tabs = 0 unless $error;
      }
    }

    # Errors
    if ($has_tabs) {
      print $desc if 1==++$nfail;
      print "    $filename\n";
    }
  }
}

exit($nfail>0 ? 1 : 0);
