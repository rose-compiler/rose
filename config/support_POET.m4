AC_DEFUN([CONFIGURE_POET],
[
AC_MSG_CHECKING(for POET)
# Begin macro CONFIGURE_POET

AC_ARG_WITH(poet,
[  --with-poet	Specify if we should build POET],
)
if test ! "$with_poet" ; then
   with_poet=no
else
   with_poet=yes
fi

AC_MSG_NOTICE([in ROSE SUPPORT MACRO: with_poet = "$with_poet"])

if test "$with_poet" = no; then
   AC_MSG_NOTICE([skipping installation for POET])
else
   test -n "$poet_dir" || poet_dir=$(top_srcdir)/src/3rdPartyLibraries/POET
   POET_INCL="-I\$(poet_dir)"
   POET_LIBS="\$(poet_dir)/libpoet.la"
   AC_SUBST(POET_LIB)
   AC_SUBST(POET_INCL)
   AC_CONFIG_FILES([src/3rdPartyLibraries/POET/Makefile])
   AC_ARG_VAR(poet_dir, directory of the POET interpreter (default: ROSE/src/3rdPartyLibraries/POET))
fi
   AC_MSG_NOTICE([setup installation for POET in ROSE. with_poet = "$with_poet"])
   AM_CONDITIONAL(ROSE_WITH_POET, [test "x$with_poet" = xyes])
])
