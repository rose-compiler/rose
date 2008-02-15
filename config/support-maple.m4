AC_DEFUN([ROSE_SUPPORT_MAPLE],
[
# Begin macro ROSE_SUPPORT_MAPLE.

# Inclusion of test for Maple Symbolic Algebra Package and it's location.

AC_MSG_CHECKING(for maple)
AC_ARG_WITH(maple,
[  --with-maple=PATH	Specify the prefix where Maple is installed],
,
if test ! "$with_maple" ; then
   with_maple=no
fi
)

echo "In ROSE SUPPORT MACRO: with_maple $with_maple"

if test "$with_maple" = no; then
   # If maple is not specified, then don't use it.
   echo "Skipping use of Maple!"
else
   maple_path=$with_maple
   echo "Setup Maple support in ROSE! path = $maple_path"
   AC_DEFINE([USE_ROSE_MAPLE_SUPPORT],1,[Controls use of ROSE support for Maple Symbolic Algebra Package.])
fi

AC_SUBST(maple_path)

# End macro ROSE_SUPPORT_MAPLE.
]
)
