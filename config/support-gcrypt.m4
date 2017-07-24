AC_DEFUN([ROSE_SUPPORT_LIBGCRYPT],
[
    AC_ARG_WITH([gcrypt],
		[AC_HELP_STRING([--with-gcrypt],
		                [Specify 'yes', 'no', (or --without-gcrypt), or an installation path to enable or disable
				 the use of libgcrypt. ROSE uses libgcrypt to compute checksums for binary analysis, and it's
				 use is optional.])],
		[],
		[with_gcrypt=no])


    ROSE_HAVE_LIBGCRYPT=
    if test "$with_gcrypt" = yes -o "$with_gcrypt" = ""; then
        # Find the gcrypt library in the default location
        LIBGCRYPT_PREFIX=
	AC_CHECK_LIB(gpg-error, gpg_strerror,
	             [AC_CHECK_LIB(gcrypt, gcry_check_version,
		                   [AC_DEFINE(ROSE_HAVE_LIBGCRYPT, [], [Defined when libgcrypt is available.])
				    ROSE_HAVE_LIBGCRYPT=yes
				    LIBGCRYPT_CPPFLAGS=
				    LIBGCRYPT_LDFLAGS="-lgcrypt -lgpg-error"
				   ])
                     ])
    elif test -n "$with_gcrypt" -a "$with_gcrypt" != no; then
        # Find the gcrypt library in the specified location
	LIBGCRYPT_PREFIX="$with_gcrypt"
	AC_CHECK_FILE(["$LIBGCRYPT_PREFIX/lib/libgcrypt.so"],
		      [AC_DEFINE(ROSE_HAVE_LIBGCRYPT, [], [Defined when libgcrypt is available.])
		       ROSE_HAVE_LIBGCRYPT=yes
		       LIBGCRYPT_CPPFLAGS="-I$LIBGCRYPT_PREFIX/include"
		       LIBGCRYPT_LDFLAGS="-L$LIBGCRYPT_PREFIX/lib -lgcrypt -lgpg-error"
		      ])
    fi

    # Sanity check: if the user told us to use libgcrypt then we must find it
    if test "$with_gcrypt" != no -a -z "$ROSE_HAVE_LIBGCRYPT"; then
        AC_MSG_ERROR([did not find libgcrypt but --with-gcrypt was specified])
    fi

    # Results:
    #    ROSE_HAVE_LIBGCRYPT	-- shell variable: non-empty when libgcrypt is available
    #    ROSE_HAVE_LIBGCRYPT    -- automake conditional: true when libgcrypt is available
    #    ROSE_HAVE_LIBGCRYPT    -- CPP symbol: defined when libgcrypt is available
    #    LIBGCRYPT_PREFIX	-- automake variable: name of the directory where gcrypt libraries and headers are installed
    #    LIBGCRYPT_CPPFLAGS	-- automake variable: extra CPP flags needed for using libgcrypt
    #    LIBGCRYPT_LDFLAGS	-- automake variable: extra loader floags to use libgcrypt
    AM_CONDITIONAL(ROSE_HAVE_LIBGCRYPT, [test -n "$ROSE_HAVE_LIBGCRYPT"])
    AC_SUBST(LIBGCRYPT_PREFIX)
    AC_SUBST(LIBGCRYPT_CPPFLAGS)
    AC_SUBST(LIBGCRYPT_LDFLAGS)
])
