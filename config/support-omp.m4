AC_DEFUN([ROSE_SUPPORT_OMNI_OPENMP],
[
# Begin macro ROSE_SUPPORT_OMNI_OPENMP.

# Inclusion of test for Omni OpenMP Runtime system and its location.

AC_MSG_CHECKING(for OpenMP using Omni runtime library)
AC_ARG_WITH(omni_omp_runtime_support,
[  --with-omni_omp_runtime_support=PATH	Specify the prefix where Omni OpenMP Runtime System is installed],
,
if test ! "$with_omni_omp_runtime_support" ; then
   with_omni_omp_runtime_support=no
fi
)

echo "In ROSE SUPPORT MACRO: with_omni_omp_runtime_support $with_omni_omp_runtime_support"

if test "$with_omni_omp_runtime_support" = no; then
   # If omni_omp_runtime_support is not specified, then don't use it.
   echo "Skipping use of Omni OpenMP Runtime support!"
else
   omni_omp_runtime_support_path=$with_omni_omp_runtime_support
   echo "Setup Omni OpenMP support in ROSE! path = $omni_omp_runtime_support_path"
   AC_DEFINE([USE_ROSE_OMNI_OPENMP_SUPPORT],1,[Controls use of ROSE support for OpenMP Translator targeting Omni RTL.])
   AC_DEFINE_UNQUOTED([OMNI_OPENMP_LIB_PATH],"$omni_omp_runtime_support_path",[Location (unquoted) of the Omni OpenMP run time library.])
fi

AC_SUBST(omni_omp_runtime_support_path)

# End macro ROSE_SUPPORT_OMNI_OPENMP.
AM_CONDITIONAL(ROSE_USE_OMNI_OPENMP,test ! "$with_omni_omp_runtime_support" = no)

]
)


