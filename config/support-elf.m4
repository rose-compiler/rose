# ELF support library needed by libdwarf
AC_DEFUN([ROSE_SUPPORT_ELF],
[
    AC_ARG_WITH([elf],
		[AC_HELP_STRING([--with-elf],
				[Specify 'yes', 'no' (or --without-elf), or an installation path to enable or disable
				 the use of libdelf. The libelf is a prerequisite for libdwarf. See also, --with-dwarf.])],
		[],
		[with_elf=no])

    # Find libelf
    ROSE_HAVE_LIBELF=
    if test "$with_elf" = yes -o "$with_elf" = ""; then
	LIBELF_PREFIX=
	AC_CHECK_LIB(elf, elf_begin,
		     [AC_DEFINE(ROSE_HAVE_LIBELF, [], [Defined when libelf is available.])
		      ROSE_HAVE_LIBELF=yes
		      LIBELF_CPPFLAGS=
		      LIBELF_LDFLAGS="-lelf"])
    elif test -n "$with_elf" -a "$with_elf" != no; then
        LIBELF_PREFIX="$with_elf"
	# ROSE requires the use of a shared library for libelf
	AC_CHECK_FILE(["$LIBELF_PREFIX/lib/libelf.so"],
		      [AC_DEFINE(ROSE_HAVE_LIBELF, [], [Defined when libelf is available.])
		       ROSE_HAVE_LIBELF=yes
		       LIBELF_CPPFLAGS="-I$LIBELF_PREFIX/include"
		       LIBELF_LDFLAGS="-L$LIBELF_PREFIX/lib -lelf"
		       ])
    fi

    # Sanity check: if the user told us to use libelf then we must find the library
    if test "$with_elf" != no -a -z "$ROSE_HAVE_LIBELF"; then
        AC_MSG_ERROR([did not find libelf but --with-elf was specified])
    fi

    # Results
    #    ROSE_HAVE_LIBELF -- shell variable, non-empty when libelf is available
    #    ROSE_HAVE_LIBELF -- automake conditional, true when libelf is available
    #    ROSE_HAVE_LIBELF -- CPP symbol defined when libelf is available (see above)
    #    LIBELF_PREFIX    -- name of the directory where elf library and headers are installed
    #    LIBELF_CPPFLAGS  -- C preprocessor flags, such as -I
    #    LIBELF_LDFLAGS   -- Loader flags, such as -L and -l
    AM_CONDITIONAL(ROSE_HAVE_LIBELF, [test -n "$ROSE_HAVE_LIBELF"])
    AC_SUBST(LIBELF_PREFIX)
    AC_SUBST(LIBELF_CPPFLAGS)
    AC_SUBST(LIBELF_LDFLAGS)
])
