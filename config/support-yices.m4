# Inclusion of test for YICES SMT Solver Package and it's location.
AC_DEFUN([ROSE_SUPPORT_YICES],[

    dnl Parse configure command-line switches for Yices and/or obtain the value from the cache.
    AC_ARG_WITH([yices],
                [AC_HELP_STRING([[[[--with-yices[=PREFIX]]]]], dnl yes, we really need 4 quotes (autoconf 2.6.1)!
                                [Use the Yices Satisfiability Modulo Theories (SMT) solver. The PREFIX, if specified,
                                 should be the prefix used to install Yices, such as "/usr/local". ROSE searches for
                                 an executable named "/bin/yices" under the specified prefix.  The default is the empty
                                 prefix.])],
                [ac_cv_use_yices=$withval],
                [ac_cv_use_yices=no])
    AC_CACHE_CHECK([whether to use yices], [ac_cv_use_yices], [ac_cv_use_yices=no])


    dnl Find the yices executable and/or library.
    HAVE_LIBYICES=
    if test $ac_cv_use_yices = yes; then
        YICES_PREFIX=
        AC_PATH_PROG(YICES, yices)
        AC_CHECK_LIB(yices, yicesl_version,
                     [AC_DEFINE(HAVE_LIBYICES, [], [Defined when the Yices SMT-Solver library is present and should be used.])
                      HAVE_LIBYICES=yes])
    elif test -n "$ac_cv_use_yices" -a "$ac_cv_use_yices" != no; then
        YICES_PREFIX="$ac_cv_use_yices"
        AC_PATH_PROG(YICES, yices, [], [$YICES_PREFIX/bin])
        AC_CHECK_FILE(["$YICES_PREFIX/lib/libyices.a"],
                      [AC_DEFINE(HAVE_LIBYICES, [], [Defined when the Yices SMT-Solver library is present and should be used.])
                       HAVE_LIBYICES=yes])
    fi

    dnl Sanity check... If the user told us to use yices, then we must find either an executable or the library.
    if test "$ac_cv_use_yices" != no -a -z "$YICES" -a -z "$HAVE_LIBYICES"; then
        AC_MSG_ERROR([found neither yices executable nor libyices.a for --with-yices])
    fi

    dnl Results
    dnl   YICES         -- defined as the name of the "yices" executable if available
    dnl   YICES_PREFIX  -- the name of the directory where Yices software is installed if no on search paths
    dnl   HAVE_LIBYICES -- defined if the Yices library and include files are available
    if test -n "$YICES"; then
        AC_DEFINE_UNQUOTED(YICES, ["$YICES"], [Absolute name of yices executable, or the empty string.])
    fi
    AC_SUBST(YICES)
    AC_SUBST(YICES_PREFIX)
    AM_CONDITIONAL(HAVE_LIBYICES, [test -n "$HAVE_LIBYICES"])
    AM_CONDITIONAL(HAVE_YICES,    [test -n "$YICES"])
])
