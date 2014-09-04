##### http://www.rosecompiler.org
#
# SYNOPSIS
#
#   ROSE_SUPPORT_X10([])
#
# DESCRIPTION
#
#   Determine if X10 compiler installation is provided.
#
#     --with-x10
#     --with-x10-lib
#     --with-x10-bin
#
#   This macro calls:
#
#     AC_SUBST(X10_INSTALL_PATH)
#     AC_SUBST(X10_BIN_PATH)
#     AC_SUBST(X10_LIBRARY_PATH)
#     AC_SUBST(X10_VERSION)
#
#   And sets:
#
#     Automake conditionals:
#     AM_CONDITIONAL(ROSE_WITH_X10)
#     AM_CONDITIONAL(ROSE_WITH_X10_BIN)
#     AM_CONDITIONAL(ROSE_WITH_X10_LIB)
#
#     CPP #defines:
#     AC_DEFINE_UNQUOTED(X10_INSTALL_PATH)
#     AC_DEFINE_UNQUOTED(X10_BIN_PATH)
#     AC_DEFINE_UNQUOTED(X10_LIBRARY_PATH)
#     AC_DEFINE_UNQUOTED(X10_VERSION)
#
# EXAMPLE
#
#   if test "x$X10_BIN_PATH" = "x"; then
#       AC_MSG_ERROR([No X10/bin])
#   elif test "x$X10_LIBRARY_PATH" = "x"; then
#       AC_MSG_ERROR([No X10/lib])
#   fi
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

