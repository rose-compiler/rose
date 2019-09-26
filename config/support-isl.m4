##### http://www.rosecompiler.org
#
# SYNOPSIS
#
#   ROSE_SUPPORT_ISL([])
#
# DESCRIPTION
#
#   Determine if support is requested for the Integer Set Library (ISL).
#
# COPYLEFT
#
#   Copyright (c) 2011 Justin Too <too1@llnl.gov>
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([ROSE_SUPPORT_ISL],
[
  ROSE_ARG_WITH(
    [isl],
    [for  (ISL)],
    [use the Integer Set Library (ISL)],
    []
  )
  if test "x$CONFIG_HAS_ROSE_WITH_ISL" != "xno"; then
    ISL_INSTALL_PATH="$ROSE_WITH_ISL"
    ISL_INCLUDE_PATH="$ROSE_WITH_ISL/include/isl"
    ISL_LIBRARY_PATH="$ROSE_WITH_ISL/lib"
  else
    ISL_INSTALL_PATH=
    ISL_INCLUDE_PATH=
    ISL_LIBRARY_PATH=
  fi

  ROSE_ARG_WITH(
    [isl-include],
    [if the Integer Set Library (ISL) include directory was specified],
    [use this Integer Set Library (ISL) include directory],
    []
  )
  if test "x$CONFIG_HAS_ROSE_WITH_ISL_INCLUDE" != "xno"; then
      ISL_INCLUDE_PATH="$ROSE_WITH_ISL_INCLUDE"
  fi

  ROSE_ARG_WITH(
    [isl-lib],
    [if the Integer Set Library (ISL) library directory was specified],
    [use this Integer Set Library (ISL) library directory],
    []
  )
  if test "x$CONFIG_HAS_ROSE_WITH_ISL_LIB" != "xno"; then
      ISL_LIBRARY_PATH="$ROSE_WITH_ISL_LIB" 
  fi

  if test "x$ISL_INCLUDE_PATH" != "x"; then
      AC_CHECK_FILE(
          [${ISL_INCLUDE_PATH}/set.h],
          [],
          [ROSE_MSG_ERROR([set.h is missing, can't compile with ISL])])
  fi

  AM_CONDITIONAL(ROSE_WITH_ISL, [test "x$ISL_INCLUDE_PATH" != "x" && test "x$ISL_LIBRARY_PATH" != "x"])
  AM_CONDITIONAL(ROSE_WITH_ISL_INCLUDE, [test "x$ISL_INCLUDE_PATH" != "x"])

  AC_SUBST(ISL_INSTALL_PATH)
  AC_SUBST(ISL_INCLUDE_PATH)
  AC_SUBST(ISL_LIBRARY_PATH)

# End macro ROSE_SUPPORT_ISL.
])

