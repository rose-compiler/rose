AC_DEFUN([ROSE_SUPPORT_RTED],
[
# Begin macro ROSE_SUPPORT_RTED.

AC_MSG_CHECKING(for rted)
AC_ARG_WITH(rted,
[  --with-rted=PATH     Configure option to have RTED enabled.],
,
if test ! "$with_rted" ; then
   with_rted=no
fi
)

AC_MSG_NOTICE([in ROSE SUPPORT MACRO: with_rted = "$with_rted"])

if test "$with_rted" = no; then
   # If RTED is not specified, then don't use it.
   AC_MSG_NOTICE([skipping use of RTED support])
else
   rted_path=$with_rted
   AC_MSG_NOTICE([setup RTED support in ROSE. path = "$rted_path"])
   AC_DEFINE([USE_ROSE_RTED_SUPPORT],1,[Controls use of ROSE support for RTED library.])
fi

AC_SUBST(rted_path)

]
)
