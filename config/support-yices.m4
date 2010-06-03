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


    dnl Find the yices executable and/or library. These variables are AC_DEFINE'd
    dnl   YICES_PREFIX         -- prefix specified (or found) from --with-yices switch
    dnl   YICES                -- the full path name of the "yices" executable
    dnl   HAVE_LIBYICES        -- defined as "1" if libyices.a exists
    dnl
    YICES_PREFIX=
    YICES=
    HAVE_LIBYICES=
    if test $ac_cv_use_yices = yes; then
        YICES_PREFIX=
	AC_PATH_PROG(YICES, yices)
	AC_CHECK_LIB(yices, yicesl_version)
    elif test -n "$ac_cv_use_yices"; then
	YICES_PREFIX="$ac_cv_use_yices"
	AC_PATH_PROG(YICES, yices, [], [$YICES_PREFIX/bin])
	AC_MSG_CHECKING([for libyices.a])
	if test -f "$YICES_PREFIX/lib/libyices.a"; then
	    AC_MSG_RESULT([$YICES_PREFIX/lib/libyices.a])
	    HAVE_LIBYICES=1
        else
	    AC_MSG_RESULT(no)
        fi
    fi

    dnl Sanity check... If the user told us to use yices, then we must find either an executable or the library.
    if test "x$YICES" = x -a "x$HAVE_LIBYICES" = x; then
	AC_MSG_ERROR([found neither yices executable nor libyices.a for --with-yices])
    fi

    dnl C preprocessor defines
    AC_DEFINE_UNQUOTED(YICES,         [$YICES],         [Full path name of the yices executable if found.])
    AC_DEFINE_UNQUOTED(HAVE_LIBYICES, [$HAVE_LIBYICES], [Defined if libyices.a exists.])

    dnl Makefile substitutions
    AC_SUBST(YICES)
    AC_SUBST(YICES_PREFIX)

    dnl Define automake conditionals
    AM_CONDITIONAL(HAVE_LIBYICES, [test -n "$HAVE_LIBYICES"])
    AM_CONDITIONAL(HAVE_YICES,    [test -n "$YICES"])
])
