##### http://www.rosecompiler.org
#
# SYNOPSIS
#
#   ROSE_SUPPORT_SPOT([])
#
# DESCRIPTION
#
#   Determine if support is requested for the SPOT LTL model checking library.
#
#####

AC_DEFUN([ROSE_SUPPORT_SPOT],
[
  ROSE_ARG_WITH(
    [spot],
    [for  (SPOT)],
    [use the SPOT LTL model checking library],
    []
  )
  if test "x$CONFIG_HAS_ROSE_WITH_SPOT" != "xno"; then
    SPOT_INSTALL_PATH="$ROSE_WITH_SPOT"
    SPOT_INCLUDE_PATH="$ROSE_WITH_SPOT/include"
    SPOT_LIBRARY_PATH="$ROSE_WITH_SPOT/lib"
    AC_DEFINE([HAVE_SPOT], [1], [Define to 1 if you have SPOT libs and headers.])
  else
    SPOT_INSTALL_PATH=
    SPOT_INCLUDE_PATH=
    SPOT_LIBRARY_PATH=
  fi

  if test "x$SPOT_INCLUDE_PATH" != "x"; then
      AC_CHECK_FILE(
          [${SPOT_INCLUDE_PATH}/bdd.h],
          [],
          [ROSE_MSG_ERROR([bdd.h is missing, can't compile with SPOT])])
  fi

  AM_CONDITIONAL(ROSE_WITH_SPOT, [test "x$SPOT_INCLUDE_PATH" != "x" && test "x$SPOT_LIBRARY_PATH" != "x"])

  AC_SUBST(SPOT_INSTALL_PATH)
  AC_SUBST(SPOT_INCLUDE_PATH)
  AC_SUBST(SPOT_LIBRARY_PATH)

# End macro ROSE_SUPPORT_SPOT.
])

