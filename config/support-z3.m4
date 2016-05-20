AC_DEFUN([ROSE_SUPPORT_Z3],[

    dnl Parse configure command-line switches for Z3 and/or obtain the value from the cache.
    AC_ARG_WITH([z3],
                [AC_HELP_STRING([[[[--with-z3[=PREFIX]]]]], dnl yes, we really need 4 quotes (autoconf 2.6.1)!
                                [Use the Z3 Satisfiability Modulo Theories Solver. PREFIX is where the
				executable z3 is contained])],
                [ac_cv_use_z3=$withval],
                [ac_cv_use_z3=try])

USE_Z3=0
if test "x$ac_cv_use_z3" != xno; then
	if test "x$ac_cv_use_z3" = xtry -o "x$ac_cv_use_z3" = xyes ; then
		if which z3 > /dev/null 2> /dev/null; then
			Z3="`which z3`"
			Z3BIN="`dirname $Z3`"
			USE_Z3=1
		elif test "x$ac_cv_use_z3" = xyes ; then
			AC_MSG_ERROR([--with-z3 set but z3 command not found in PATH])
		fi
	else
		if test -d "$ac_cv_use_z3"; then
			Z3BIN="$ac_cv_use_z3"
			USE_Z3=1
		else
			AC_MSG_ERROR([Argument to --with-z3 must be a path to bin directory, but argument is not a directory])
		fi
	fi
fi

if test $USE_Z3 = 1; then
	if test ! -x "$Z3BIN/z3" ; then
		AC_MSG_ERROR([z3 could not be found in the z3 bin directory $Z3BIN])
	fi
	AC_MSG_RESULT([$Z3BIN])
fi
    AC_DEFINE_UNQUOTED(ROSE_Z3, ["$ROSE_Z3"], [Absolute name of z3 executable, or the empty string.])
    AC_SUBST(Z3BIN)
    AM_CONDITIONAL(USE_Z3, test "$USE_Z3" = 1)
])
