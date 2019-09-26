# Cut-n-pasted from boost_chrono.m4 instead of downloading from the original
# location, in order to preserve any local bug fixes already applied by the
# ROSE team to the boost_chrono detection.  All instances of "chrono" were
# then replaced by "random" (preserving capitalizations) and a single occurrence
# of "boost::chrono::system_clock::time_point time" was replaced by a suitable
# type from boost::random.

AC_DEFUN([AX_BOOST_RANDOM],
[
	AC_ARG_WITH([boost-random],
	AS_HELP_STRING([--with-boost-random@<:@=special-lib@:>@],
                   [use the Random library from boost - it is possible to specify a certain library for the linker
                        e.g. --with-boost-random=boost_random-gcc-mt ]),
        [
        if test "$withval" = "no"; then
			want_boost="no"
        elif test "$withval" = "yes"; then
            want_boost="yes"
            ax_boost_user_random_lib=""
        else
		    want_boost="yes"
		ax_boost_user_random_lib="$withval"
		fi
        ],
        [want_boost="yes"]
	)

	if test "x$want_boost" = "xyes"; then
        AC_REQUIRE([AC_PROG_CC])
        AC_REQUIRE([AC_CANONICAL_BUILD])
		CPPFLAGS_SAVED="$CPPFLAGS"
		CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
		export CPPFLAGS

		LDFLAGS_SAVED="$LDFLAGS"
		LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"
		export LDFLAGS

		LIBS_SAVED="$LIBS"
		LIBS="$LIBS $BOOST_SYSTEM_LIB $BOOST_THREAD_LIB"
		export LIBS

        AC_CACHE_CHECK(whether the Boost::Random library is available,
					   ax_cv_boost_random,
        [AC_LANG_PUSH([C++])
			 CXXFLAGS_SAVE=$CXXFLAGS

			 AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[@%:@include <boost/random.hpp>]],
                                   [[boost::random::mt19937 rng;]])],
                   ax_cv_boost_random=yes, ax_cv_boost_random=no)
			 CXXFLAGS=$CXXFLAGS_SAVE
             AC_LANG_POP([C++])
		])
		if test "x$ax_cv_boost_random" = "xyes"; then
			AC_SUBST(BOOST_CPPFLAGS)

			AC_DEFINE(HAVE_BOOST_RANDOM,,[define if the Boost::Random library is available])
            BOOSTLIBDIR=`echo $BOOST_LDFLAGS | sed -e 's/@<:@^\/@:>@*//'`

			LDFLAGS_SAVE=$LDFLAGS
            if test "x$ax_boost_user_random_lib" = "x"; then
                for libextension in `ls $BOOSTLIBDIR/libboost_random*.so* $BOOSTLIBDIR/libboost_random*.dylib* $BOOSTLIBDIR/libboost_random*.a* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^lib\(boost_random.*\)\.so.*$;\1;' -e 's;^lib\(boost_random.*\)\.dylib.*$;\1;' -e 's;^lib\(boost_random.*\)\.a.*$;\1;'` ; do
                     ax_lib=${libextension}
				    AC_CHECK_LIB($ax_lib, exit,
                                 [BOOST_RANDOM_LIB="-l$ax_lib"; AC_SUBST(BOOST_RANDOM_LIB) link_random="yes"; break],
                                 [link_random="no"])
				done
                if test "x$link_random" != "xyes"; then
                for libextension in `ls $BOOSTLIBDIR/boost_random*.dll* $BOOSTLIBDIR/boost_random*.a* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^\(boost_random.*\)\.dll.*$;\1;' -e 's;^\(boost_random.*\)\.a.*$;\1;'` ; do
                     ax_lib=${libextension}
				    AC_CHECK_LIB($ax_lib, exit,
                                 [BOOST_RANDOM_LIB="-l$ax_lib"; AC_SUBST(BOOST_RANDOM_LIB) link_random="yes"; break],
                                 [link_random="no"])
				done
                fi

            else
               for ax_lib in $ax_boost_user_random_lib boost_random-$ax_boost_user_random_lib; do
				      AC_CHECK_LIB($ax_lib, exit,
                                   [BOOST_RANDOM_LIB="-l$ax_lib"; AC_SUBST(BOOST_RANDOM_LIB) link_random="yes"; break],
                                   [link_random="no"])
                  done

            fi
            if test "x$ax_lib" = "x"; then
                AC_MSG_ERROR([could not find a version of the library])
            fi
			if test "x$link_random" = "xno"; then
				AC_MSG_ERROR([could not link against "$ax_lib"])
			fi
		fi

		CPPFLAGS="$CPPFLAGS_SAVED"
	LDFLAGS="$LDFLAGS_SAVED"
	LIBS="$LIBS_SAVED"
	fi
])
