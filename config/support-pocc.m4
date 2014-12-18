AC_DEFUN([ROSE_SUPPORT_POCC],
[
AC_MSG_CHECKING(for POCC)
# Begin macro ROSE_SUPPORT_POCC.

# Inclusion of installation for Polyhedral Compiler Collection

# There are currently no prerequisites
AC_ARG_WITH(pocc,
[  --with-pocc	Specify if we should build POCC],
,
if test ! "$with_pocc" ; then
   with_pocc=no
fi
)

echo "In ROSE SUPPORT MACRO: with_pocc $with_pocc"

if test "$with_pocc" = no; then
   echo "Skipping installation for POCC!"
else
   echo "Setup installation for POCC in ROSE! with_pocc = $with_pocc"
   AM_CONDITIONAL(ROSE_WITH_POCC, [test "x$with_pocc" = xyes])
fi

# End macro ROSE_SUPPORT_POCC.
]
)
