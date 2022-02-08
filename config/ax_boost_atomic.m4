##### http://autoconf-archive.cryp.to/ax_boost_atomic.html
#
# SYNOPSIS
#
#   AX_BOOST_ATOMIC
#
# DESCRIPTION
#
#   Test for Atomic library from the Boost C++ libraries. The macro
#   requires a preceding call to AX_BOOST_BASE. Further documentation
#   is available at <http://randspringer.de/boost/index.html>.
#
#   This macro calls:
#
#     AC_SUBST(BOOST_ATOMIC_LIB)
#
#   And sets:
#
#     HAVE_BOOST_ATOMIC
#
# COPYLEFT
#
#   Copyright (c) 2007 Thomas Porschberg <thomas@randspringer.de>
#   Copyright (c) 2007 Michael Tindal
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([AX_BOOST_ATOMIC],
[
	AC_ARG_WITH([boost-atomic],
	     AS_HELP_STRING([--with-boost-atomic@<:@=special-lib@:>@],
                   [use the Atomic library from boost - it is possible to specify a certain library for the linker
                        e.g. --with-boost-atomic=boost_atomic-gcc-mt-d-1_33_1 ]),
                   [
                   if test "$withval" = "no"; then
	           		want_boost="no"
                   elif test "$withval" = "yes"; then
                       want_boost="yes"
                       ax_boost_user_atomic_lib=""
                   else
	           	    want_boost="yes"
                   	ax_boost_user_atomic_lib="$withval"
	           fi
                   ],
                   [want_boost="yes"]
	)

     if test "x$want_boost" = "xyes"; then
         AC_REQUIRE([AC_PROG_CC])
	 CPPFLAGS_SAVED="$CPPFLAGS"
	 CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
	 export CPPFLAGS

	 LDFLAGS_SAVED="$LDFLAGS"
	 LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"
         export LDFLAGS

         AC_CACHE_CHECK(whether the Boost::Atomic library is available,
					   ax_cv_boost_atomic,
                   [AC_LANG_PUSH([C++])
           			 AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[@%:@include <boost/atomic.hpp> ]],
                                                   [[boost::atomic<int> r; return 0;]])],
                                                    ax_cv_boost_atomic=yes, ax_cv_boost_atomic=no)
                    AC_LANG_POP([C++])
		])

         if test "x$ax_cv_boost_atomic" = "xyes"; then
	        AC_DEFINE(HAVE_BOOST_ATOMIC,,[define if the Boost::Atomic library is available])
                BOOSTLIBDIR=`echo $BOOST_LDFLAGS | sed -e 's/@<:@^\/@:>@*//'`

                if test "x$ax_boost_user_atomic_lib" = "x"; then

                    for libextension in `ls $BOOSTLIBDIR/libboost_atomic*.{so,dylib,a}* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^lib\(boost_atomic.*\)\.so.*$;\1;' -e 's;^lib\(boost_atomic.*\)\.a*$;\1;' -e 's;^lib\(boost_atomic.*\)\.dylib$;\1;'` ; do
                         ax_lib=${libextension}
	                 AC_CHECK_LIB($ax_lib, toupper,
                                     [BOOST_ATOMIC_LIB="-l$ax_lib"; AC_SUBST(BOOST_ATOMIC_LIB) link_atomic="yes"; break],
                                     [link_atomic="no"])
  	            done

                    if test "x$link_atomic" != "xyes"; then
                        for libextension in `ls $BOOSTLIBDIR/boost_atomic*.{dll,a}* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^\(boost_atomic.*\)\.dll.*$;\1;' -e 's;^\(boost_atomic.*\)\.a*$;\1;'` ; do
                         ax_lib=${libextension}
	                 AC_CHECK_LIB($ax_lib, toupper,
                                     [BOOST_ATOMIC_LIB="-l$ax_lib"; AC_SUBST(BOOST_ATOMIC_LIB) link_atomic="yes"; break],
                                     [link_atomic="no"])
  	                done
                    fi
                else
                    for ax_lib in $ax_boost_user_atomic_lib boost_atomic-$ax_boost_user_atomic_lib; do
	            	AC_CHECK_LIB($ax_lib, main,
                                    [BOOST_ATOMIC_LIB="-l$ax_lib"; AC_SUBST(BOOST_ATOMIC_LIB) link_atomic="yes"; break],
                                    [link_atomic="no"])
                    done
                fi

	        if test "x$link_atomic" != "xyes"; then
	            AC_MSG_ERROR([could not link against "$ax_lib"])
                fi
         fi

         CPPFLAGS="$CPPFLAGS_SAVED"
    	 LDFLAGS="$LDFLAGS_SAVED"
    fi
])
