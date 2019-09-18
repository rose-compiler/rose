dnl Tests for magic.h and libmagic
AC_DEFUN([ROSE_SUPPORT_LIBMAGIC],[

    dnl Parse configure command-line switches for libmagic and/or obtain the value from the cache.
    AC_ARG_WITH(
        [magic],
        AS_HELP_STRING(
            [--with-magic=PREFIX],
            [Use libmagic available from http://sourceforge.net/projects/libmagic for free.
             The PREFIX, if specified, should be the prefix used to install libmagic, such as "/usr/local".
             The default is the empty prefix, in which case the headers and library must be installed in a
             place where they will be found. Saying "no" for the prefix is the same as saying
             "--without-magic".]),
            [ac_cv_use_magic=$withval],
            [ac_cv_use_magic=no])
    AC_CACHE_CHECK([whether to use magic], [ac_cv_use_magic], [ac_cv_use_magic=no])

    dnl Find the magic library
    ROSE_HAVE_LIBMAGIC=
    if test $ac_cv_use_magic = yes; then
        ROSE_LIBMAGIC_PREFIX=
        AC_CHECK_LIB(magic, magic_open,
                     [AC_DEFINE(ROSE_HAVE_LIBMAGIC, [], [Defined when libmagic is available.])
                     ROSE_HAVE_LIBMAGIC=yes])
    elif test -n "$ac_cv_use_magic" -a "$ac_cv_use_magic" != no; then
        ROSE_LIBMAGIC_PREFIX="$ac_cv_use_magic"
        old_LDFLAGS="$LDFLAGS"
        LDFLAGS="$LDFLAGS -L$ROSE_LIBMAGIC_PREFIX/lib"
        AC_CHECK_LIB(magic, magic_open,
                     [AC_DEFINE(ROSE_HAVE_LIBMAGIC, [], [Defined when libmagic is available.])
                      ROSE_HAVE_LIBMAGIC=yes])
        LDFLAGS="$old_LDFLAGS"
    fi

    dnl Sanity check: if the user told us to use libmagic then we must find the library
    if test "$ac_cv_use_magic" != no -a -z "$ROSE_HAVE_LIBMAGIC"; then
        AC_MSG_ERROR([did not find libmagic library but --with-magic was specified])
    fi

    dnl Results
    dnl   ROSE_LIBMAGIC_PREFIX -- name of the directory where libmagic is installed
    dnl   ROSE_HAVE_LIBMAGIC   -- defined if the libmagic library is available
    AC_SUBST(ROSE_LIBMAGIC_PREFIX)
    AM_CONDITIONAL(ROSE_HAVE_LIBMAGIC, [test -n "$ROSE_HAVE_LIBMAGIC"])
])
