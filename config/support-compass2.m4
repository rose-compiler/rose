##### http://www.rosecompiler.org
#
# SYNOPSIS
#
#   ROSE_SUPPORT_COMPASS2([])
#
# DESCRIPTION
#
#   Determine if support is requested for the Compass static analysis tool.
#
#   Test for the Xerces C++ XML library.
#
#   This macro calls:
#
#     AM_CONDITIONAL(ROSE_BUILD_COMPASS2, [test "x$HAVE_XERCES" = "xyes"])
#
# COPYLEFT
#
#   Copyright (c) 2011 Justin Too <too1@llnl.gov>
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([ROSE_SUPPORT_COMPASS2],
[
  AC_MSG_CHECKING([if the Compass static analysis tool should be built])

  WANT_COMPASS="no"
  AC_ARG_ENABLE([compass2],
      AC_HELP_STRING(
          [--enable-compass2],
          [build the Compass2 static analysis tool under projects/]
      ),
      [
        if test "x$enableval" = "xno"; then
            WANT_COMPASS="no"
        elif test "x$enableval" = "xyes"; then
            WANT_COMPASS="yes"
        else
            AC_MSG_FAILURE([invalid value $enableval for --enable-compass2])
        fi
      ],
      [WANT_COMPASS="no"]
  )
  AC_MSG_RESULT([$WANT_COMPASS])

  dnl
  dnl Xerces C++ XML library
  dnl
  if test "x$WANT_COMPASS" = "xyes"; then
    dnl Don't need pthreads
    AC_DEFUN([AX_PTHREAD], [])
    AX_LIB_XERCES([3.1.1])
    if test "x$HAVE_XERCES" = "xno"; then
      AC_MSG_FAILURE([compass requires the Xerces C++ XML library])
    fi
  fi

  AM_CONDITIONAL(ROSE_BUILD_COMPASS2, [test "x$HAVE_XERCES" = "xyes"])

  ABS_COMPASS2_XML_SRCDIR="$(cd "$srcdir" && pwd)"/projects/compass2/share/xml
  AC_SUBST(ABS_COMPASS2_XML_SRCDIR)

# End macro ROSE_SUPPORT_COMPASS.
])

