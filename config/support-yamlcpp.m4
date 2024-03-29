# Tests for libyaml-cpp [https://code.google.com/p/yaml-cpp]
AC_DEFUN([ROSE_SUPPORT_YAMLCPP],[

    dnl Parse configure command-line switches for YAML and/or obtain the value from the cache.
    AC_ARG_WITH(
        [yaml],
        AS_HELP_STRING(
            [--with-yaml=PREFIX],
            [Use libyaml-cpp available from https://code.google.com/p/yaml-cpp for free.
             The PREFIX, if specified, should be the prefix used to install libyaml-cpp,
             such as "/usr/local".  The default is the empty prefix, in which case the
             headers and library must be installed in a place where they will be found.
             Saying "no" for the prefix is the same as saying "--without-yaml".]),
            [ac_cv_use_yaml=$withval],
            [ac_cv_use_yaml=no])
    AC_CACHE_CHECK([whether to use yaml], [ac_cv_use_yaml], [ac_cv_use_yaml=no])

    dnl YAML-CPP root directory
    if test "$ac_cv_use_yaml" = yes; then
        ROSE_YAMLCPP_PREFIX=
    elif test -n "$ac_cv_use_yaml" -a "$ac_cv_use_yaml" != no; then
        ROSE_YAMLCPP_PREFIX="$ac_cv_use_yaml"
    fi

    dnl Is the YAML-CPP library available?
    ROSE_HAVE_YAMLCPP=
    if test "$ac_cv_use_yaml" != "no"; then
        # Save variables
        AC_LANG_PUSH(C++)
        saved_LDFLAGS="$LDFLAGS"
        saved_LIBS="$LIBS"
        saved_CPPFLAGS="$CPPFLAGS"

        # Set include and library search paths
        CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
        LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"
        if test "$ROSE_YAMLCPP_PREFIX" != ""; then
            LDFLAGS="$LDFLAGS -L$ROSE_YAMLCPP_PREFIX/lib"
            CPPFLAGS="$CPPFLAGS -I$ROSE_YAMLCPP_PREFIX/include"
        fi
        LIBS="$LIBS -lyaml-cpp"

        # Look for the YAML-CPP library
        AC_LINK_IFELSE(
            [AC_LANG_PROGRAM([#include <yaml-cpp/yaml.h>],
                             [YAML::Node configFile = YAML::LoadFile(std::string());])],
            [ROSE_HAVE_YAMLCPP=yes],
            [AC_MSG_ERROR([did not find yaml-cpp library but --with-yaml was specified])])

        # Restore variables
        LDFLAGS="$saved_LDFLAGS"
        LIBS="$saved_LIBS_"
        CPPFLAGS="$saved_CPPFLAGS"
        AC_LANG_POP(C++)
    fi

    dnl Results
    dnl   ROSE_YAMLCPP_PREFIX -- name of the directory where yaml-cpp is installed
    dnl   ROSE_HAVE_YAMLCPP   -- defined if the yaml-cpp library is available
    AC_SUBST(ROSE_YAMLCPP_PREFIX)
    AM_CONDITIONAL(ROSE_HAVE_YAMLCPP, [test -n "$ROSE_HAVE_YAMLCPP"])
    if test -n "$ROSE_HAVE_YAMLCPP"; then
        AC_DEFINE(ROSE_HAVE_YAMLCPP, [], [Defined when libyaml-cpp is available])
    fi
])
