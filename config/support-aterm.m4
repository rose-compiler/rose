##### http://www.rosecompiler.org
#
# SYNOPSIS
#
#   ROSE_SUPPORT_ATERM([])
#
# DESCRIPTION
#
#   Determine if ATERM compiler installation is provided.
#
#     --with-aterm
#     --with-aterm-lib
#     --with-aterm-bin
#
#   This macro calls:
#
#     AC_SUBST(ATERM_INSTALL_PATH)
#     AC_SUBST(ATERM_BIN_PATH)
#     AC_SUBST(ATERM_LIBRARY_PATH)
#     AC_SUBST(ATERM_VERSION)
#
#   And sets:
#
#     Automake conditionals:
#     AM_CONDITIONAL(ROSE_WITH_ATERM)
#     AM_CONDITIONAL(ROSE_WITH_ATERM_BIN)
#     AM_CONDITIONAL(ROSE_WITH_ATERM_LIB)
#     AM_CONDITIONAL(ROSE_BUILD_ATERM_SUPPORT)
#
#     CPP #defines:
#     AC_DEFINE_UNQUOTED(ATERM_INSTALL_PATH)
#     AC_DEFINE_UNQUOTED(ATERM_BIN_PATH)
#     AC_DEFINE_UNQUOTED(ATERM_LIBRARY_PATH)
#     AC_DEFINE_UNQUOTED(ATERM_VERSION)
#
# EXAMPLE
#
#   if test "x$ATERM_BIN_PATH" = "x"; then
#       AC_MSG_ERROR([No ATERM/bin])
#   elif test "x$ATERM_LIBRARY_PATH" = "x"; then
#       AC_MSG_ERROR([No ATERM/lib])
#   fi
#
# COPYLEFT
#
#   Copyright (c) 2013 Justin Too <too1@llnl.gov>
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([ROSE_SUPPORT_ATERM],
[
  AC_BEFORE([$0], [ROSE_SUPPORT_ATERM_FRONTEND])

  ROSE_CONFIGURE_SECTION([Checking ATERM library])

  #============================================================================
  # --with-aterm=/path/to/aterm-trunk/aterm.dist/
  #============================================================================
  ROSE_ARG_WITH(
    [aterm],
    [for an installation of the Aterm library],
    [specify the path to the ATerm installation],
    []
  )
  if test "x$CONFIG_HAS_ROSE_WITH_ATERM" != "xno"; then
    ATERM_INSTALL_PATH="$ROSE_WITH_ATERM"
    ATERM_BIN_PATH="${ROSE_WITH_ATERM}/bin"
    ATERM_LIBRARY_PATH="${ROSE_WITH_ATERM}/lib"
  else
    ATERM_INSTALL_PATH=
    ATERM_BIN_PATH=
    ATERM_LIBRARY_PATH=
  fi

  #============================================================================
  # --with-aterm-bin=/path/to/aterm-trunk/aterm.dist/bin/
  #============================================================================
  ROSE_ARG_WITH(
    [aterm-bin],
    [if the ATERM compiler bin directory was specified],
    [use this ATERM bin directory],
    []
  )
  if test "x$CONFIG_HAS_ROSE_WITH_ATERM_BIN" != "xno"; then
      ATERM_BIN_PATH="$ROSE_WITH_ATERM_BIN"
  fi

  #============================================================================
  # --with-aterm-lib=/path/to/aterm-trunk/aterm.dist/lib/
  #============================================================================
  ROSE_ARG_WITH(
    [aterm-lib],
    [if the ATERM compiler library directory was specified],
    [use this ATERM library directory],
    []
  )
  if test "x$CONFIG_HAS_ROSE_WITH_ATERM_LIB" != "xno"; then
      ATERM_LIBRARY_PATH="$ROSE_WITH_ATERM_LIB"
  fi

# DQ (3/23/2013): debugging...
AC_MSG_NOTICE([ATERM_INSTALL_PATH = "$ATERM_INSTALL_PATH"])
AC_MSG_NOTICE([ATERM_BIN_PATH     = "$ATERM_BIN_PATH"])
AC_MSG_NOTICE([ATERM_LIBRARY_PATH = "$ATERM_LIBRARY_PATH"])

  #============================================================================
  # Validate installation (if provided)
  #============================================================================
  if test "x$ATERM_BIN_PATH" != "x"; then
      #======================================
      # atermc - Check for the ATERM compiler
      #======================================
      AC_CHECK_FILE(
          [${ATERM_BIN_PATH}],
          [],
          [ROSE_MSG_ERROR([bin directory is missing, can't use this ATERM installation])])

      #======================================
      # Using atrmannos to check version of ATERM library
      #======================================
      # AC_MSG_CHECKING([for the Aterm version])
      # ATERM_VERSION="`${ATERM_BIN_PATH}/atrmannos -v | grep 'atrmannos - Version:' | awk '{print [$][3]}'`"
      # exit_status=$?
      # pipe_status=[${PIPESTATUS[0]}]
      # if test ${exit_status} -ne 0 || test ${pipe_status} -ne 0; then
      #     ROSE_MSG_ERROR([Could not determine your ATERM version])
      # else
      #     AC_MSG_RESULT([$ATERM_VERSION])
      # fi
  fi

  #============================================================================
  # Set Automake Conditionals and Substitutions
  #============================================================================
  AM_CONDITIONAL(ROSE_WITH_ATERM, [test "x$ATERM_BIN_PATH" != "x" && test "x$ATERM_LIBRARY_PATH" != "x"])
  AM_CONDITIONAL(ROSE_WITH_ATERM_BIN, [test "x$ATERM_BIN_PATH" != "x"])
  AM_CONDITIONAL(ROSE_WITH_ATERM_LIB, [test "x$ATERM_LIBRARY_PATH" != "x"])
  AM_CONDITIONAL(ROSE_BUILD_ATERM_SUPPORT, [test "x$ATERM_LIBRARY_PATH" != "x"])

  AC_SUBST(ATERM_INSTALL_PATH)
  AC_SUBST(ATERM_BIN_PATH)
  AC_SUBST(ATERM_LIBRARY_PATH)
  # AC_SUBST(ATERM_VERSION)

  #============================================================================
  # Set CPP #defines
  #============================================================================
  AC_DEFINE_UNQUOTED(
    ATERM_INSTALL_PATH,
    ["$ATERM_INSTALL_PATH"],
    [Absolute path of the ATERM installation])
  AC_DEFINE_UNQUOTED(
    ATERM_BIN_PATH,
    ["$ATERM_BIN_PATH"],
    [Absolute path of the ATERM installation bin directory])
  AC_DEFINE_UNQUOTED(
    ATERM_LIBRARY_PATH,
    ["$ATERM_LIBRARY_PATH"],
    [Absolute path of the ATERM installation lib directory])
  # AC_DEFINE_UNQUOTED(
  #  ATERM_VERSION,
  #  ["$ATERM_VERSION"],
  #  [Version number of the user-specified ATERM compiler])


# DQ (10/4/2014): Adding macro for rose_config.h so we know when ATerms are supported.
# usingAterm = 'test "x$ATERM_LIBRARY_PATH" != "x" && echo "true"'
# AC_DEFINE(USE_ROSE_ATERM_SUPPORT,[$usingAterm],[Controls use of ROSE support for ATerm Library.])
if test "x$ATERM_LIBRARY_PATH" != "x"; then
   AC_DEFINE([USE_ROSE_ATERM_SUPPORT], [1], [Controls use of ROSE support for ATerm Library.])
fi

# End macro ROSE_SUPPORT_ATERM.
])
