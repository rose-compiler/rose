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
#
#   And sets:
#
#     Automake conditionals:
#     AM_CONDITIONAL(ROSE_EXPERIMENTAL_FLANG_ROSE_CONNECTION)
#
#     CPP #defines:
#     AC_DEFINE_UNQUOTED(FLANG_INSTALL_PATH)
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

  AC_MSG_NOTICE([FLANG_INSTALL_PATH = "$FLANG_INSTALL_PATH"])

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

  #============================================================================
  # Set Automake Conditionals and Substitutions
  #============================================================================
  AM_CONDITIONAL(ROSE_WITH_FLANG, [test "x$FLANG_INSTALL_PATH" != "x"])
  AM_CONDITIONAL(ROSE_EXPERIMENTAL_FLANG_ROSE_CONNECTION, [test "x$FLANG_INSTALL_PATH" != "x"])

  AC_SUBST(FLANG_INSTALL_PATH)

  #============================================================================
  # Set CPP #defines
  #============================================================================
  AC_DEFINE_UNQUOTED(
    FLANG_INSTALL_PATH,
    ["$FLANG_INSTALL_PATH"],
    [Absolute path of the FLANG installation])

# End macro ROSE_SUPPORT_FLANG.
])
