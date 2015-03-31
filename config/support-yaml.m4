# Tests for libyaml-cpp [https://code.google.com/p/yaml-cpp]
AC_DEFUN([ROSE_SUPPORT_YAML],[

    dnl Parse configure command-line switches for YAML and/or obtain the value from the cache.
    AC_ARG_WITH([yaml],
                [AC_HELP_STRING([[[[--with-yaml[=PREFIX]]]]], dnl yes, we really need 4 quotes (autoconf 2.6.1)!
		                [Use libyaml-cpp available from https://code.google.com/p/yaml-cpp for free.
				 The PREFIX, if specified, should be the prefix used to install libyaml-cpp,
				 such as "/usr/local".  The default is the empty prefix, in which case the
				 headers and library must be installed in a place where they will be found.
				 Saying "no" for the prefix is the same as saying "--without-yaml".])],
                [ac_cv_use_yaml=$withval],
		[ac_cv_use_yaml=no])
    AC_CACHE_CHECK([whether to use yaml], [ac_cv_use_yaml], [ac_cv_use_yaml=no])

    dnl Find the yaml library
    ROSE_HAVE_LIBYAML=
    if test $ac_cv_use_yaml = yes; then
        ROSE_YAML_PREFIX=
	AC_CHECK_LIB(yaml-cpp, YAML::LoadFile,
		     [AC_DEFINE(ROSE_HAVE_LIBYAML, [], [Defined when libyaml-cpp is available.])
		     ROSE_HAVE_LIBYAML=yes])
    elif test -n "$ac_cv_use_yaml" -a "$ac_cv_use_yaml" != no; then
        ROSE_YAML_PREFIX="$ac_cv_use_yaml"
	AC_CHECK_FILE(["$ROSE_YAML_PREFIX/lib/libyaml-cpp.a"],
		      [AC_DEFINE(ROSE_HAVE_LIBYAML, [], [Defined when libyaml-cpp is available.])
		       ROSE_HAVE_LIBYAML=yes])
    fi

    dnl Sanity check: if the user told us to use libyaml then we must find the library
    if test "$ac_cv_use_yaml" != no -a -z "$ROSE_HAVE_LIBYAML"; then
        AC_MSG_ERROR([did not find yaml-cpp library but --with-yaml was specified])
    fi

    dnl Results
    dnl   ROSE_LIBYAML_PREFIX -- name of the directory where yaml-cpp is installed
    dnl   ROSE_HAVE_LIBYAML   -- defined if the yaml-cpp library is available
    AC_SUBST(ROSE_YAML_PREFIX)
    AM_CONDITIONAL(ROSE_HAVE_LIBYAML, [test -n "$ROSE_HAVE_LIBYAML"])
])
