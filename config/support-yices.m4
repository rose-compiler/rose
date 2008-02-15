AC_DEFUN([ROSE_SUPPORT_YICES],
[
# Begin macro ROSE_SUPPORT_YICES.

# Inclusion of test for YICES SMT Solver Package and it's location.

AC_MSG_CHECKING(for yices)
AC_ARG_WITH(yices,
[  --with-yices=PATH	Specify the prefix where Yices SMT solver is installed],
,
if test ! "$with_yices" ; then
   with_yices=no
fi
)

echo "In ROSE SUPPORT MACRO: with_yices $with_yices"

if test "$with_yices" = no; then
   # If yices is not specified, then don't use it.
   echo "Skipping use of Yices SMT Solver support!"
else
   yices_path=$with_yices
   echo "Setup Yices support in ROSE! path = $yices_path"
   AC_DEFINE([USE_ROSE_YICES_SUPPORT],1,[Controls use of ROSE support for Yices Satisfiability Modulo Theories (SMT) Solver Package.])
fi

AC_SUBST(yices_path)

# End macro ROSE_SUPPORT_YICES.
]
)
