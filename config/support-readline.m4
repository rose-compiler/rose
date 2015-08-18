# Tests for presence of the GNU libreadline library
AC_DEFUN([ROSE_SUPPORT_LIBREADLINE],
[
  ROSE_ARG_WITH(
    [libreadline],
    [for libreadline],
    [Use the GNU libreadline library for interactive input. If not present then editing of input lines will not be possible.],
    []
  )
  if test "x$CONFIG_HAS_ROSE_WITH_LIBREADLINE" = "xno"; then
    LIBREADLINE_INSTALL_PATH=
    LIBREADLINE_INCLUDE_PATH=
    LIBREADLINE_LIBRARY_PATH=
  elif test "x$CONFIG_HAS_ROSE_WITH_LIBREADLINE" = "xyes"; then
    LIBREADLINE_INSTALL_PATH=/usr
    LIBREADLINE_INCLUDE_PATH=/usr/include
    LIBREADLINE_LIBRARY_PATH=/usr/lib
  else
    LIBREADLINE_INSTALL_PATH="$ROSE_WITH_LIBREADLINE"
    LIBREADLINE_INCLUDE_PATH="$ROSE_WITH_LIBREADLINE/include"
    LIBREADLINE_LIBRARY_PATH="$ROSE_WITH_LIBREADLINE/lib"
  fi

  ROSE_ARG_WITH(
    [libreadline-include],
    [if the GNU libreadline include directory was specified],
    [use this GNU libreadline include directory],
    []
  )
  if test "x$CONFIG_HAS_ROSE_WITH_LIBREADLINE_INCLUDE" != "xno"; then
      LIBREADLINE_INCLUDE_PATH="$ROSE_WITH_LIBREADLINE_INCLUDE"
  fi

  ROSE_ARG_WITH(
    [libreadline-lib],
    [if the GNU libreadline library directory was specified],
    [use this GNU libreadline library directory],
    []
  )
  if test "x$CONFIG_HAS_ROSE_WITH_LIBREADLINE_LIB" != "xno"; then
      LIBREADLINE_LIBRARY_PATH="$ROSE_WITH_LIBREADLINE_LIB"
  fi

  if test "x$LIBREADLINE_INCLUDE_PATH" != "x"; then
      AC_CHECK_FILE(
          [${LIBREADLINE_INCLUDE_PATH}/readline/readline.h],
          [],
          [ROSE_MSG_ERROR([libreadline.h is missing, can't compile with readline])])
  fi

  if test "x$LIBREADLINE_LIBRARY_PATH" != "x"; then
      AC_CHECK_FILE(
          [${LIBREADLINE_LIBRARY_PATH}/libreadline.a],
          [],
          [ROSE_MSG_ERROR([libreadline.a is missing, can't compile with readline])])

      ORIG_LIBS="$LIBS"
      for readline_lib_dependency in "" termcap curses ncurses; do
          if test -z "$readline_lib_dependency"; then
            TRY_LIB="-lreadline"
          else
            TRY_LIB="-lreadline -l$readline_lib_dependency"
          fi
          LIBS="$ORIG_LIBS $TRY_LIB"
          AC_TRY_LINK_FUNC(readline, LIBREADLINE_LIBS="$TRY_LIB", LIBREADLINE_LIBS="")
          if test -n "$LIBREADLINE_LIBS"; then
            break
          fi
      done

      if test -z "$LIBREADLINE_LIBS"; then
        LIBREADLINE_LIBS=""
      fi

      LIBS="$ORIG_LIBS"

      #LDFLAGS="$LDFLAGS -L${LIBREADLINE_LIBRARY_PATH} ${LIBREADLINE_LIBS}"

      if test -z "$LIBREADLINE_LIBS"; then
        ROSE_MSG_ERROR([libreadline.a is missing the readline function, can't compile with readline])
      fi
  fi

  AM_CONDITIONAL(ROSE_WITH_LIBREADLINE, [test "x$LIBREADLINE_INCLUDE_PATH" != "x" && test "x$LIBREADLINE_LIBRARY_PATH" != "x"])
  AM_CONDITIONAL(ROSE_WITH_LIBREADLINE_INCLUDE, [test "x$LIBREADLINE_INCLUDE_PATH" != "x"])

  AC_SUBST(LIBREADLINE_INSTALL_PATH)
  AC_SUBST(LIBREADLINE_INCLUDE_PATH)
  AC_SUBST(LIBREADLINE_LIBRARY_PATH)
  AC_SUBST(LIBREADLINE_LIBS)

  if test "x$LIBREADLINE_INCLUDE_PATH" != "x" -a "x$LIBREADLINE_LIBRARY_PATH" != "x"; then
    AC_DEFINE(ROSE_HAVE_LIBREADLINE, 1, [Defined if the readline library and headers are available.])
  fi

# End macro ROSE_SUPPORT_LIBREADLINE.
])

