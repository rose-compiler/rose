#### http://www.rosecompiler.org
#
# SYNOPSIS
#
#   ROSE_SUPPORT_FLANG([])
#
# DESCRIPTION
#
#   Determine if F18/Flang compiler installation is provided.
#
#     --with-flang
#     --with-flang-src
#
#   This macro calls:
#
#     AC_SUBST(FLANG_INSTALL_PATH)
#     AC_SUBST(FLANG_SOURCE_PATH)
#
#   And sets:
#
#     Automake conditionals:
#     AM_CONDITIONAL(ROSE_EXPERIMENTAL_FLANG_ROSE_CONNECTION)
#
#     CPP #defines:
#     AC_DEFINE_UNQUOTED(FLANG_INSTALL_PATH)
#     AC_DEFINE_UNQUOTED(FLANG_SOURCE_PATH)
#
# EXAMPLE
#
#   if test "x$FLANG_INSTALL_PATH" = "x"; then
#       AC_MSG_ERROR([No FLANG/lib])
#   fi
#

AC_DEFUN([ROSE_SUPPORT_FLANG],
[
  ROSE_CONFIGURE_SECTION([Checking FLANG library])

  #============================================================================
  # --with-flang=/path/to/flang-installation/
  #============================================================================
  ROSE_ARG_WITH(
    [flang],
    [for an installation of the Flang compiler libraries],
    [specify the path to the Flang installation],
    []
  )
  if test "x$CONFIG_HAS_ROSE_WITH_FLANG" != "xno"; then
      FLANG_INSTALL_PATH="$ROSE_WITH_FLANG"
  else
    FLANG_INSTALL_PATH=
    FLANG_SOURCE_PATH=
  fi

  #============================================================================
  # --with-flang-src=/path/to/flang-source
  #============================================================================
  ROSE_ARG_WITH(
    [flang-src],
    [for an installation of the Flang compiler source files],
    [specify the path to the Flang compiler source files],
    []
  )
  if test "x$CONFIG_HAS_ROSE_WITH_FLANG_SRC" != "xno"; then
      FLANG_SOURCE_PATH="$ROSE_WITH_FLANG_SRC"
  fi

AC_MSG_NOTICE([FLANG_INSTALL_PATH = "$FLANG_INSTALL_PATH"])
AC_MSG_NOTICE([FLANG_SOURCE_PATH = "$FLANG_SOURCE_PATH"])

  #============================================================================
  # Validate installation (if provided)
  #============================================================================
  if test "x$FLANG_INSTALL_PATH" != "x"; then
      #===========================================
      # Check for the FLANG compiler lib directory
      #===========================================
      AC_CHECK_FILE(
          [${FLANG_INSTALL_PATH}/lib],
          [],
          [ROSE_MSG_ERROR([flang_install_path/lib directory is missing, can't use this FLANG installation])])
  fi
  if test "x$FLANG_SOURCE_PATH" != "x"; then
      #========================================================
      # Check for the FLANG parser directory (for include files
      #========================================================
      AC_CHECK_FILE(
          [${FLANG_SOURCE_PATH}/lib/parser],
          [],
          [ROSE_MSG_ERROR([flang_source_path/lib/parser directory is missing, can't use this FLANG installation])])
  fi

  #============================================================================
  # Set Automake Conditionals and Substitutions
  #============================================================================
  AM_CONDITIONAL(ROSE_WITH_FLANG, [test "x$FLANG_INSTALL_PATH" != "x"])
  AM_CONDITIONAL(ROSE_WITH_FLANG_SRC, [test "x$FLANG_SOURCE_PATH" != "x"])
  AM_CONDITIONAL(ROSE_EXPERIMENTAL_FLANG_ROSE_CONNECTION, [test "x$FLANG_INSTALL_PATH" != "x" && test "x$FLANG_SOURCE_PATH" != "x"])

  AC_SUBST(FLANG_INSTALL_PATH)
  AC_SUBST(FLANG_SOURCE_PATH)

  #============================================================================
  # Set CPP #defines
  #============================================================================
  AC_DEFINE_UNQUOTED(
    FLANG_INSTALL_PATH,
    ["$FLANG_INSTALL_PATH"],
    [Absolute path of the FLANG installation])
  AC_DEFINE_UNQUOTED(
    FLANG_SOURCE_PATH,
    ["$FLANG_SOURCE_PATH"],
    [Absolute path of the FLANG compilersource files])

# End macro ROSE_SUPPORT_FLANG.
])
