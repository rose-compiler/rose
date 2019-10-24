AC_DEFUN([ROSE_SUPPORT_Z3],[

    dnl Parse configure command-line switches for Z3 and/or obtain the value from the cache.
    AC_ARG_WITH([z3],
        AS_HELP_STRING([--with-z3=PREFIX],
            [Use the Z3 SMT solver available from https://github.com/Z3Prover/z3.
             The PREFIX, if specified, should be the prefix used to install Z3, such as "/usr/local".
             The default is the empty prefix, in which case the headers and library must be installed
             in a place where they will be found. Saying "no" for the prefix is the same as saying
             "--without-z3".]),
        [ac_cv_use_z3=$withval],
        [ac_cv_use_z3=no])

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
                AC_MSG_ERROR([argument to --with-z3 must be a path to prefix directory, but argument is not a directory])
            fi
        fi
    fi

    if test $USE_Z3 = 1; then
        # Try to figure out a version number because the blacklisting needs it.
        if test -r "$Z3_INCLUDE_PATH/z3_version.h"; then
            Z3_VERSION=`sed -n 's/#define Z3_FULL_VERSION.*"Z3 \(.*\)"/\1/p' "$Z3_INCLUDE_PATH/z3_version.h"`
        elif test -r "$Z3_LIBRARY_PATH/cmake/z3/Z3Config.cmake"; then
            Z3_VERSION=`sed -n 's/set(Z3_VERSION_\(MAJOR\|MINOR\|PATCH\|TWEAK\) \(@<:@0-9@:>@*\).*/\2/p' "$Z3_LIBRARY_PATH/cmake/z3/Z3Config.cmake" |tr '\n' . |sed 's/\.$//'`
        else
            Z3_VERSION=unknown
        fi

        # z3_version.h was added in 4.8.1 but not included by z3.h. Therefore ROSE needs to include it explicitly
        # but only if the file exists.
        AC_CHECK_HEADER($Z3_INCLUDE_PATH/z3_version.h,
                        [AC_DEFINE(ROSE_HAVE_Z3_VERSION_H, 1, [Define if z3_version.h exists])])

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
