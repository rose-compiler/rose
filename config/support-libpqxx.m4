dnl Tests for the libpqxx library, PostgreSQL C++ API
AC_DEFUN([ROSE_SUPPORT_LIBPQXX],[

    dnl Parse configure command-line switches
    AC_ARG_WITH([pqxx],
	        [AC_HELP_STRING([[[[--with-pqxx[=PREFIX]]]]], dnl yes, we really need 4 quotes (autoconf 2.6.1)!
		                [Use libpqxx available from http://pqxx.org/development/libpqxx/ for free.
				 On Debian-based systems its available as "libpqxx-dev".  The C PostgreSQL
				 library (libpq) must be installed before you can use libpqxx.])],
		[pqxx=$withval],
		[pqxx=default])

    ROSE_HAVE_LIBPQXX=

    # Does the user tell us where to find libpqxx?
    if test $pqxx = yes -o $pqxx = default; then
        ROSE_LIBPQXX_PREFIX=
	old_LIBS="$LIBS"
	LIBS="$LIBS -lpqxx -lpq"
	old_CPPFLAGS="$CPPFLAGS"
    elif test -n "$pqxx" -a "$pqxx" != no; then
        ROSE_LIBPQXX_PREFIX="$pqxx"
	old_LIBS="$LIBS"
	LIBS="$LIBS -L$ROSE_LIBPQXX_PREFIX/lib -lpqxx -lpq"
	old_CPPFLAGS="$CPPFLAGS"
	CPPFLAGS="-I$ROSE_LIBPQXX_PREFIX/include"
    fi

    # Search for the library
    if test $pqxx = no; then
        AC_MSG_NOTICE([skipping check for libpqxx])
    else
        AC_CHECKING([for libpqxx])
	AC_LANG_PUSH([C++])
	AC_LINK_IFELSE([
	    AC_LANG_SOURCE([
		    #include <pqxx/connection>
		    #include <pqxx/transaction>
		    #include <pqxx/tablewriter>
		    #include <string>
		    int main() {
			(void)pqxx::connection(std::string());
		    }
	    ])],
	    [
		AC_MSG_RESULT(yes)
		ROSE_HAVE_LIBPQXX=yes
		AC_DEFINE(ROSE_HAVE_LIBPQXX, [], [Defined if the pqxx library is available])
	    ],
	    [
		AC_MSG_RESULT(no)
		LIBS="$old_LIBS"
		CPPFLAGS="$old_CPPFLAGS"
	    ])
	AC_LANG_POP([C++])
    fi

    dnl Sanity check: if the user told us to use it then we must find it
    if test "$pqxx" != no -a "$pqxx" != default -a -z "$ROSE_HAVE_LIBPQXX"; then
        AC_MSG_ERROR([did not find pqxx library but --with-pqxx was specified])
    fi

    dnl Results
    dnl   ROSE_LIBPQXX_PREFIX -- name of the directory where pqxx is installed
    dnl   ROSE_HAVE_LIBPQXX   -- defined if the pqxx library is available
    AC_SUBST(ROSE_LIBPQXX_PREFIX)
    AM_CONDITIONAL(ROSE_HAVE_LIBPQXX, [test -n "$ROSE_HAVE_LIBPQXX"])
])
