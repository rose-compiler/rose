AC_DEFUN([ROSE_SUPPORT_SAT],
[
# Begin macro ROSE_SUPPORT_SAT.

# Inclusion of test for Internal SAT Solver 

# There are currently no prerequisites
AC_ARG_WITH(sat,
[  --with-sat	Specify if we should compile internal sata solver],
,
if test ! "$with_sat" ; then
   with_sat=no
fi
)

echo "In ROSE SUPPORT MACRO: with_sat $with_sat"

if test "$with_sat" = no; then
   echo "Skipping use of internal SAT Solver support!"
else
   echo "Setup internal SAT solver support in ROSE! with_sat = $with_sat"
   AC_DEFINE([USE_ROSE_SAT_SUPPORT],1,[Controls use of ROSE support for internal Satisfiability (SAT) Solver.])
fi

# End macro ROSE_SUPPORT_SAT.
]
)
