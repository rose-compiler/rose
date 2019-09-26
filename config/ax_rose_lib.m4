##### http://autoconf-archive.cryp.to/ax_rose.html
#
# SYNOPSIS
#
#   AX_ROSE_LIB
#
# DESCRIPTION
#
#   Test for program options library from the Boost C++ libraries. The
#   macro requires a preceding call to AX_ROSE_LIB_BASE. Further
#   documentation is available at
#   <http://randspringer.de/rose/index.html>.
#
#   This macro calls:
#
#     AC_SUBST(ROSE_LIB)
#
#   And sets:
#
#     HAVE_ROSE
#
# COPYLEFT
#
#   Copyright (c) 2007 Thomas Porschberg <thomas@randspringer.de>
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([AX_ROSE_LIB],
[
	AC_ARG_WITH([rose-lib],
		AS_HELP_STRING([--with-rose-lib@<:@=special-lib@:>@],
                       [use the program options library from rose - it is possible to specify a certain library for the linker
                        e.g. --with-rose-lib=rose-gcc-mt-1_33_1 ]),
        [
        if test "$withval" = "no"; then
			want_rose="no"
        elif test "$withval" = "yes"; then
            want_rose="yes"
            ax_rose_user_lib=""
        else
		    want_rose="yes"
        	ax_rose_user_lib="$withval"
		fi
        ],
        [want_rose="yes"]
	)

	if test "x$want_rose" = "xyes"; then
        AC_REQUIRE([AC_PROG_CC])
	    export want_rose
		CPPFLAGS_SAVED="$CPPFLAGS"
		CPPFLAGS="$CPPFLAGS $ROSE_CPPFLAGS $BOOST_CPPFLAGS "
		export CPPFLAGS
		LDFLAGS_SAVED="$LDFLAGS"
		LDFLAGS="$LDFLAGS $ROSE_LDFLAGS $BOOST_LDFLAGS "
		export LDFLAGS
		AC_CACHE_CHECK([whether the Boost::Program_Options library is available],
					   ax_cv_rose,
					   [AC_LANG_PUSH(C++)
 		                AC_COMPILE_IFELSE(AC_LANG_PROGRAM([[@%:@include <rose.h>
                                                          ]],
                                  [[ return 0;]]),
                           ax_cv_rose=yes, ax_cv_rose=no)
			               	AC_LANG_POP([C++])
		])
		if test "$ax_cv_rose" = yes; then
				AC_DEFINE(HAVE_ROSE,,[define if the Boost::PROGRAM_OPTIONS library is available])
                  ROSELIBDIR=`echo $ROSE_LDFLAGS | sed -e 's/@<:@^\/@:>@*//'`
                if test "x$ax_rose_user_lib" = "x"; then
                for libextension in `ls $ROSELIBDIR/librose*.{so,a}* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^lib\(rose.*\)\.so.*$;\1;' -e 's;^lib\(rose.*\)\.a*$;\1;'` ; do
                     ax_lib=${libextension}
				    AC_CHECK_LIB($ax_lib, exit,
                                 [ROSE_LIB="-l$ax_lib"; AC_SUBST(ROSE_LIB) link="yes"; break],
                                 [link="no"])
  				done
                if test "x$link" != "xyes"; then
                for libextension in `ls $ROSELIBDIR/rose*.{dll,a}* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^\(rose.*\)\.dll.*$;\1;' -e 's;^\(rose.*\)\.a*$;\1;'` ; do
                     ax_lib=${libextension}
				    AC_CHECK_LIB($ax_lib, exit,
                                 [ROSE_LIB="-l$ax_lib"; AC_SUBST(ROSE_LIB) link="yes"; break],
                                 [link="no"])
  				done
                fi
                else
                  for ax_lib in $ax_rose_user_lib rose-$ax_rose_user_lib; do
				      AC_CHECK_LIB($ax_lib, main,
                                   [ROSE_LIB="-l$ax_lib"; AC_SUBST(ROSE_LIB) link="yes"; break],
                                   [link="no"])
                  done
                fi
				if test "x$link" != "xyes"; then
					AC_MSG_ERROR([could not link against "$ax_lib"])
				fi
		fi
		CPPFLAGS="$CPPFLAGS_SAVED"
    	LDFLAGS="$LDFLAGS_SAVED"
	fi
])
