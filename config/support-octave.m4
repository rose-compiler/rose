##### http://www.rosecompiler.org
#
# SYNOPSIS
#
#   ROSE_SUPPORT_OCTAVE([])
#
# DESCRIPTION
#
#   Determine if the modified GNU Octave parser is provided.
#
#     --with-octave-parser
#
#   This macro calls:
#
#     AC_SUBST(OCTAVE_PARSER_INSTALL_TARFILE)
#
#   And sets:
#
#     Automake conditionals:
#     AM_CONDITIONAL(ROSE_WITH_GNU_OCTAVE_PARSER)
#
#     CPP #defines:
#     AC_DEFINE_UNQUOTED(OCTAVE_PARSER_INSTALL_TARFILE)
#     AC_DEFINE(USE_ROSE_GNU_OCTAVE_SUPPORT)
#

AC_DEFUN([ROSE_SUPPORT_OCTAVE],
[
  AC_BEFORE([$0], [ROSE_SUPPORT_OCTAVE_FRONTEND])

  ROSE_CONFIGURE_SECTION([Checking modified GNU Octave parser])

  #============================================================================
  # --with-octave-parser=/path/to/octave-parser-install/
  #============================================================================
  ROSE_ARG_WITH(
    [octave-parser],
    [for an installation of the modified GNU Octave parser],
    [support for the GNU Octave parser],
    []
  )

  if test "x$CONFIG_HAS_ROSE_WITH_OCTAVE_PARSER" != "xno"; then
    OCTAVE_PARSER_INSTALL_TARFILE="$ROSE_WITH_OCTAVE_PARSER"
  else
    OCTAVE_PARSER_INSTALL_TARFILE=
  fi

  AC_MSG_NOTICE([OCTAVE_PARSER_INSTALL_TARFILE = "$OCTAVE_PARSER_INSTALL_TARFILE"])

  #============================================================================
  # Validate installation (if provided)
  #============================================================================
  if test "x$OCTAVE_INSTALL_PATH" != "x"; then
      #======================================
      #  - Check for the modified GNU octave parser file(s)
      #======================================
      AC_CHECK_FILE(
          [${OCTAVE_INSTALL_PATH}],
          [],
          [ROSE_MSG_ERROR([octave install file is missing, can't use this GNU Octave parser installation])])
  fi

  #============================================================================
  # Set Automake Conditionals and Substitutions
  #============================================================================
  AM_CONDITIONAL(ROSE_WITH_GNU_OCTAVE_PARSER, [test "x$OCTAVE_PARSER_INSTALL_TARFILE" != "x"])

  AC_SUBST(OCTAVE_PARSER_INSTALL_TARFILE)

  #============================================================================
  # Set CPP #defines
  #============================================================================
  AC_DEFINE_UNQUOTED(
    OCTAVE_PARSER_INSTALL_TARFILE,
    ["$OCTAVE_PARSER_INSTALL_TARFILE"],
    [Absolute path of the modified GNU Octave installation])

if test "x$OCTAVE_PARSER_INSTALL_TARFILE" != "x"; then
   AC_DEFINE([USE_ROSE_GNU_OCTAVE_SUPPORT], [1], [Controls use of ROSE support for the modified GNU Octave parser.])
fi

# End macro ROSE_SUPPORT_OCTAVE.
])

