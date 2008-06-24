AC_DEFUN([ROSE_SUPPORT_GCC_OMP],
[
# Begin macro ROSE_SUPPORT_OMP.

# Inclusion of test for Omni OpenMP Runtime system and its location.

AC_MSG_CHECKING(for GCC OpenMP)
AC_ARG_WITH(gcc_omp,
[  --with-gcc_omp=PATH	Specify the prefix where Omni OpenMP Runtime System is installed],
,
if test ! "$with_gcc_omp" ; then
   with_gcc_omp=no
fi
)

echo "In ROSE SUPPORT MACRO: with_gcc_omp $with_gcc_omp"

if test "$with_gcc_omp" = no; then
   # If omp_runtime_support is not specified, then don't use it.
   echo "Skipping use of GCC OpenMP support!"
else
   gcc_omp_path=$with_gcc_omp
   echo "Setup OpenMP support in ROSE! path = $gcc_omp_path"
   AC_DEFINE([USE_ROSE_GCC_OMP],1,[Use of GCC OpenMP in ROSE.])
   CFLAGS="$CFLAGS -fopenmp -pthread"
   LDFLAGS="$LDFLAGS -fopenmp -pthread"
   CXXFLAGS="$CXXFLAGS -fopenmp -pthread"
fi

AC_SUBST(gcc_omp_path)

# End macro ROSE_SUPPORT_OMP.
]
)
