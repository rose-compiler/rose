AC_DEFUN([ROSE_SUPPORT_OPENMP],
[
# Begin macro ROSE_SUPPORT_OPENMP.

# Inclusion of test for Omni OpenMP Runtime system and its location.

AC_MSG_CHECKING(for OpenMP)
AC_ARG_WITH(omp_runtime_support,
[  --with-omp_runtime_support=PATH	Specify the prefix where Omni OpenMP Runtime System is installed],
,
if test ! "$with_omp_runtime_support" ; then
   with_omp_runtime_support=no
fi
)

echo "In ROSE SUPPORT MACRO: with_omp_runtime_support $with_omp_runtime_support"

if test "$with_omp_runtime_support" = no; then
   # If omp_runtime_support is not specified, then don't use it.
   echo "Skipping use of OpenMP Runtime support!"
else
   omp_runtime_support_path=$with_omp_runtime_support
   echo "Setup OpenMP support in ROSE! path = $omp_runtime_support_path"
   AC_DEFINE([USE_ROSE_OPENMP_SUPPORT],1,[Controls use of ROSE support for OpenMP Translator.])
fi

AC_SUBST(omp_runtime_support_path)

# End macro ROSE_SUPPORT_OPENMP.
]
)