AC_DEFUN([ROSE_SUPPORT_X10],
[
  AC_BEFORE([$0], [ROSE_SUPPORT_X10_FRONTEND])

  ROSE_CONFIGURE_SECTION([X10 Compiler])

  #============================================================================
  # --with-x10=/path/to/x10-trunk/x10.dist/
  #============================================================================
  ROSE_ARG_WITH(
    [x10],
    [for an installation of the X10 compiler],
    [support the X10 compiler],
    []
  )
  if test "x$CONFIG_HAS_ROSE_WITH_X10" != "xno"; then
    X10_INSTALL_PATH="$ROSE_WITH_X10"
    X10_BIN_PATH="${ROSE_WITH_X10}/bin"
    X10_LIBRARY_PATH="${ROSE_WITH_X10}/lib"
  else
    X10_INSTALL_PATH=
    X10_BIN_PATH=
    X10_LIBRARY_PATH=
  fi

  #============================================================================
  # --with-x10-bin=/path/to/x10-trunk/x10.dist/bin/
  #============================================================================
  ROSE_ARG_WITH(
    [x10-bin],
    [if the X10 compiler bin directory was specified],
    [use this X10 bin directory],
    []
  )
  if test "x$CONFIG_HAS_ROSE_WITH_X10_BIN" != "xno"; then
      X10_BIN_PATH="$ROSE_WITH_X10_BIN"
  fi

  #============================================================================
  # --with-x10-lib=/path/to/x10-trunk/x10.dist/lib/
  #============================================================================
  ROSE_ARG_WITH(
    [x10-lib],
    [if the X10 compiler library directory was specified],
    [use this X10 library directory],
    []
  )
  if test "x$CONFIG_HAS_ROSE_WITH_X10_LIB" != "xno"; then
      X10_LIBRARY_PATH="$ROSE_WITH_X10_LIB"
  fi

  #============================================================================
  # Validate installation (if provided)
  #============================================================================
  if test "x$X10_BIN_PATH" != "x"; then
      #======================================
      # x10c - Check for the X10 compiler
      #======================================
      AC_CHECK_FILE(
          [${X10_BIN_PATH}/x10c],
          [],
          [ROSE_MSG_ERROR([x10c is missing, can't use this X10 installation])])

      #======================================
      # x10c - Check version of X10 compiler
      #======================================
      AC_MSG_CHECKING([for the x10c compiler version])
      X10_VERSION="`${X10_BIN_PATH}/x10c -version | grep 'x10c version' | awk '{print [$][3]}'`"
      exit_status=$?
      pipe_status=[${PIPESTATUS[0]}]
      if test ${exit_status} -ne 0 || test ${pipe_status} -ne 0; then
          ROSE_MSG_ERROR([Could not determine your X10 compiler version])
      else
          AC_MSG_RESULT([$X10_VERSION])
      fi
  fi

  #============================================================================
  # Validate X10 Jar files exist in X10 lib path
  #============================================================================
  if test "x$X10_LIBRARY_PATH" != "x"; then
      AC_CHECK_FILE(
          [${X10_LIBRARY_PATH}/x10c.jar],
          [],
          [ROSE_MSG_ERROR([x10c.jar is missing, can't use this X10 installation])])
      AC_CHECK_FILE(
          [${X10_LIBRARY_PATH}/lpg.jar],
          [],
          [ROSE_MSG_ERROR([lpg.jar is missing, can't use this X10 installation])])
      AC_CHECK_FILE(
          [${X10_LIBRARY_PATH}/com.ibm.wala.cast_1.0.0.201101071300.jar],
          [],
          [ROSE_MSG_ERROR([com.ibm.wala.cast_1.0.0.201101071300.jar is missing, can't use this X10 installation])])
      AC_CHECK_FILE(
          [${X10_LIBRARY_PATH}/com.ibm.wala.cast.java_1.0.0.201101071300.jar],
          [],
          [ROSE_MSG_ERROR([com.ibm.wala.cast.java_1.0.0.201101071300.jar is missing, can't use this X10 installation])])
      AC_CHECK_FILE(
          [${X10_LIBRARY_PATH}/com.ibm.wala.core_1.1.3.201101071300.jar],
          [],
          [ROSE_MSG_ERROR([com.ibm.wala.core_1.1.3.201101071300.jar is missing, can't use this X10 installation])])
      AC_CHECK_FILE(
          [${X10_LIBRARY_PATH}/com.ibm.wala.shrike_1.3.1.201101071300.jar],
          [],
          [ROSE_MSG_ERROR([com.ibm.wala.shrike_1.3.1.201101071300.jar is missing, can't use this X10 installation])])
      AC_CHECK_FILE(
          [${X10_LIBRARY_PATH}/x10wala.jar],
          [],
          [ROSE_MSG_ERROR([x10wala.jar is missing, can't use this X10 installation])])
      AC_CHECK_FILE(
          [${X10_LIBRARY_PATH}/commons-math3-3.3.jar],
          [],
          [ROSE_MSG_ERROR([commons-math3-3.3.jar is missing, can't use this X10 installation])])
      AC_CHECK_FILE(
          [${X10_LIBRARY_PATH}/ecj-4.4.jar],
          [],
          [ROSE_MSG_ERROR([ecj-4.4.jar is missing, can't use this X10 installation])])
      AC_CHECK_FILE(
          [${X10_LIBRARY_PATH}/org.eclipse.equinox.common_3.6.0.v20100503.jar],
          [],
          [ROSE_MSG_ERROR([org.eclipse.equinox.common_3.6.0.v20100503.jar is missing, can't use this X10 installation])])
  fi

  #============================================================================
  # Set Automake Conditionals and Substitutions
  #============================================================================
  AM_CONDITIONAL(ROSE_WITH_X10, [test "x$X10_BIN_PATH" != "x" && test "x$X10_LIBRARY_PATH" != "x"])
  AM_CONDITIONAL(ROSE_WITH_X10_BIN, [test "x$X10_BIN_PATH" != "x"])
  AM_CONDITIONAL(ROSE_WITH_X10_LIB, [test "x$X10_LIBRARY_PATH" != "x"])

  AC_SUBST(X10_INSTALL_PATH)
  AC_SUBST(X10_BIN_PATH)
  AC_SUBST(X10_LIBRARY_PATH)
  AC_SUBST(X10_VERSION)

  #============================================================================
  # Set CPP #defines
  #============================================================================
  AC_DEFINE_UNQUOTED(
    X10_INSTALL_PATH,
    ["$X10_INSTALL_PATH"],
    [Absolute path of the X10 installation])
  AC_DEFINE_UNQUOTED(
    X10_BIN_PATH,
    ["$X10_BIN_PATH"],
    [Absolute path of the X10 installation bin directory])
  AC_DEFINE_UNQUOTED(
    X10_LIBRARY_PATH,
    ["$X10_LIBRARY_PATH"],
    [Absolute path of the X10 installation lib directory])
  AC_DEFINE_UNQUOTED(
    X10_VERSION,
    ["$X10_VERSION"],
    [Version number of the user-specified X10 compiler])

# End macro ROSE_SUPPORT_X10.
])

