AC_DEFUN([ROSE_SUPPORT_Z3],[

    dnl Parse configure command-line switches for Z3 and/or obtain the value from the cache.
    AC_ARG_WITH([z3],
                [AC_HELP_STRING([[[[--with-z3[=PREFIX]]]]], dnl yes, we really need 4 quotes (autoconf 2.6.1)!
                                [Use the Z3 Satisfiability Modulo Theories Solver. PREFIX is where the
                                executable z3 is contained])],
                [ac_cv_use_z3=$withval],
                [ac_cv_use_z3=try])

    USE_Z3=0
    if test "$ac_cv_use_z3" != no; then
        if test "$ac_cv_use_z3" = try -o "$ac_cv_use_z3" = yes ; then
            if which z3 > /dev/null 2> /dev/null; then
                Z3="`which z3`"
                Z3BIN="`dirname $Z3`"
                Z3_PREFIX="`dirname $Z3BIN`"
                Z3_INCLUDE_PATH="$Z3_PREFIX/include"
                Z3_LIBRARY_PATH="$Z3_PREFIX/lib"
                AC_DEFINE([HAVE_Z3], [1], [Define to 1 if you have Z3 libs and headers.])
                AC_DEFINE([ROSE_HAVE_Z3], [1], [Define to 1 if you have Z3 libs and headers.])
                USE_Z3=1
            elif test "$ac_cv_use_z3" = yes ; then
                AC_MSG_ERROR([--with-z3 set but z3 not found in PATH])
            fi
        else
            if test -d "$ac_cv_use_z3"; then
                Z3_PREFIX="$ac_cv_use_z3"
                Z3BIN="$Z3_PREFIX/bin"
		Z3="$Z3BIN/z3"
                Z3_INCLUDE_PATH="$Z3_PREFIX/include"
                Z3_LIBRARY_PATH="$Z3_PREFIX/lib"
                AC_DEFINE([HAVE_Z3], [1], [Define to 1 if you have Z3 libs and headers.])
                AC_DEFINE([ROSE_HAVE_Z3], [1], [Define to 1 if you have Z3 libs and headers.])
                USE_Z3=1
            else
                AC_MSG_ERROR([Argument to --with-z3 must be a path to prefix directory, but argument is not a directory])
            fi
        fi
    fi

    if test $USE_Z3 = 1; then
        if test ! -x "$Z3BIN/z3" ; then
            AC_MSG_ERROR([z3 could not be found in the z3 bin directory $Z3BIN])
        fi
        AC_MSG_RESULT([$Z3BIN])
    fi

    AC_DEFINE_UNQUOTED(ROSE_Z3, ["$Z3"], [Absolute name of z3 executable, or the empty string.])
    AC_SUBST(Z3BIN)
    AC_SUBST(Z3_INCLUDE_PATH)
    AC_SUBST(Z3_LIBRARY_PATH)
    AC_SUBST(Z3_PREFIX)
    AM_CONDITIONAL(USE_Z3, test "$USE_Z3" = 1)

    # Correct names should have used the ROSE_ prefix. I don't want to break existing code, so I'll only add the
    # correct names here and not remove the wrong names above. [Robb Matzke, 2017-10-17]
    ROSE_Z3_PREFIX="$Z3_PREFIX"
    AC_SUBST(ROSE_Z3_PREFIX)

    AM_CONDITIONAL(ROSE_HAVE_LIBZ3, [test -n "$Z3_LIBRARY_PATH"])
    AM_CONDITIONAL(ROSE_HAVE_Z3,    [test -n "$Z3BIN"])
])
