##### http://www.rosecompiler.org
#
# SYNOPSIS
#
#   ROSE_SUPPORT_COBOL([])
#
# DESCRIPTION
#
#   Determine if GnuCOBOL parse-tree installation is provided.
#
#     --with-cobpt
#
#   This macro calls:
#
#     AC_SUBST(COBPT_INSTALL_PATH)
#     AC_SUBST(COBPT_LIBRARY_PATH)
#
#   And sets:
#
#     Automake conditionals:
#     AM_CONDITIONAL(ROSE_WITH_GNU_COBOL_PT_LIB)
#
#     CPP #defines:
#     AC_DEFINE_UNQUOTED(COBPT_INSTALL_PATH)
#     AC_DEFINE_UNQUOTED(COBPT_LIBRARY_PATH)
#     AC_DEFINE(USE_ROSE_GNU_COBOL_PT_SUPPORT)
#

AC_DEFUN([ROSE_SUPPORT_COBOL],
[
  AC_BEFORE([$0], [ROSE_SUPPORT_COBOL_FRONTEND])

  ROSE_CONFIGURE_SECTION([Checking GnuCOBOL parse-tree library])

  #============================================================================
  # --with-cobpt=/path/to/cobpt-install/
  #============================================================================
  ROSE_ARG_WITH(
    [cobpt],
    [for an installation of the GnuCobol parse-tree library],
    [support the GnuCobol cobpt library],
    []
  )

  if test "x$CONFIG_HAS_ROSE_WITH_COBPT" != "xno"; then
    COBPT_INSTALL_PATH="$ROSE_WITH_COBPT"
    COBPT_LIBRARY_PATH="${ROSE_WITH_COBPT}/lib"
  else
    COBPT_INSTALL_PATH=
    COBPT_LIBRARY_PATH=
  fi

  AC_MSG_NOTICE([COBPT_INSTALL_PATH = "$COBPT_INSTALL_PATH"])
  AC_MSG_NOTICE([COBPT_LIBRARY_PATH = "$COBPT_LIBRARY_PATH"])

  #============================================================================
  # Validate installation (if provided)
  #============================================================================
  if test "x$COBPT_LIB_PATH" != "x"; then
      #======================================
      #  - Check for the GnuCOBOL parse-tree library
      #======================================
      AC_CHECK_FILE(
          [${COBPT_LIB_PATH}],
          [],
          [ROSE_MSG_ERROR([lib directory is missing, can't use this GnuCOBOL parse-tree installation])])
  fi

  #============================================================================
  # Set Automake Conditionals and Substitutions
  #============================================================================
  AM_CONDITIONAL(ROSE_WITH_GNU_COBOL_PT_LIB, [test "x$COBPT_INSTALL_PATH" != "x" && test "x$COBPT_LIBRARY_PATH" != "x"])

  AC_SUBST(COBPT_INSTALL_PATH)
  AC_SUBST(COBPT_LIBRARY_PATH)

  #============================================================================
  # Set CPP #defines
  #============================================================================
  AC_DEFINE_UNQUOTED(
    COBPT_INSTALL_PATH,
    ["$COBPT_INSTALL_PATH"],
    [Absolute path of the COBPT installation])
  AC_DEFINE_UNQUOTED(
    COBPT_LIBRARY_PATH,
    ["$COBPT_LIBRARY_PATH"],
    [Absolute path of the COBPT installation lib directory])

if test "x$COBPT_LIBRARY_PATH" != "x"; then
   AC_DEFINE([USE_ROSE_GNU_COBOL_PT_SUPPORT], [1], [Controls use of ROSE support for GnuCOBOL parse-tree library.])
fi

# End macro ROSE_SUPPORT_COBOL.
])

