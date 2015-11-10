# Tests for presence of the GNU readline library
# NOTICE: The name of the library is "GNU readline"; the switch and most variables use "libreadline"
AC_DEFUN([ROSE_SUPPORT_LIBREADLINE],
[

    dnl Test for the "--with-libreadline" switch. If the value is "no" (or --without-libreadline) then none of the rest
    dnl of the readline-related settings are used.
    ROSE_ARG_WITH(
        [libreadline],
        [whether to use GNU readline],
        [Enable the use of the GNU readline library for interactive input. If not present then interactive inputs can
            be edited only with basic terminal editing commands like backspace, kill word, and kill line.  A readline
            installation directory can be specified for this switch, otherwise the default is /usr. To disable readline
            support in ROSE set this to "no" or use --without-libreadline.  The other readline-related switches are only
            processed when --with-libreadline is enabled.],
        []
    )
    if test "$ROSE_WITH_LIBREADLINE" = "no"; then
        LIBREADLINE_INSTALL_PATH=
        LIBREADLINE_INCLUDE_PATH=
        LIBREADLINE_LIBRARY_PATH=
    elif test "$ROSE_WITH_LIBREADLINE" = "yes"; then
        LIBREADLINE_INSTALL_PATH=/usr
        LIBREADLINE_INCLUDE_PATH=/usr/include
        LIBREADLINE_LIBRARY_PATH=/usr/lib
    else
        LIBREADLINE_INSTALL_PATH="$ROSE_WITH_LIBREADLINE"
        LIBREADLINE_INCLUDE_PATH="$ROSE_WITH_LIBREADLINE/include"
        LIBREADLINE_LIBRARY_PATH="$ROSE_WITH_LIBREADLINE/lib"
    fi

    if test "$ROSE_WITH_LIBREADLINE" != "no"; then
        dnl Since we have "--with-libreadline", process the optional "--with-libreadline-include" and "--with-libreadline-lib"
        dnl switches. If present, these override the defaults we set above.
        ROSE_ARG_WITH(
            [libreadline-include],
            [if the GNU readline include directory was specified],
            [Override the include directory for GNU readline rather than using a default based on --with-libreadline.],
            []
        )
        if test "$CONFIG_HAS_ROSE_WITH_LIBREADLINE_INCLUDE" != "no"; then
            LIBREADLINE_INCLUDE_PATH="$ROSE_WITH_LIBREADLINE_INCLUDE"
        fi

        ROSE_ARG_WITH(
            [libreadline-lib],
            [if the GNU readline library directory was specified],
            [Override the library directory for GNU readline rather than using a default based on --with-libreadline.],
            []
        )
        if test "$CONFIG_HAS_ROSE_WITH_LIBREADLINE_LIB" != "no"; then
            LIBREADLINE_LIBRARY_PATH="$ROSE_WITH_LIBREADLINE_LIB"
        fi

        dnl Find the readline header file, which must exist if the user wants readline support.
        AC_CHECK_FILE(
            [${LIBREADLINE_INCLUDE_PATH}/readline/readline.h],
            [],
            [ROSE_MSG_ERROR([libreadline.h is missing; use --with-libreadline=no to disable])])

        dnl Make sure the readline library can be linked.  The readline library sometimes requires additional libraries but
        dnl will often link fine during this configure check since those other features aren't used. Therefore, try to find
        dnl those libraries explicitly if we want readline.  This way of doing things will work regardless of whether these
        dnl libraries are static or shared.
        if test "$LIBREADLINE_LIBRARY_PATH" != ""; then
            dnl Use the termcap library if it's available.
            AC_CHECK_LIB(termcap, tputs,
                [LIBREADLINE_LIBS="-ltermcap $LIBREADLINE_LIBS"])

            dnl Use either ncurses or curses, but not both.
            AC_CHECK_LIB(ncurses, addch,
                [LIBREADLINE_LIBS="-lncurses $LIBREADLINE_LIBS"],
                [AC_CHECK_LIB(curses, addch,
                    [LIBREADLINE_LIBS="-lcurses $LIBREADLINE_LIBS"],
                    [], dnl failure is allowed
                    [$LIBREADLINE_LIBS])],
                [$LIBREADLINE_LIBS])

            dnl Look for -lreadline, but make sure that "-L" appears before the "-lreadline". It seems the only
            dnl way to do this is to add both compiler switches to the OTHER-LIBRARIES (5th) argument. We can't
            dnl leave the LIBRARY (1st) argument empty, so use some dummy library that we're sure exists; the
            dnl message spit out by this check will be misleading ("looking for readline in -lm")--oh well.
            AC_CHECK_LIB(m, readline,
                [LIBREADLINE_LIBS="-L$LIBREADLINE_LIBRARY_PATH -lreadline $LIBREADLINE_LIBS"],
                [ROSE_MSG_ERROR([libreadline.a is missing; use --with-libreadline=no to disable])],
                [-L$LIBREADLINE_LIBRARY_PATH -lreadline $LIBREADLINE_LIBS])
        fi
    fi

    dnl Final results.
    AM_CONDITIONAL(ROSE_WITH_LIBREADLINE, [test "$LIBREADLINE_INCLUDE_PATH" != "" && test "$LIBREADLINE_LIBRARY_PATH" != ""])
    AM_CONDITIONAL(ROSE_WITH_LIBREADLINE_INCLUDE, [test "$LIBREADLINE_INCLUDE_PATH" != ""])

    AC_SUBST(LIBREADLINE_INSTALL_PATH)
    AC_SUBST(LIBREADLINE_INCLUDE_PATH)
    AC_SUBST(LIBREADLINE_LIBRARY_PATH)
    AC_SUBST(LIBREADLINE_LIBS)

    if test "$LIBREADLINE_INCLUDE_PATH" != "" -a "$LIBREADLINE_LIBRARY_PATH" != ""; then
        AC_DEFINE(ROSE_HAVE_LIBREADLINE, 1, [Defined if the readline library and headers are available.])
    fi

    AC_MSG_NOTICE([LIBREADLINE_LIBS = $LIBREADLINE_LIBS])

])

