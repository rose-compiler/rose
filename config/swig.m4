AC_DEFUN([AC_PROG_SWIG],
[dnl
  has_swig=yes
# This uses SWIG as an environment variable instead of command line argument to configure.
  AC_ARG_VAR([SWIG], [SWIG binary])
  AC_CHECK_PROGS([SWIG], [swig],
    has_swig=no
    AC_MSG_WARN([Swig was not found. Please set the SWIG variable if it is not in your PATH.]))
  if test x$has_swig = xyes; then
    AC_CACHE_CHECK(
    # Change here if using an earlier version of swig
      [whether SWIG is >=1.3.31],
      [ac_cv_swig_ver],
      [dnl
    # Change here if using an earlier version of swig
         if $SWIG swig -version | sed "/SWIG Version /{;s///;s/^\(.*\)\.\(.*\)\.\(.*\)/\1 * 10000 + \2 * 100 + \3/;q;};d" | xargs expr | xargs test 10331 -le; then
           ac_cv_swig_ver=yes
         else
           ac_cv_swig_ver=no
         fi
      ])
    if test $ac_cv_swig_ver = no; then
    # Change here if using an earlier version of swig
      AC_MSG_WARN([Source has not been tested with a version of Swig prior to 1.3.31])
      has_swig=no
    fi
  fi
  AM_CONDITIONAL([HAS_SWIG], [test x$has_swig = xyes])
  AC_SUBST([SWIG])
])
