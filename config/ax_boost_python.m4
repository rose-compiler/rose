# Copied and modified from ax_boost_chrono with ROSE-specific changes.
#
# SYNOPSIS
#
#   AX_BOOST_PYTHON
#
# DESCRIPTION
#
#   Test for Python library from the Boost C++ libraries. The macro requires
#   a preceding call to AX_BOOST_BASE. Further documentation is available at
#   <http://randspringer.de/boost/index.html>.
#
#   This macro calls:
#
#     AC_SUBST(BOOST_PYTHON_LIB)
#
#   And sets:
#
#     HAVE_BOOST_PYTHON
#
# LICENSE
#
#   Copyright (c) 2012 Xiyue Deng <manphiz@gmail.com>
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 3

AC_DEFUN([AX_BOOST_PYTHON],
[
    AC_ARG_WITH([boost-python],
    AS_HELP_STRING([--with-boost-python@<:@=special-lib@:>@],
        [use the Python library from boost - it is possible to specify a certain library for the linker
         e.g. --with-boost-python=boost_python-gcc-mt ]),
        [
            if test "$withval" = "no"; then
                want_boost="no"
            elif test "$withval" = "yes"; then
                want_boost="yes"
                ax_boost_user_python_lib=""
            else
                want_boost="yes"
                ax_boost_user_python_lib="$withval"
            fi
        ],
        [want_boost="yes"])

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

        AC_CACHE_CHECK([whether the Boost::Python library is available], ax_cv_boost_python,
            [
                AC_LANG_PUSH([C++])
                    CXXFLAGS_SAVE=$CXXFLAGS

                    AC_COMPILE_IFELSE(
                        [AC_LANG_PROGRAM([[@%:@include <boost/python.hpp>]], [[int dummy;]])],
                        ax_cv_boost_python=yes,
                        ax_cv_boost_python=no)
                    CXXFLAGS=$CXXFLAGS_SAVE
                AC_LANG_POP([C++])
            ])

        if test "x$ax_cv_boost_python" = "xyes"; then
            AC_SUBST(BOOST_CPPFLAGS)

            AC_DEFINE(HAVE_BOOST_PYTHON,,[define if the Boost::Python library is available])
            BOOSTLIBDIR=`echo $BOOST_LDFLAGS | sed -e 's/@<:@^\/@:>@*//'`

            LDFLAGS_SAVE=$LDFLAGS
            if test "x$ax_boost_user_python_lib" = "x"; then
                for libextension in `ls $BOOSTLIBDIR/libboost_python*.so* $BOOSTLIBDIR/libboost_python*.dylib* $BOOSTLIBDIR/libboost_python*.a* 2>/dev/null |\
                                     sed 's,.*/,,' |\
                                     sed -e 's;^lib\(boost_python.*\)\.so.*$;\1;' -e 's;^lib\(boost_python.*\)\.dylib.*$;\1;' -e 's;^lib\(boost_python.*\)\.a.*$;\1;'` ; do
                    ax_lib=${libextension}
                    AC_CHECK_LIB($ax_lib, exit,
                        [BOOST_PYTHON_LIB="-l$ax_lib"; AC_SUBST(BOOST_PYTHON_LIB) link_python="yes"; break],
                        [link_python="no"])
                done

                if test "x$link_python" != "xyes"; then
                    for libextension in `ls $BOOSTLIBDIR/boost_python*.dll* $BOOSTLIBDIR/boost_python*.a* 2>/dev/null |\
                                         sed 's,.*/,,' |\
                                         sed -e 's;^\(boost_python.*\)\.dll.*$;\1;' -e 's;^\(boost_python.*\)\.a.*$;\1;'` ; do
                        ax_lib=${libextension}
                        AC_CHECK_LIB($ax_lib, exit,
                            [BOOST_PYTHON_LIB="-l$ax_lib"; AC_SUBST(BOOST_PYTHON_LIB) link_python="yes"; break],
                            [link_python="no"])
                    done
                fi
            else
                for ax_lib in $ax_boost_user_python_lib boost_python-$ax_boost_user_python_lib; do
                    AC_CHECK_LIB($ax_lib, exit,
                        [BOOST_PYTHON_LIB="-l$ax_lib"; AC_SUBST(BOOST_PYTHON_LIB) link_python="yes"; break],
                        [link_python="no"])
                done
            fi

            if test "x$ax_lib" = "x"; then
                AC_MSG_ERROR([could not find a version of the library])
            fi

            if test "x$link_python" = "xno"; then
                AC_MSG_ERROR([could not link against "$ax_lib"])
            fi
        fi

        CPPFLAGS="$CPPFLAGS_SAVED"
        LDFLAGS="$LDFLAGS_SAVED"
        LIBS="$LIBS_SAVED"
    fi
])
