AC_DEFUN([ROSE_SUPPORT_GCC_OMP],
[
# Begin macro ROSE_SUPPORT_OMP.

# Inclusion of test for Omni OpenMP Runtime system and its location.

AC_MSG_CHECKING(for GCC OpenMP)
AC_ARG_WITH(parallel-ast-traversal-omp,
[  --with-parallel-ast-traversal-omp  Enables AST traversal in parallel using OpenMP.],
,
if test ! "$with_parallel-ast-traversal-omp" ; then
   with_parallel-ast-traversal-omp=no
fi
)

echo "In ROSE SUPPORT MACRO: with_parallel-ast-traversal-omp $with_parallel-ast-traversal-omp"

if test "$with_parallel-ast-traversal-omp" = no; then
   # If omp_runtime_support is not specified, then don't use it.
   echo "Skipping use of GCC OpenMP support!"
else
   parallel-ast-traversal-omp_path=$with_parallel-ast-traversal-omp
   echo "Setup OpenMP support in ROSE! path = $parallel-ast-traversal-omp_path"
   AC_DEFINE([USE_ROSE_GCC_OMP],1,[Use of GCC OpenMP in ROSE.])
   CFLAGS="$CFLAGS -fopenmp -pthread"
   LDFLAGS="$LDFLAGS -fopenmp -pthread"
   CXXFLAGS="$CXXFLAGS -fopenmp -pthread"
fi

AC_SUBST(parallel-ast-traversal-omp_path)

# End macro ROSE_SUPPORT_OMP.
]
)
