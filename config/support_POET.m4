AC_DEFUN([CONFIGURE_POET],
[
test -n "$poet_dir" || poet_dir=$(top_srcdir)/src/3rdPartyLibraries/POET
POET_INCL="-I\$(poet_dir)"
POET_LIBS="\$(poet_dir)/libpoet.a"
AC_SUBST(POET_LIB)
AC_SUBST(POET_INCL)
AC_CONFIG_FILES([src/3rdPartyLibraries/POET/Makefile])
AC_ARG_VAR(poet_dir, directory of the POET interpreter (default: ROSE/src/3rdPartyLibraries/POET))
])
