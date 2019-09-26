##### http://www.rosecompiler.org
#
# SYNOPSIS
#
#   ROSE_SUPPORT_GMP([])
#
# DESCRIPTION
#
#   Determine if support is requested for the GNU Multiple Precision Arithmetic Library.
#
# COPYLEFT
#
#   Copyright (c) 2011 Justin Too <too1@llnl.gov>
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([ROSE_SUPPORT_GMP],
[
  ROSE_ARG_WITH(
    [gmp],
    [for  (GMP)],
    [use the GNU Multiple Precision Arithmetic Library (GMP)],
    []
  )
  if test "x$CONFIG_HAS_ROSE_WITH_GMP" != "xno"; then
    GMP_INSTALL_PATH="$ROSE_WITH_GMP"
    GMP_INCLUDE_PATH="$ROSE_WITH_GMP/include"
    GMP_LIBRARY_PATH="$ROSE_WITH_GMP/lib"
  else
    GMP_INSTALL_PATH=
    GMP_INCLUDE_PATH=
    GMP_LIBRARY_PATH=
  fi

  ROSE_ARG_WITH(
    [gmp-include],
    [if the  GNU Multiple Precision Arithmetic Library (GMP) include directory was specified],
    [use this GNU Multiple Precision Arithmetic Library (GMP) include directory],
    []
  )
  if test "x$CONFIG_HAS_ROSE_WITH_GMP_INCLUDE" != "xno"; then
      GMP_INCLUDE_PATH="$ROSE_WITH_GMP_INCLUDE"
  fi

  ROSE_ARG_WITH(
    [gmp-lib],
    [if the GNU Multiple Precision Arithmetic Library (GMP) library directory was specified],
    [use this GNU Multiple Precision Arithmetic Library (GMP) library directory],
    []
  )
  if test "x$CONFIG_HAS_ROSE_WITH_GMP_LIB" != "xno"; then
      GMP_LIBRARY_PATH="$ROSE_WITH_GMP_LIB" 
  fi

  if test "x$GMP_INCLUDE_PATH" != "x"; then
      AC_CHECK_FILE(
          [${GMP_INCLUDE_PATH}/gmp.h],
          [],
          [ROSE_MSG_ERROR([gmp.h is missing, can't compile with GMP])])
  fi

  AM_CONDITIONAL(ROSE_WITH_GMP, [test "x$GMP_INCLUDE_PATH" != "x" && test "x$GMP_LIBRARY_PATH" != "x"])
  AM_CONDITIONAL(ROSE_WITH_GMP_INCLUDE, [test "x$GMP_INCLUDE_PATH" != "x"])

  AC_SUBST(GMP_INSTALL_PATH)
  AC_SUBST(GMP_INCLUDE_PATH)
  AC_SUBST(GMP_LIBRARY_PATH)

# End macro ROSE_SUPPORT_GMP.
])

