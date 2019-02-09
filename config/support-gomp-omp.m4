AC_DEFUN([ROSE_WITH_GOMP_OPENMP_LIBRARY],
[
# Check if gomp 4.4 OpenMP runtime library is available
# Begin macro ROSE_WITH_GOMP_OPENMP_LIBRARY.
# Inclusion of test for GCC GOMP OpenMP Runtime system and its location.

AC_MSG_CHECKING(for OpenMP using gomp runtime library)
AC_ARG_WITH(gomp_omp_runtime_library,
[  --with-gomp_omp_runtime_library=PATH	Specify the prefix where GOMP Runtime System is installed],
,
if test ! "$with_gomp_omp_runtime_library" ; then
   with_gomp_omp_runtime_library=no
fi
)

AC_MSG_NOTICE([in ROSE SUPPORT MACRO: with_gomp_omp_runtime_library = "$with_gomp_omp_runtime_library"])

if test "$with_gomp_omp_runtime_library" = no; then
   # If gomp_omp_runtime_library is not specified, then don't use it.
   AC_MSG_NOTICE([skipping use of GOMP OpenMP Runtime Library.])
else
   gomp_omp_runtime_library_path=$with_gomp_omp_runtime_library
   AC_MSG_NOTICE([setup GCC GOMP OpenMP library in ROSE. path = "$gomp_omp_runtime_library_path"])
   AC_DEFINE([USE_ROSE_GOMP_OPENMP_LIBRARY],1,[Controls use of ROSE support for OpenMP Translator targeting GCC 4.4 's GOMP OpenMP RTL.])
   AC_DEFINE_UNQUOTED([GCC_GOMP_OPENMP_LIB_PATH],"$gomp_omp_runtime_library_path",[Location (unquoted) of the GCC 4.4's GOMP OpenMP runtime library.])
fi

AC_DEFINE_UNQUOTED([ROSE_INSTALLATION_PATH],"$prefix",[Location (unquoted) of the top directory path to which ROSE is installed.])

AC_SUBST(gomp_omp_runtime_library_path)

# End macro ROSE_WITH_GOMP_OPENMP_LIBRARY.
AM_CONDITIONAL(WITH_GOMP_OPENMP_LIB,test ! "$with_gomp_omp_runtime_library" = no)

]
)
