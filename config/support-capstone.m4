dnl Tests for the Capstone disassembler library
AC_DEFUN([ROSE_SUPPORT_CAPSTONE],[

    dnl Parse configure command-line switches
    AC_ARG_WITH([capstone],
	        [AC_HELP_STRING([[[[--with-capstone[=PREFIX]]]]], dnl yes, we really need 4 quotes (autoconf 2.6.1)!
		                [Use libcapstone available from http://www.capstone-engine.org for free.])],
		[capstone=$withval],
		[capstone=no])

    ROSE_HAVE_CAPSTONE=
    ROSE_CAPSTONE_PREFIX=

    # Save some variables to be restored later
    old_LIBS="$LIBS"
    old_CPPFLAGS="$CPPFLAGS"

    # Does the user tell us where to find it?
    if test "$capstone" = yes; then
	LIBS="$LIBS -lcapstone"
    elif test -n "$capstone" -a "$capstone" != no; then
        ROSE_CAPSTONE_PREFIX="$capstone"
	LIBS="$LIBS -L$ROSE_CAPSTONE_PREFIX/lib -lcapstone"
	CPPFLAGS="-I$ROSE_CAPSTONE_PREFIX/include"
    fi

    # Search for the library
    if test "$capstone" = no; then
        AC_MSG_NOTICE([skipping check for capstone])
    else
        AC_CHECKING([for capstone])
	AC_LANG_PUSH([C++])
	AC_LINK_IFELSE([
	    AC_LANG_SOURCE([
		    #include <capstone/capstone.h>
		    int main() {
		    	csh handle;
		        cs_open(CS_ARCH_X86, CS_MODE_64, &handle);
		    }
	    ])],
	    [
		AC_MSG_RESULT(yes)
		ROSE_HAVE_CAPSTONE=yes
		AC_DEFINE(ROSE_HAVE_CAPSTONE, [], [Defined if the Capstone library is available])
	    ],
	    [
		AC_MSG_RESULT(no)
		LIBS="$old_LIBS"
		CPPFLAGS="$old_CPPFLAGS"
	    ])
	AC_LANG_POP([C++])
    fi

    dnl Sanity check: if the user told us to use it then we must find it
    if test "$capstone" != no -a "$capstone" != default -a -z "$ROSE_HAVE_CAPSTONE"; then
        AC_MSG_ERROR([did not find Capstone library but --with-capstone was specified])
    fi

    # Restore variables
    LIBS="$old_LIBS"
    CPPFLAGS="$old_CPPFLAGS"

    # Change ROSE_HAVE_CAPSTONE to be "yes" or the prefix if it is present; or empty if not present
    if test -n "$ROSE_HAVE_CAPSTONE" -a -n "$ROSE_CAPSTONE_PREFIX"; then
        ROSE_HAVE_CAPSTONE="$ROSE_CAPSTONE_PREFIX"
    fi

    dnl Results
    dnl   ROSE_CAPSTONE_PREFIX -- name of the directory where Capstone is installed
    dnl   ROSE_HAVE_CAPSTONE   -- defined if the Capstone library is available
    AC_SUBST(ROSE_CAPSTONE_PREFIX)
    AM_CONDITIONAL(ROSE_HAVE_CAPSTONE, [test -n "$ROSE_HAVE_CAPSTONE"])
])
