##### http://www.rosecompiler.org
#
# SYNOPSIS
#
#   ROSE_SUPPORT_STRATEGO([])
#
# DESCRIPTION
#
#   Determine if STRATEGO compiler installation is provided.
#
#     --with-stratego
#     --with-stratego-lib
#     --with-stratego-bin
#
#   This macro calls:
#
#     AC_SUBST(STRATEGO_INSTALL_PATH)
#     AC_SUBST(STRATEGO_BIN_PATH)
#     AC_SUBST(STRATEGO_LIBRARY_PATH)
#     AC_SUBST(STRATEGO_VERSION)
#
#   And sets:
#
#     Automake conditionals:
#     AM_CONDITIONAL(ROSE_WITH_STRATEGO)
#     AM_CONDITIONAL(ROSE_WITH_STRATEGO_BIN)
#     AM_CONDITIONAL(ROSE_WITH_STRATEGO_LIB)
#
#     CPP #defines:
#     AC_DEFINE_UNQUOTED(STRATEGO_INSTALL_PATH)
#     AC_DEFINE_UNQUOTED(STRATEGO_BIN_PATH)
#     AC_DEFINE_UNQUOTED(STRATEGO_LIBRARY_PATH)
#     AC_DEFINE_UNQUOTED(STRATEGO_VERSION)
#
# EXAMPLE
#
#   if test "x$STRATEGO_BIN_PATH" = "x"; then
#       AC_MSG_ERROR([No STRATEGO/bin])
#   elif test "x$STRATEGO_LIBRARY_PATH" = "x"; then
#       AC_MSG_ERROR([No STRATEGO/lib])
#   fi
#
# COPYLEFT
#
#   Copyright (c) 2013 Justin Too <too1@llnl.gov>
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([ROSE_SUPPORT_STRATEGO],
[
  AC_BEFORE([$0], [ROSE_SUPPORT_STRATEGO_FRONTEND])

  ROSE_CONFIGURE_SECTION([Checking STRATEGO library])

  #============================================================================
  # --with-stratego=/path/to/stratego-trunk/stratego.dist/
  #============================================================================
  ROSE_ARG_WITH(
    [stratego],
    [for an installation of the Stratego library],
    [support the Stratego library],
    []
  )
  if test "x$CONFIG_HAS_ROSE_WITH_STRATEGO" != "xno"; then
    STRATEGO_INSTALL_PATH="$ROSE_WITH_STRATEGO"
    STRATEGO_BIN_PATH="${ROSE_WITH_STRATEGO}/bin"
    STRATEGO_LIBRARY_PATH="${ROSE_WITH_STRATEGO}/lib"
  else
    STRATEGO_INSTALL_PATH=
    STRATEGO_BIN_PATH=
    STRATEGO_LIBRARY_PATH=
  fi

  #============================================================================
  # --with-stratego-bin=/path/to/stratego-trunk/stratego.dist/bin/
  #============================================================================
  ROSE_ARG_WITH(
    [stratego-bin],
    [if the STRATEGO compiler bin directory was specified],
    [use this STRATEGO bin directory],
    []
  )
  if test "x$CONFIG_HAS_ROSE_WITH_STRATEGO_BIN" != "xno"; then
      STRATEGO_BIN_PATH="$ROSE_WITH_STRATEGO_BIN"
  fi

  #============================================================================
  # --with-stratego-lib=/path/to/stratego-trunk/stratego.dist/lib/
  #============================================================================
  ROSE_ARG_WITH(
    [stratego-lib],
    [if the STRATEGO compiler library directory was specified],
    [use this STRATEGO library directory],
    []
  )
  if test "x$CONFIG_HAS_ROSE_WITH_STRATEGO_LIB" != "xno"; then
      STRATEGO_LIBRARY_PATH="$ROSE_WITH_STRATEGO_LIB"
  fi

# DQ (3/23/2013): debugging...
AC_MSG_NOTICE([STRATEGO_INSTALL_PATH = "$STRATEGO_INSTALL_PATH"])
AC_MSG_NOTICE([STRATEGO_BIN_PATH     = "$STRATEGO_BIN_PATH"])
AC_MSG_NOTICE([STRATEGO_LIBRARY_PATH = "$STRATEGO_LIBRARY_PATH"])

  #============================================================================
  # Validate installation (if provided)
  #============================================================================
  if test "x$STRATEGO_BIN_PATH" != "x"; then
      #======================================
      # strategoc - Check for the STRATEGO compiler
      #======================================
      AC_CHECK_FILE(
          [${STRATEGO_BIN_PATH}],
          [],
          [ROSE_MSG_ERROR([bin directory is missing, can't use this STRATEGO installation])])

      #======================================
      # Using atrmannos to check version of STRATEGO library
      #======================================
      # AC_MSG_CHECKING([for the Stratego version])
      # STRATEGO_VERSION="`${STRATEGO_BIN_PATH}/atrmannos -v | grep 'atrmannos - Version:' | awk '{print [$][3]}'`"
      # exit_status=$?
      # pipe_status=[${PIPESTATUS[0]}]
      # if test ${exit_status} -ne 0 || test ${pipe_status} -ne 0; then
      #     ROSE_MSG_ERROR([Could not determine your STRATEGO version])
      # else
      #     AC_MSG_RESULT([$STRATEGO_VERSION])
      # fi
  fi

  #============================================================================
  # Set Automake Conditionals and Substitutions
  #============================================================================
  AM_CONDITIONAL(ROSE_WITH_STRATEGO, [test "x$STRATEGO_BIN_PATH" != "x" && test "x$STRATEGO_LIBRARY_PATH" != "x"])
  AM_CONDITIONAL(ROSE_WITH_STRATEGO_BIN, [test "x$STRATEGO_BIN_PATH" != "x"])
  AM_CONDITIONAL(ROSE_WITH_STRATEGO_LIB, [test "x$STRATEGO_LIBRARY_PATH" != "x"])

  AC_SUBST(STRATEGO_INSTALL_PATH)
  AC_SUBST(STRATEGO_BIN_PATH)
  AC_SUBST(STRATEGO_LIBRARY_PATH)
  # AC_SUBST(STRATEGO_VERSION)

  #============================================================================
  # Set CPP #defines
  #============================================================================
  AC_DEFINE_UNQUOTED(
    STRATEGO_INSTALL_PATH,
    ["$STRATEGO_INSTALL_PATH"],
    [Absolute path of the STRATEGO installation])
  AC_DEFINE_UNQUOTED(
    STRATEGO_BIN_PATH,
    ["$STRATEGO_BIN_PATH"],
    [Absolute path of the STRATEGO installation bin directory])
  AC_DEFINE_UNQUOTED(
    STRATEGO_LIBRARY_PATH,
    ["$STRATEGO_LIBRARY_PATH"],
    [Absolute path of the STRATEGO installation lib directory])
  # AC_DEFINE_UNQUOTED(
  #  STRATEGO_VERSION,
  #  ["$STRATEGO_VERSION"],
  #  [Version number of the user-specified STRATEGO compiler])


# DQ (10/4/2014): Adding macro for rose_config.h so we know when Strategos are supported.
# usingStratego = 'test "x$STRATEGO_LIBRARY_PATH" != "x" && echo "true"'
# AC_DEFINE(USE_ROSE_STRATEGO_SUPPORT,[$usingStratego],[Controls use of ROSE support for Stratego Library.])
if test "x$STRATEGO_LIBRARY_PATH" != "x"; then
   AC_DEFINE([USE_ROSE_STRATEGO_SUPPORT], [1], [Controls use of ROSE support for Stratego Library.])
fi

# End macro ROSE_SUPPORT_STRATEGO.
])

