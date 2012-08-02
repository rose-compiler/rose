dnl LNP: FIXME: Insert header
dnl Autoconf script for plutorose
dnl Created 2009-03-14 - Justin Holewinski
dnl Fully redesigned 2010-01-11 - Louis-Noel Pouchet

AC_INIT([polyopt], [0.2.0], [polyhedral@cse.ohio-state.edu])
## A common file, which should be included by every source file.
AC_CONFIG_SRCDIR([polyopt/include/polyopt/PolyOpt.hpp])
## Put as most as possible configuration files to an auxialiry
## directory.
AC_CONFIG_AUX_DIR([config])
## Initialize automake. Here, a special tar version that enables
## (very) long filenames.
AM_INIT_AUTOMAKE([1.9 tar-ustar no-define foreign dist-bzip2])
#AM_INIT_AUTOMAKE([1.9 tar-ustar no-define foreign dist-bzip2 -Wall -Werror])


AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL
AC_PROG_INSTALL

## Check for some essential headers.
AC_HEADER_STDC
AC_CHECK_HEADERS([errno.h stddef.h stdlib.h string.h strings.h unistd.h])

## Check for some essential C properties.
AC_C_CONST
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_C_VOLATILE

AC_CHECK_PROGS(DOXYGEN, doxygen, doxygen)
AC_CHECK_PROGS(SVN,svn)
AC_CHECK_PROGS(GIT,git)
AC_CHECK_PROGS(AUTOCONF,autoconf)
AC_CHECK_PROGS(AUTOMAKE,automake)

AC_LANG(C++)


# Make sure Boost is present and useable.
AX_BOOST_BASE([1.35.0], [], [AC_MSG_ERROR([
========================================
|    ERROR                             |
========================================
| Boost 1.35.0 or later is required.   |
========================================
])])


# Make sure Rose is present and useable.
dnl All of this should go into a separate AX_ROSE macro someday.
AC_ARG_WITH([rose], [AS_HELP_STRING([--with-rose], [defines the root directory of a local Rose installation.])], [ROSE_ROOT=$withval], [])
ROSE_CPPFLAGS="-I$ROSE_ROOT/include"
ROSE_LDFLAGS="-L$ROSE_ROOT/lib -lrose"

dnl Check for rose.h
CPPFLAGS_BACKUP=$CPPFLAGS
CPPFLAGS="$CPPFLAGS $ROSE_CPPFLAGS $BOOST_CPPFLAGS"
AC_CHECK_HEADERS([rose.h], [], [AC_MSG_ERROR([
========================================
|    ERROR                             |
========================================
| Rose 0.9.4a is required.             |
========================================
])],
[/* rose.h complains if preceded by certain standard includes */])
CPPFLAGS=$CPPFLAGS_BACKUP
AC_SUBST(ROSE_CPPFLAGS)
AC_SUBST(ROSE_LDFLAGS)



dnl Offer --with-pocc-prefix.
AC_ARG_WITH(pocc-prefix,
	    AC_HELP_STRING([--with-pocc-prefix=DIR],
              	           [DIR Location of PoCC installation package]),
              [pocc_install_prefix=$withval; ])
AC_SUBST(pocc_install_prefix)


#Setup variables for bechmark script.
## LNP: FIXME: clean this.
LOCAL_SOURCE_DIR="$srcdir/"
WDSAVE="`pwd`"
cd "`dirname "$LOCAL_SOURCE_DIR"`"
SOURCE_DIR="`pwd`/`basename "$LOCAL_SOURCE_DIR"`"
cd "$WDSAVE"
AC_SUBST(SOURCE_DIR)

BINARY_DIR="`pwd`"
AC_SUBST(BINARY_DIR)



## Variable substitutions.
AC_SUBST([ac_aux_dir])
AC_SUBST([abs_top_srcdir])

## Configure Makefiles
AC_CONFIG_FILES([
  Makefile
  doc/Makefile
  polyopt/Makefile
  src/Makefile
  benchmarks/Makefile
  benchmarks/run.pl
  tests/Makefile
  tests/sanity/run-sanity-check.sh
  tests/regression/polybenchs/run-regression.pl
  tests/correctness/run-correctness.pl
  ],
  [test -z "$CONFIG_HEADERS" || echo timestamp > polyopt/include/polyopt/stamp-h.in; ])
## Output the result.
AC_OUTPUT
