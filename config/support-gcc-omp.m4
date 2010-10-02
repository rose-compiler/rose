AC_DEFUN([ROSE_SUPPORT_GCC_OMP],
[
# Begin macro ROSE_SUPPORT_OMP.

# Inclusion of test for Omni OpenMP Runtime system and its location.

AC_MSG_CHECKING(for GCC OpenMP)
AC_ARG_WITH(parallel_ast_traversal_omp,
[  --with-parallel_ast_traversal_omp  Enables AST traversal in parallel using OpenMP.],
,
if test ! "$with_parallel_ast_traversal_omp" ; then
   with_parallel_ast_traversal_omp=no
fi
)

echo "In ROSE SUPPORT MACRO: with_parallel_ast_traversal_omp $with_parallel_ast_traversal_omp"

if test "$with_parallel_ast_traversal_omp" = no; then
   # If omp_runtime_support is not specified, then don't use it.
   echo "Skipping use of GCC OpenMP support!"
else
   parallel_ast_traversal_omp_path=$with_parallel_ast_traversal_omp
   echo "Setup OpenMP support in ROSE! path = $parallel_ast_traversal_omp_path"
   AC_DEFINE([USE_ROSE_GCC_OMP],1,[Use of GCC OpenMP in ROSE.])
   CFLAGS="$CFLAGS -fopenmp -pthread"
   LDFLAGS="$LDFLAGS -fopenmp -pthread"
   CXXFLAGS="$CXXFLAGS -fopenmp -pthread"
fi

AC_SUBST(parallel_ast_traversal_omp_path)

# End macro ROSE_SUPPORT_OMP.
]
)
