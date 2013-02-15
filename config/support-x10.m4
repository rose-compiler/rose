# Inclusion of X10 compiler path.
AC_DEFUN([ROSE_SUPPORT_X10],[

    dnl We need to know where to find the X10 compiler.
    AC_ARG_WITH([X10path],
                [AC_HELP_STRING([[[[--with-X10path[=PREFIX]]]]], dnl yes, we really need 4 quotes (autoconf 2.6.1)!
                                [Specify the X10 path. The default is the empty prefix.])], [ac_cv_use_X10path=$withval], [ac_cv_use_X10_path=no])
    AC_CACHE_CHECK([whether to use X10], [ac_cv_use_X10path], [ac_cv_use_X10path=no])

    dnl Find the X10 executable and/or library.
    HAVE_LIBX10=
    if test $ac_cv_use_X10path = yes; then
        AC_ERROR([Invalid option: $ac_cv_use_X10path.  Please specify the path to the X10 compiler.])
    elif test -n "$ac_cv_use_X10path" -a "$ac_cv_use_X10path" != no; then
        ROSE_X10_PREFIX="$ac_cv_use_X10path"
      #  AC_PATH_PROG(ROSE_X10, x10path, [], [$ROSE_X10_PREFIX/bin])
      #  AC_CHECK_FILE(["$ROSE_X10_PREFIX/lib/libX10.a"], [AC_DEFINE(ROSE_HAVE_LIBX10, [], [Defined when the X10 library is present and should be used.]) 
      #  ROSE_HAVE_LIBX10=yes])
    fi

    dnl Sanity check... If the user told us to use X10, then we must find either an executable or the library.
    if test "$ac_cv_use_X10path" != no -a -z "$ROSE_X10path" -a -z "$ROSE_HAVE_LIBX10"; then
        AC_MSG_ERROR([found neither X10 executable nor libX10.a for --with-x10path])
    fi

    dnl Results
    dnl   ROSE_X10         -- defined as the name of the "X10" executable if available
    dnl   ROSE_X10_PREFIX  -- the name of the directory where X10 software is installed if no on search paths
    dnl   ROSE_HAVE_LIBX10 -- defined if the X10 library and include files are available
    if test -n "$ROSE_X10"; then
        AC_DEFINE_UNQUOTED(ROSE_X10, ["$ROSE_X10"], [Absolute name of X10 executable, or the empty string.])
    fi
    AC_SUBST(ROSE_X10)
    AC_SUBST(ROSE_X10_PREFIX)
    AM_CONDITIONAL(ROSE_HAVE_LIBX10, [test -n "$ROSE_HAVE_LIBX10"])
    AM_CONDITIONAL(ROSE_HAVE_X10,    [test -n "$ROSE_X10"])
])
