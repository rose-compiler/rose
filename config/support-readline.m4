# Tests for presence of the GNU readline library
AC_DEFUN([ROSE_SUPPORT_READLINE],[

    dnl Parse configure command-line switches for readline and/or obtain the value from the cache.
    AC_ARG_WITH([readline],
                [AC_HELP_STRING([[[[--with-readline[=PREFIX]]]]], dnl yes, we really need 4 quotes (autoconf 2.6.1)!
                                [Use the GNU readline library for interactive input.  If not present then editing
                                 of input lines is not possible.])],
                [ac_cv_use_readline=$withval],
                [ac_cv_use_readline=no])
    AC_CACHE_CHECK([whether to use readline], [ac_cv_use_readline], [ac_cv_use_readline=no])

    dnl readline root directory
    if test "$ac_cv_use_readline" = yes; then
        ROSE_READLINE_PREFIX=
    elif test -n "$ac_cv_use_readline" -a "$ac_cv_use_readline" != no; then
        ROSE_READLINE_PREFIX="$ac_cv_use_readline"
    fi

    dnl Is the readline library available?
    if test "$ac_cv_use_readline" != no; then
        AC_CHECK_LIB(readline, readline)
    fi
    
    dnl Results
    dnl   ROSE_READLINE_PREFIX  -- name of the directory where readline is installed, such as "/usr"
    dnl   HAVE_LIBREADLINE      -- defined in rose_config.h if the readline library is available
    dnl   ROSE_HAVE_LIBREADLINE -- defined in rose.h (via rosePublicConfig.h) if the readline library is available
    AC_SUBST(ROSE_READLINE_PREFIX)
    AM_CONDITIONAL(ROSE_HAVE_LIBREADLINE, [test "$ac_cv_lib_readline_readline" = yes ])
])
