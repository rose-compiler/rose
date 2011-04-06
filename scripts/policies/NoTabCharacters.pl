#!/usr/bin/perl
# DO NOT DISABLE without first checking with a ROSE core developer
my $desc = <<EOF;
ROSE has adopted a policy of using only ASCII SPACE ('\\040') characters for
source code indentation. The use of ASCII HT (a.k.a., TAB, '\\011') characters
for indentation is fraught with dangers that lead to code which is not
indented in a sane manner when viewed by other developers.  Code editors
(XEmacs, vim, eclipse, etc) have varying capabilities for what happens when
a user presses the key labeled \"Tab\":

   XEmacs:  (setq indent-tabs-mode nil)          [see Robb]
   vim:     [please add hints to $0]
   Eclipse: In Preferences, select General/Editors/Text Editors and check the 'Insert spaces for tabs' check box.

If your editor is not capable of inserting an appropriate number of SPACE
characters when you press the \"Tab\" key, then you can convert TAB characters
to SPACE characters with the Unix \"expand\" command.

The following C/C++ source files contain TAB characters, which can be fixed
in a variety of ways. Perhaps the easiest is to run this script again with
the "--fix" switch (see "$0 --help" for details).
EOF
BEGIN {push @INC, $1 if $0 =~ /(.*)\//}

=head1 NAME

NoTabCharacters - Verifies that source files do not contain tab characters

=head1 SYNOPSIS

 NoTabCharacters [FILES_OR_DIRECTORIES...]
 NoTabCharacters --fix[=NCOLS] [FILES_OR_DIRECTORIES...]

=head1 DESCRIPTION

This script is part of the policy checking system. It scans various kinds of source code files (based on file name extension),
looking for ASCII HT (horizontal tab) characters and reports (or fixes, with the --fix switch) when it finds such a character.
The normal report is simply a description of the policy followed by the names of files that violate the policy.  An alternative
format, enabled with the B<--verbose> switch, produces compiler-like error messages, one per line that violates the policy.

Source file names and directories (which are scanned recursively) are specified on the command line. If no files or directories
are specified then the current working directory (CWD) is scanned, or the ROSE "src" directory is scanned if the CWD is the top of the ROSE source tree.

If the B<--fix> switch is specified then files are fixed in place by substituting each ASCII HT (horizontal tab) character with
an appropriate number of ASCII SP (space) characters.  The expansion of tabs to spaces assumes a tab stop at every eigth
column, but this can be altered to every Nth column by specifying B<--fix=N>.

=cut

use strict;
use FileLister;
use Policies;
use Text::Tabs;

sub help {
  local $_ = `(pod2man $0 |nroff -man) 2>/dev/null` ||
	     `pod2text $0 2>/dev/null` ||
	     `sed -ne '/^=pod/,/^=cut/p' $0 2>/dev/null`;
  die "$0: see source file for documentation" unless $_;
  if (open LESS, "|less") {
    print LESS $_;
    close LESS;
  } else {
    print $_;
  }
};

sub usage {
  return ("usage: $0 [--verbose] [FILES_OR_DIRECTORIES...]\n" .
	  "usage: $0 --fix[=N] [--verbose] [FILES_OR_DIRECTORIES...]\n")
}

# Parse command-line switches
my($do_fix, $do_verbose);
while (@ARGV>0) {
  if ($ARGV[0] eq '--') {
    shift @ARGV;
    last;
  } elsif ($ARGV[0] =~ /^--fix(=(\d+))?$/) {
    $do_fix = $2 || 8;
    shift @ARGV;
  } elsif ($ARGV[0] eq '--verbose' || $ARGV[0] eq '-v') {
    $do_verbose++;
    shift @ARGV;
  } elsif ($ARGV[0] eq '--help' || $ARGV[0] eq '-h' || $ARGV[0] eq '-?') {
    help;
    exit 0;
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
  $Text::Tabs::tabstop = $do_fix;
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
    my($has_tabs,$linenum);
    while (<FILE>) {
      $linenum++;
      if (/\t/) {
	$has_tabs = 1;
	if ($do_verbose && !$do_fix) {
	  print STDERR "$filename:$linenum: line contains ASCII HT (tab) characters\n";
	} else {
	  last;
	}
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
      $nfail++;
      unless ($do_verbose) {
	print $desc if 1==$nfail;
	print "    $filename\n";
      }
    }
  }
}

exit($nfail>0 ? 1 : 0);
