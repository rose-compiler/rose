#!/usr/bin/perl
# Filter out common errors from autoconf tools.
# Reads standard input; writes to standard output.
use strict;
$|=1;

my($filtering) = 0;
while (<>) {
  if (/^configure.in:\d+: warning: AC_LIBTOOL_SETUP was called before AC_LIBLTDL_CONVENIENCE/ ||
      /^configure.in:\d+: warning: AC_LIBTOOL_SETUP was called before AC_LIBTOOL_DLOPEN/) {
    $filtering = 1;
  } elsif ($filtering && /( is expanded from\.\.\.$)|(: the top level$)/) {
    # do not emit
  } else {
    $filtering = 0;
    print;
  }
}
