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
#     CPP #defines:
#      AC_DEFINE_UNQUOTED([ROSE_BUILD_X10_LANGUAGE_SUPPORT])
#
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

AC_DEFUN([ROSE_SUPPORT_X10_FRONTEND],
[
  AC_BEFORE([$0], [ROSE_SUPPORT_X10_BACKEND])

  # --enable-frontend-x10
  ROSE_ARG_ENABLE(
    [frontend-x10],
    [if the X10 frontend is enabled],
    [Enable the ROSE X10 Frontend (requires the X10 compiler, see --with-x10)],
    [auto-enable])

  # Validate and Normalize CLI argument:
  #
  #   true->yes, false->no
  #   auto-enable->[yes,no] if X10 is available
  #       This argument is intended for the default case when
  #       the user does not specify --[enable/disable]-frontend-x10.
  #
  if test "x$ROSE_ENABLE_FRONTEND_X10" = "xtrue"; then
      ROSE_ENABLE_FRONTEND_X10="yes"
  elif test "x$ROSE_ENABLE_FRONTEND_X10" = "xfalse"; then
      ROSE_ENABLE_FRONTEND_X10="no"
  elif test "x$ROSE_ENABLE_FRONTEND_X10" = "xauto-enable"; then
      dnl See ROSE/config/support-x10.m4
      if test "x$X10_BIN_PATH"     != "x" && \
         test "x$X10_LIBRARY_PATH" != "x"
      then
          ROSE_ENABLE_FRONTEND_X10="yes"
      else
          ROSE_ENABLE_FRONTEND_X10="no"
      fi
  elif test "x$ROSE_ENABLE_FRONTEND_X10" = "xyes"; then
      :
  elif test "x$ROSE_ENABLE_FRONTEND_X10" = "xno"; then
      :
  else
      ROSE_MSG_ERROR([Unknown argument passed to --enable-frontend-x10: '$ROSE_ENABLE_FRONTEND_X10'])
  fi

  if test "x$ROSE_ENABLE_FRONTEND_X10" = "xyes"; then
      # X10 Frontend requires an installation of the X10 Compiler
      if test "x$X10_BIN_PATH" = "x"; then
          ROSE_MSG_ERROR([The ROSE X10 Frontend requires the X10 compiler, see --help for --with-x10.])
      elif test "x$X10_LIBRARY_PATH" = "x"; then
          ROSE_MSG_ERROR([The ROSE X10 Frontend requires the X10 compiler libraries, see --help for --with-x10.])
      fi

      AM_CONDITIONAL(ROSE_BUILD_X10_LANGUAGE_SUPPORT, [test "x" = "x"])
      AC_DEFINE_UNQUOTED(
          [ROSE_BUILD_X10_LANGUAGE_SUPPORT],
          [],
          [Build ROSE with the X10 frontend])
  else
      AM_CONDITIONAL(ROSE_BUILD_X10_LANGUAGE_SUPPORT, [false])
  fi

# End macro ROSE_SUPPORT_X10_FRONTEND.
])

