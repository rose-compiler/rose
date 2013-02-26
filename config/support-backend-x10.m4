##### http://www.rosecompiler.org
#
# SYNOPSIS
#
#   ROSE_SUPPORT_X10_BACKEND([])
#
# DESCRIPTION
#
#   Determine if a working alternate X10 Backend compiler
#   is specified.
#
#     --enable-alternate_backend_x10_compiler
#
#   This macro calls:
#
#     AC_SUBST(BACKEND_X10_COMPILER)
#
#   And sets:
#
#     CPP #defines:
#     AC_DEFINE_UNQUOTED([BACKEND_X10_COMPILER_NAME_WITH_PATH])
#
# LAST MODIFICATION
#
#   2013-02-27
#
# COPYLEFT
#
#   Copyright (c) 2013 Justin Too <too1@llnl.gov>
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([ROSE_SUPPORT_X10_BACKEND],
[
  # --with-alternate_backend_x10_compiler
  ROSE_ARG_WITH(
    [alternate_backend_x10_compiler],
    [for the X10 backend compiler],
    [Specify an alternative X10 compiler Backend],
    [x10c])

  if test "x$ROSE_WITH_ALTERNATE_BACKEND_X10_COMPILER" != "x"; then
      BACKEND_X10_COMPILER="$ROSE_WITH_ALTERNATE_BACKEND_X10_COMPILER"

      # Simple test that the X10 compiler works
      "$BACKEND_X10_COMPILER" -version
      if test $? -ne 0; then
          ROSE_MSG_ERROR([The backend X10 compiler does not seem to work])
      fi

      AC_SUBST(BACKEND_X10_COMPILER)
      AC_DEFINE_UNQUOTED(
          [BACKEND_X10_COMPILER_NAME_WITH_PATH],
          ["$BACKEND_X10_COMPILER"],
          [Name of backend X10 compiler, including path])
  fi
])

