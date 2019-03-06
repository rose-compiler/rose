##### http://www.rosecompiler.org
#
# SYNOPSIS
#
#   ROSE_SUPPORT_VALGRIND([])
#
# DESCRIPTION
#
#   Determine if support is requested for the Valgrind package.
#
# COPYLEFT
#
#   Copyright (c) 2013 Justin Too <too1@llnl.gov>
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([ROSE_SUPPORT_VALGRIND],
[
  ROSE_CONFIGURE_SECTION([Checking Valgrind])

  ROSE_ARG_WITH(
    [valgrind],
    [for Valgrind installation],
    [add Valgrind support in ROSE],
    []
  )

  if test "x$CONFIG_HAS_ROSE_WITH_VALGRIND" != "xno"; then
    VALGRIND_INSTALL_PATH="$ROSE_WITH_VALGRIND"
    VALGRIND_BIN_PATH="$ROSE_WITH_VALGRIND/bin"
    VALGRIND_INCLUDE_PATH="$ROSE_WITH_VALGRIND/include"
    VALGRIND_LIBRARY_PATH="$ROSE_WITH_VALGRIND/lib"
  else
    VALGRIND_INSTALL_PATH=
    VALGRIND_BIN_PATH=
    VALGRIND_INCLUDE_PATH=
    VALGRIND_LIBRARY_PATH=
  fi

  ROSE_ARG_WITH(
    [valgrind-bin],
    [if the Valgrind bin directory was specified],
    [use this Valgrind bin directory],
    []
  )
  if test "x$CONFIG_HAS_ROSE_WITH_VALGRIND_BIN" != "xno"; then
      VALGRIND_BIN_PATH="$ROSE_WITH_VALGRIND_BIN"
  fi

  ROSE_ARG_WITH(
    [valgrind-include],
    [if the Valgrind include directory was specified],
    [use this Valgrind include directory],
    []
  )
  if test "x$CONFIG_HAS_ROSE_WITH_VALGRIND_INCLUDE" != "xno"; then
      VALGRIND_INCLUDE_PATH="$ROSE_WITH_VALGRIND_INCLUDE"
  fi

  ROSE_ARG_WITH(
    [valgrind-lib],
    [if the Valgrind library directory was specified],
    [use this Valgrind library directory],
    []
  )
  if test "x$CONFIG_HAS_ROSE_WITH_VALGRIND_LIB" != "xno"; then
      VALGRIND_LIBRARY_PATH="$ROSE_WITH_VALGRIND_LIB"
  fi

  if test "x$VALGRIND_BIN_PATH" != "x"; then
      AC_CHECK_FILE(
          [${VALGRIND_BIN_PATH}/valgrind],
          [],
          [ROSE_MSG_ERROR([valgrind binary is missing, can't configure with Valgrind])])
      if ! valgrind_version="$(${VALGRIND_BIN_PATH}/valgrind --version)"; then
          ROSE_MSG_ERROR([could not execute your Valgrind binary])
      else
          AC_MSG_CHECKING([valgrind version])
          AC_MSG_RESULT([$valgrind_version])
      fi
  fi

  if test "x$VALGRIND_INCLUDE_PATH" != "x"; then
      AC_CHECK_FILE(
          [${VALGRIND_INCLUDE_PATH}/valgrind/valgrind.h],
          [],
          [ROSE_MSG_ERROR([valgrind.h is missing, can't compile with Valgrind])])
  fi

  AM_CONDITIONAL(ROSE_WITH_VALGRIND, [test "x$VALGRIND_BIN_PATH" != "x"])
  AM_CONDITIONAL(ROSE_WITH_VALGRIND_BIN, [test "x$VALGRIND_BIN_PATH" != "x"])
  AM_CONDITIONAL(ROSE_WITH_VALGRIND_INCLUDE, [test "x$VALGRIND_INCLUDE_PATH" != "x"])
  AM_CONDITIONAL(ROSE_WITH_VALGRIND_LIBRARY, [test "x$VALGRIND_LIBRARY_PATH" != "x"])

  AC_SUBST(VALGRIND_INSTALL_PATH)
  AC_SUBST(VALGRIND_BIN_PATH)
  AC_SUBST(VALGRIND_INCLUDE_PATH)
  AC_SUBST(VALGRIND_LIBRARY_PATH)

# Don't set VALGRIND here because that turns on actually running valgrind in
# many tests, as opposed to just having the path available for
# uninitializedField_tests
VALGRIND_BINARY="${VALGRIND_BIN_PATH}/valgrind"
AC_SUBST(VALGRIND_BINARY)
#AC_SUBST(VALGRIND, $VALGRIND_BINARY)
AM_CONDITIONAL(USE_VALGRIND, [test "x$VALGRIND_BINARY" != "x" && test "x$VALGRIND_BIN_PATH" != "x"])

# End macro ROSE_SUPPORT_VALGRIND.
])

