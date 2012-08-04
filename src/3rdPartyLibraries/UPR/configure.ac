#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.59])
AC_INIT([UPR - Unified Parallel Runtime], [0.1], [vanderbruggentristan@gmail.com])
AM_INIT_AUTOMAKE([1.9 no-define foreign])

AC_CONFIG_MACRO_DIR([m4])

AC_PROG_LIBTOOL

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_MAKE_SET

# Declaration of ARG_WITH...
AC_ARG_WITH(rose,       [  --with-rose=DIR       ROSE Compiler is in charge of the code generation. Enable UPR-builder (Code Generation Backend)])
AC_ARG_WITH(boost,      [  --with-boost=DIR      Needed by ROSE Compiler])

AC_ARG_WITH(cuda-inc,   [  --with-cuda-inc=DIR   For CUDA runtime library])
AC_ARG_WITH(cuda-lib,   [  --with-cuda-lib=DIR   For CUDA runtime examples (need --with-cuda-inc=DIR)])

AC_ARG_WITH(xomp-inc,   [  --with-xomp-inc=DIR   For Xomp runtime library])
AC_ARG_WITH(xomp-lib,   [  --with-xomp-lib=DIR   For Xomp runtime examples (need --with-xomp-inc=DIR)])

# AC_ARG_WITH(rose): Rose install directory
if test "$with_rose" != ""
then
        AC_SUBST(ROSE_DIR, [$with_rose])
fi
AM_CONDITIONAL(HAVE_ROSE, [test "$with_rose" != ""])

# AC_ARG_WITH(boost): Boost install directory
if test "$with_boost" != ""
then
        AC_SUBST(BOOST_DIR, [$with_boost])
        if test "$with_rose" != ""
        then
                AC_ERROR([Need boost directory, if ROSE is present])
        fi
fi
AM_CONDITIONAL(HAVE_BOOST, [test "$with_boost" != ""])

AX_OPENCL([C++])
AM_CONDITIONAL(HAVE_OPENCL, [test "$_OPENCL" != ""])


# AC_ARG_WITH(cuda-inc): CUDA header files
if test "$with_cuda_inc" != ""
then
        AC_SUBST(CUDA_INC_DIR, [$with_cuda_inc])
fi
AM_CONDITIONAL(HAVE_CUDA_INC, [test "$with_cuda_inc" != ""])

# AC_ARG_WITH(cuda-lib): CUDA library
if test "$with_cuda_lib" != ""
then
        AC_SUBST(CUDA_LIB_DIR, [$with_cuda_lib])
        if test "$with_cuda_inc" == ""
        then
                 AC_ERROR([Need CUDA headers directory if the libraries directory are provided])
        fi
fi
AM_CONDITIONAL(HAVE_CUDA_LIB, test "$with_cuda_lib" != "")

# AC_ARG_WITH(xomp-inc): Xomp header files
if test "$with_xomp_inc" != ""
then
        AC_SUBST(XOMP_INC_DIR, [$with_xomp_inc])
fi
AM_CONDITIONAL(HAVE_XOMP_INC, [test "$with_xomp_inc" != ""])

# AC_ARG_WITH(xomp-lib): Xomp library
if test "$with_xomp_lib" != ""
then
        AC_SUBST(XOMP_LIB_DIR, [$with_xomp_lib])
        if test "$with_xomp_inc" == ""
        then
                 AC_ERROR([Need Xomp headers directory if the libraries directory are provided])
        fi
fi
AM_CONDITIONAL(HAVE_XOMP_LIB, [test "$with_xomp_lib" != ""])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 include/Makefile
                 include/UPR/Makefile
                 lib/Makefile
                 tools/Makefile
                 examples/Makefile
                 examples/cuda/Makefile
                 examples/opencl/Makefile
                 examples/xomp/Makefile
                 docs/Makefile
                 docs/doxygen/Makefile
                 docs/doxygen/doxy.conf
                 docs/figures/Makefile])

AC_OUTPUT
