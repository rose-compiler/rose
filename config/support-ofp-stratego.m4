##### http://www.rosecompiler.org
#
# SYNOPSIS
#
#   ROSE_SUPPORT_OFP_STRATEGO([])
#
# DESCRIPTION
#
#   Determine if the OFP-Stratego tools installation is provided.
#
#     --with-ofp-bin
#
#   This macro calls:
#
#     AC_SUBST(OFP_BIN_PATH)
#
#   And sets:
#
#     Automake conditionals:
#     AM_CONDITIONAL(ROSE_WITH_OFP_BIN)
#
#     CPP #defines:
#     AC_DEFINE_UNQUOTED(OFP_BIN_PATH)
#
# EXAMPLE
#
#   if test "x$OFP_BIN_PATH" = "x"; then
#       AC_MSG_ERROR([No OFP-STRATEGO/bin])
#   fi
#
# COPYLEFT
#
#   Copyright (c) 2017 Craig Rasmussen <rasmussen17@llnl.gov>
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([ROSE_SUPPORT_OFP_STRATEGO],
[
  AC_BEFORE([$0], [ROSE_SUPPORT_OFP_STRATEGO_FRONTEND])

  ROSE_CONFIGURE_SECTION([Checking OFP-STRATEGO binaries])

  #============================================================================
  # --with-ofp-bin=/path/to/ofp-trunk/ofp.dist/bin/
  #============================================================================
  ROSE_ARG_WITH(
    [ofp-bin],
    [if the OFP-STRATEGO bin directory was specified],
    [use this OFP-STRATEGO bin directory],
    []
  )
  if test "x$CONFIG_HAS_ROSE_WITH_OFP_BIN" != "xno"; then
      OFP_BIN_PATH="$ROSE_WITH_OFP_BIN"
  fi

# debugging...
echo "OFP_BIN_PATH = "$OFP_BIN_PATH

  #============================================================================
  # Validate installation (if provided)
  #============================================================================
  if test "x$OFP_BIN_PATH" != "x"; then
      #========================================
      # OFP - Check for the Fortran parse table
      #========================================
      AC_CHECK_FILE(
          [${OFP_BIN_PATH/Fortran.tbl}],
          [],
          [ROSE_MSG_ERROR([Fortran parse table is missing, can't use this OFP installation])])
  fi

  #============================================================================
  # Set Automake Conditionals and Substitutions
  #============================================================================
  AM_CONDITIONAL(ROSE_WITH_OFP_BIN, [test "x$OFP_BIN_PATH" != "x"])

  AC_SUBST(OFP_BIN_PATH)

  #============================================================================
  # Set CPP #defines
  #============================================================================
  AC_DEFINE_UNQUOTED(
    OFP_BIN_PATH,
    ["$OFP_BIN_PATH"],
    [Absolute path of the OFP installation bin directory])

if test "x$OFP_BIN_PATH" != "x"; then
   AC_DEFINE([USE_ROSE_OFP_STRATEGO_SUPPORT], [1], [Controls use of ROSE support for OFP Stratego tools installation.])
fi

# End macro ROSE_SUPPORT_OFP_STRATEGO.
])

