# DWARF debugging-format library and it's location.
AC_DEFUN([ROSE_SUPPORT_DWARF],
[
    AC_ARG_WITH(
        [dwarf],
        AS_HELP_STRING(
            [--with-dwarf=PREFIX],
            [Use the libdwarf library, which is necessary in order to parse debugging tables in ELF files.
             The PREFIX, if specified, should be the prefix used to install libdwarf, such as "/usr/local".
             The default is the empty prefix, in which case the headers and library must be installed in a
             place where they will be found. Saying "no" for the prefix is the same as saying
             "--without-dwarf". See also, --with-elf which is a prerequisite for --with-dwarf.]),
            [],
            [with_dwarf=no])

    # Find the dwarf library
    ROSE_HAVE_LIBDWARF=
    if test "$with_dwarf" = yes -o "$with_dwarf" = ""; then
        LIBDWARF_PREFIX=
        AC_CHECK_LIB(dwarf, dwarf_child,
                     [AC_DEFINE(ROSE_HAVE_LIBDWARF, [], [Defined when libdwarf is available.])
                      ROSE_HAVE_LIBDWARF=yes
                      LIBDWARF_CPPFLAGS=
                      LIBDWARF_LDFLAGS="-ldwarf"])
    elif test -n "$with_dwarf" -a "$with_dwarf" != no; then
        LIBDWARF_PREFIX="$with_dwarf"
        # ROSE requires the use of a shared library for libdwarf
        AC_CHECK_FILE(["$LIBDWARF_PREFIX/lib/libdwarf.so"],
                      [AC_DEFINE(ROSE_HAVE_LIBDWARF, [], [Defined when libdwarf is available.])
                       ROSE_HAVE_LIBDWARF=yes
                       LIBDWARF_CPPFLAGS="-I$LIBDWARF_PREFIX/include"
                       LIBDWARF_LDFLAGS="-L$LIBDWARF_PREFIX/lib -ldwarf"
                       ])
    fi

    # Sanity check: if the user told us to use libdwarf then we must find the library
    if test "$with_dwarf" != no -a -z "$ROSE_HAVE_LIBDWARF"; then
        AC_MSG_ERROR([did not find libdwarf but --with-dwarf was specified])
    fi
    if test "$with_dwarf" != no -a "$ROSE_HAVE_LIBDWARF" = yes -a "$ROSE_HAVE_LIBELF" != yes; then
        AC_MSG_ERROR([libdwarf depends on libelf, so you must specify --with-libelf also])
    fi
    

    # Results
    #    ROSE_HAVE_LIBDWARF -- shell variable, non-empty when libdwarf is available
    #    ROSE_HAVE_LIBDWARF -- automake conditional, true when libdwarf is available
    #    ROSE_HAVE_LIBDWARF -- CPP symbol defined when libdwarf is available (see above)
    #    LIBDWARF_PREFIX    -- name of the directory where dwarf library and headers are installed
    #    LIBDWARF_CPPFLAGS  -- C preprocessor flags, such as -I
    #    LIBDWARF_LDFLAGS   -- Loader flags, such as -L and -l
    AM_CONDITIONAL(ROSE_HAVE_LIBDWARF, [test -n "$ROSE_HAVE_LIBDWARF"])
    AC_SUBST(LIBDWARF_PREFIX)
    AC_SUBST(LIBDWARF_CPPFLAGS)
    AC_SUBST(LIBDWARF_LDFLAGS)
])


# OLD VARIABLES NO LONGER USED
#   ROSE_USE_DWARF         -- cmake variable to determine if libdwarf is present; use ROSE_HAVE_LIBDWARF instead
#   USE_ROSE_DWARF_SUPPORT -- CPP symbol to determine if libdwarf is present; use ROSE_HAVE_LIBDWARF instead
#   DWARF_INCLUDE          -- substitution for CPP flags; use LIBDWARF_CPPFLAGS instead
#   DWARF_LINK             -- substitution for loader flags; use LIBDWARF_LDFLAGS instead
#   dwarf_path             -- substitution for dwarf installation prefix; use LIBDWARF_PREFIX instead
