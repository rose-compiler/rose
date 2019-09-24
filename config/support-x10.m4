# ROSE no longer supports X10. This file exists only because the X10 supporting
# source code has not yet been removed from ROSE.
AC_DEFUN([ROSE_SUPPORT_X10],
[
    X10_INSTALL_PATH=
    X10_BIN_PATH=
    X10_LIBRARY_PATH=


  #============================================================================
  # Set Automake Conditionals and Substitutions
  #============================================================================
  AM_CONDITIONAL(ROSE_WITH_X10, [false])
  AM_CONDITIONAL(ROSE_WITH_X10_BIN, [false])
  AM_CONDITIONAL(ROSE_WITH_X10_LIB, [false])

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

