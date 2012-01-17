#!/usr/bin/perl
# Filter out common errors from autoconf tools.
# Reads standard input; writes to standard output.
use strict;
$|=1;

my($filtering,%filtered);

while (<>) {

  # These are the patterns for warnings we wish to ignore...
  if (
      /^configure.(?:in|ac):\d+: (warning: AC_LIBTOOL_SETUP was called before AC_LIBLTDL_CONVENIENCE)/   ||
      /^configure.(?:in|ac):\d+: (warning: AC_LIBTOOL_SETUP was called before AC_LIBTOOL_DLOPEN)/        ||
      /^configure.(?:in|ac):\d+: (warning: AC_REQUIRE: `AC_PATH_X' was expanded before it was required)/ ||
      /^configure.(?:in|ac):\d+: (warning: AC_CACHE_VAL\(mdl_cv_have_OpenGL, ...\): suspicious presence of an AC_SUBST in the second argument, where no actions should be taken)/ ||
      /^configure.(?:in|ac):\d+: (warning: m4_require: `_LT_SYS_DYNAMIC_LINKER' was expanded before it was required)/
     ) {
    $filtering = 1;
    $filtered{$1}++;

  } elsif ($filtering && /( is expanded from\.\.\.$)|(: the top level$)/) {
    # do not emit
  } else {
    $filtering = 0;
    print;
  }
}

if (keys %filtered) {
  #      --------------------------------------------------------------------------------
  print "+ NOTE: The following known warnings were suppressed from the previous autoconf\n";
  print "commands because we already know about them.  They are due to the way\n";
  print "we support multiple versions of autoconf tools.  Run the autoconf\n";
  print "commands by hand too see the details.\n";
  print "    $_\n" for sort keys %filtered;
}
