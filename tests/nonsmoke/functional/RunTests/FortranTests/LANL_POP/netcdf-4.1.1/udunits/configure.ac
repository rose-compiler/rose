#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
#
# Copyright 2008, 2009 University Corporation for Atmospheric Research
#
# This file is part of the UDUNITS-2 package.  See the file LICENSE
# in the top-level source-directory of the package for terms and
# conditions.
#

AC_PREREQ(2.59)
AC_INIT(UDUNITS, 2.1.9, support-udunits@unidata.ucar.edu)
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([.])
AC_CONFIG_SRCDIR([lib/converter.c])
AM_INIT_AUTOMAKE([foreign subdir-objects])
AC_CONFIG_HEADERS([config.h expat/expat_config.h])

CFLAGS_COVERAGE=''
LIBS_COVERAGE=''

AC_ARG_ENABLE([coverage],
[AS_HELP_STRING([--enable-coverage],[Turn on code-coverage support])],
[case "${enableval}" in
  yes) CFLAGS_COVERAGE='--coverage'
       LIBS_COVERAGE=-lgcov
       coverage_enabled=true;;
  no) ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-coverage]) ;;
esac])
AC_SUBST(CFLAGS_COVERAGE)
AC_SUBST(LIBS_COVERAGE)

AC_ARG_ENABLE([debug],
[AS_HELP_STRING([--enable-debug],[Turn on debugging support])],
[case "${enableval}" in
  yes)
    CFLAGS="${CFLAGS:+$CFLAGS }-g"
    debug=true ;;
  no)
    CFLAGS="${CFLAGS:+$CFLAGS }-O"
    debug=false ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
esac],
[if test "$coverage_enabled" = true; then
    CFLAGS="${CFLAGS:+$CFLAGS }-g"
    debug=true
else
    debug=false
fi
])
AM_CONDITIONAL([DEBUG], [test x$debug = xtrue])

AM_CONDITIONAL([ENABLE_UDUNITS_1], [true])
AC_ARG_ENABLE([udunits-1],
[AS_HELP_STRING([--disable-udunits-1],[Turn off support for the
UDUNITS-1 API [default=enabled]])],
[case "${enableval}" in
  no)   AM_CONDITIONAL([ENABLE_UDUNITS_1], [false])
	;;
  yes)  ;;
  *) AC_MSG_ERROR([bad value ${enableval} for --enable-udunits-1]) ;;
esac])

# Ensure that compilation is optimized and with assertions disabled by default.
if test x$debug \!= xtrue; then
    CFLAGS="-O${CFLAGS:+ $CFLAGS}"
    CPPFLAGS="-DNDEBUG${CPPFLAGS:+ $CPPFLAGS}"
fi

# Checks for programs.
AC_PROG_CC(xlc cc c89 acc gcc)
AM_PROG_CC_C_O
if test "$ac_cv_prog_cc_${ac_cc}_c_o" = yes; then
    case "$CFLAGS" in
	"-g") ;;
	*) CFLAGS="${CFLAGS:+$CFLAGS }-g";;
    esac
fi
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_YACC
AM_PROG_LEX

# Checks for libraries.
AC_SEARCH_LIBS([dirname], [gen], , 
    AC_MSG_ERROR([cannot find function dirname]))
AC_SEARCH_LIBS([log10], [m], , 
    AC_MSG_ERROR([cannot find function log10]))

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([float.h inttypes.h stddef.h stdlib.h string.h strings.h])

# Check for CUNIT unit-testing package
dnl AC_MSG_NOTICE([Checking for CUNIT unit-testing package.])
dnl AC_CHECK_FILE([/opt/include/CUnit/CUnit.h],
dnl     [CPPFLAGS_CUNIT=-I/opt/include
dnl     AC_SUBST(CPPFLAGS_CUNIT)
dnl     AC_CHECK_FILE([/opt/lib/libcunit.so],
dnl 	[LIBS_CUNIT='-L/opt/lib -lcunit'
dnl 	AC_SUBST(LIBS_CUNIT)],
dnl 	[AC_CHECK_FILE([/opt/lib/libcunit.a],
dnl 	    [LIBS_CUNIT='-L/opt/lib -lcunit'
dnl 	    AC_SUBST(LIBS_CUNIT)])])],
dnl     [AC_CHECK_FILE([/usr/local/include/CUnit/CUnit.h],
dnl 	[CPPFLAGS_CUNIT=-I/usr/local/include
dnl 	AC_SUBST(CPPFLAGS_CUNIT)
dnl 	AC_CHECK_FILE([/usr/local/lib/libcunit.so],
dnl 	    [LIBS_CUNIT='-L/usr/local/lib -lcunit'
dnl 	    AC_SUBST(LIBS_CUNIT)],
dnl 	    [AC_CHECK_FILE([/usr/local/lib/libcunit.a],
dnl 		[LIBS_CUNIT='-L/usr/local/lib -lcunit'
dnl 		AC_SUBST(LIBS_CUNIT)])])])])
dnl if test "$CPPFLAGS_CUNIT" -a "$LIBS_CUNIT"; then
dnl     AC_MSG_NOTICE([CUNIT found.  Enabling unit-tests.])
dnl     AM_CONDITIONAL([HAVE_CUNIT], [true])
dnl else
    AC_MSG_NOTICE([CUNIT not found.  Disabling unit-tests.])
    AM_CONDITIONAL([HAVE_CUNIT], [false])
dnl fi

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T

# Needed for the Expat subpackage:
AC_C_BIGENDIAN([byteorder=4321],[byteorder=1234])
AC_DEFINE_UNQUOTED([BYTEORDER],[$byteorder], [Define to 4321 for big-endian and 1234 for little-endian])
AC_DEFINE([XML_CONTEXT_BYTES], 1024, [Define to specify how much context to retain around the current parse point.])

# Checks for library functions.
AC_CHECK_FUNCS([floor memmove memset modf pow strcasecmp strdup strpbrk])

AC_PROG_LIBTOOL

AC_CONFIG_FILES([Makefile
		 expat/Makefile
                 lib/Makefile
                 lib/xmlFailures/Makefile
                 lib/xmlSuccesses/Makefile
		 prog/Makefile
		 test/Makefile])
AC_OUTPUT
