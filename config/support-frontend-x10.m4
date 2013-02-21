##### http://www.rosecompiler.org
#
# SYNOPSIS
#
#   ROSE_SUPPORT_X10_FRONTEND([])
#
# DESCRIPTION
#
#   Determine if ROSE X10 Frontend is requested, and
#   if required X10 installation is available.
#
#     --enable-frontend-x10
#     --with-x10
#
#   Note: Requires X10/bin and X10/lib.
#
#   This macro sets:
#
#     Automake conditionals:
#     AM_CONDITIONAL(ROSE_BUILD_X10_LANGUAGE_SUPPORT)
#
#
# LAST MODIFICATION
#
#   2013-02-20
#
# COPYLEFT
#
#   Copyright (c) 2013 Justin Too <too1@llnl.gov>
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([ROSE_SUPPORT_X10_FRONTEND],
[
  # --enable-frontend-x10
  ROSE_ARG_ENABLE(
    [frontend-x10],
    [if the X10 frontend is enabled],
    [Enable the ROSE X10 Frontend (requires the X10 compiler, see --with-x10)],
    [no]
  )

  if test "x$ROSE_ENABLE_FRONTEND_X10" = "xyes"; then
      # X10 Frontend requires an installation of the X10 Compiler
      if test "x$X10_BIN_PATH" = "x"; then
          ROSE_MSG_ERROR([The ROSE X10 Frontend requires the X10 compiler, see --help for --with-x10.])
      elif test "x$X10_LIBRARY_PATH" = "x"; then
          ROSE_MSG_ERROR([The ROSE X10 Frontend requires the X10 compiler libraries, see --help for --with-x10.])
      fi

      AM_CONDITIONAL(ROSE_BUILD_X10_LANGUAGE_SUPPORT, [true])
      AC_DEFINE([ROSE_BUILD_X10_LANGUAGE_SUPPORT], [], [Build ROSE with the X10 frontend])
  fi

# End macro ROSE_SUPPORT_X10_FRONTEND.
])

