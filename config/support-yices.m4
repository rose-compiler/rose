# Inclusion of test for YICES SMT Solver Package and it's location.
AC_DEFUN([ROSE_SUPPORT_YICES],[

    dnl Parse configure command-line switches for Yices and/or obtain the value from the cache.
    AC_ARG_WITH(
        [yices],
        AS_HELP_STRING(
            [--with-yices=PREFIX],
            [Use the Yices SMT solver available as a non-free download from https://yices.csl.sri.com (consider
             using the Z3 solver instead, which is free and better supported). The PREFIX, if specified,
             should be the prefix used to install Yices, such as "/usr/local". The default is the empty prefix,
             in which case the headers and library must be installed in a place where they will be found. Saying
             "no" for the prefix is the same as saying "--without-yices".]),
            [ac_cv_use_yices=$withval],
            [ac_cv_use_yices=no])
    AC_CACHE_CHECK([whether to use yices], [ac_cv_use_yices], [ac_cv_use_yices=no])


    dnl Find the yices executable and/or library.
    HAVE_LIBYICES=
    if test $ac_cv_use_yices = yes; then
        ROSE_YICES_PREFIX=
        AC_PATH_PROG(ROSE_YICES, yices)
        AC_CHECK_LIB(yices, yicesl_version,
                     [AC_DEFINE(ROSE_HAVE_LIBYICES, [], [Defined when the Yices SMT-Solver library is present and should be used.])
                      ROSE_HAVE_LIBYICES=yes])
    elif test -n "$ac_cv_use_yices" -a "$ac_cv_use_yices" != no; then
        ROSE_YICES_PREFIX="$ac_cv_use_yices"
        AC_PATH_PROG(ROSE_YICES, yices, [], [$ROSE_YICES_PREFIX/bin])
        AC_CHECK_FILE(["$ROSE_YICES_PREFIX/lib/libyices.a"],
                      [AC_DEFINE(ROSE_HAVE_LIBYICES, [], [Defined when the Yices SMT-Solver library is present and should be used.])
                       ROSE_HAVE_LIBYICES=yes])
    fi

    dnl Sanity check... If the user told us to use yices, then we must find either an executable or the library.
    if test "$ac_cv_use_yices" != no -a -z "$ROSE_YICES" -a -z "$ROSE_HAVE_LIBYICES"; then
        AC_MSG_ERROR([found neither yices executable nor libyices.a for --with-yices])
    fi

    dnl Results
    dnl   ROSE_YICES         -- defined as the name of the "yices" executable if available
    dnl   ROSE_YICES_PREFIX  -- the name of the directory where Yices software is installed if no on search paths
    dnl   ROSE_HAVE_LIBYICES -- defined if the Yices library and include files are available
    if test -n "$ROSE_YICES"; then
        AC_DEFINE_UNQUOTED(ROSE_YICES, ["$ROSE_YICES"], [Absolute name of yices executable, or the empty string.])
    fi
    AC_SUBST(ROSE_YICES)
    AC_SUBST(ROSE_YICES_PREFIX)
    AM_CONDITIONAL(ROSE_HAVE_LIBYICES, [test -n "$ROSE_HAVE_LIBYICES"])
    AM_CONDITIONAL(ROSE_HAVE_YICES,    [test -n "$ROSE_YICES"])
])
