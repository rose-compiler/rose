##### http://autoconf-archive.cryp.to/ax_boost_iostreams.html
#
# SYNOPSIS
#
#   AX_BOOST_IOSTREAMS
#
# DESCRIPTION
#
#   Test for IOStreams library from the Boost C++ libraries. The macro
#   requires a preceding call to AX_BOOST_BASE. Further documentation
#   is available at <http://randspringer.de/boost/index.html>.
#
#   This macro calls:
#
#     AC_SUBST(BOOST_IOSTREAMS_LIB)
#
#   And sets:
#
#     HAVE_BOOST_IOSTREAMS
#
# COPYLEFT
#
#   Copyright (c) 2007 Thomas Porschberg <thomas@randspringer.de>
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.
#
# MODIFIED
#  Robb Matzke, 2015-01: fixed indentation
#  Robb Matzke, 2015-01: fixed message about "library not found" when it's the headers that aren't found
#  Robb Matzke, 2015-01: it is not an error for the iostreams library to be missing when headers are present
#  Robb Matzke, 2015-01: replaced test .C file with mapped_file_params since that's what ROSE needs from iostreams

AC_DEFUN([AX_BOOST_IOSTREAMS],
[
    AC_ARG_WITH([boost-iostreams],
        AS_HELP_STRING([--with-boost-iostreams@<:@=special-lib@:>@],
            [use the IOStreams library from boost - it is possible to specify a certain library for the linker
                        e.g. --with-boost-iostreams=boost_iostreams-gcc-mt-d-1_33_1 ]),
        [
            if test "$withval" = "no"; then
                want_boost="no"
            elif test "$withval" = "yes"; then
                want_boost="yes"
                ax_boost_user_iostreams_lib=""
            else
                want_boost="yes"
                ax_boost_user_iostreams_lib="$withval"
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

        AC_CACHE_CHECK([whether the boost::iostreams headers are available],
            ax_cv_boost_iostreams,
            [
                AC_LANG_PUSH([C++])
                AC_COMPILE_IFELSE(
                    [AC_LANG_PROGRAM(
                        [[
                            @%:@include <boost/iostreams/device/mapped_file.hpp>
                        ]],
                        [[
                            boost::iostreams::mapped_file_params params;
                            boost::iostreams::mapped_file f(params);
                            f.const_data();
                            return 0;
                        ]])],
                    ax_cv_boost_iostreams=yes,
                    ax_cv_boost_iostreams=no)
                AC_LANG_POP([C++])
            ])


        if test "x$ax_cv_boost_iostreams" = "xyes"; then
            AC_DEFINE(HAVE_BOOST_IOSTREAMS,,[define if the Boost::IOStreams library is available])
            BOOSTLIBDIR=`echo $BOOST_LDFLAGS | sed -e 's/@<:@^\/@:>@*//'`
            if test "x$ax_boost_user_iostreams_lib" = "x"; then
                for libextension in `ls $BOOSTLIBDIR/libboost_iostreams*.{so,dylib,a}* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^lib\(boost_iostreams.*\)\.so.*$;\1;' -e 's;^lib\(boost_iostreams.*\)\.a*$;\1;' -e 's;^lib\(boost_iostreams.*\)\.dylib$;\1;'` ; do
                    ax_lib=${libextension}
                    AC_CHECK_LIB($ax_lib, toupper,
                        [BOOST_IOSTREAMS_LIB="-l$ax_lib"; AC_SUBST(BOOST_IOSTREAMS_LIB) link_iostreams="yes"; break],
                        [link_iostreams="no"])
                done

                if test "x$link_iostreams" != "xyes"; then
                    for libextension in `ls $BOOSTLIBDIR/boost_iostreams*.{dll,a}* 2>/dev/null | sed 's,.*/,,' | sed -e 's;^\(boost_iostreams.*\)\.dll.*$;\1;' -e 's;^\(boost_iostreams.*\)\.a*$;\1;'` ; do
                        ax_lib=${libextension}
                        AC_CHECK_LIB($ax_lib, toupper,
                            [BOOST_IOSTREAMS_LIB="-l$ax_lib"; AC_SUBST(BOOST_IOSTREAMS_LIB) link_iostreams="yes"; break],
                            [link_iostreams="no"])
                    done
                fi
            else
                for ax_lib in $ax_boost_user_iostreams_lib boost_iostreams-$ax_boost_user_iostreams_lib; do
                    AC_CHECK_LIB($ax_lib, main,
                        [BOOST_IOSTREAMS_LIB="-l$ax_lib"; AC_SUBST(BOOST_IOSTREAMS_LIB) link_iostreams="yes"; break],
                        [link_iostreams="no"])
                done
            fi
            
            dnl Failing to link against libiostreams should not be an error here; perhaps at a higher level it is.
            if test "x$link_iostreams" != "xyes"; then
                AC_MSG_NOTICE([boost::iostreams headers are available, but not the library])
            fi
        fi

        CPPFLAGS="$CPPFLAGS_SAVED"
        LDFLAGS="$LDFLAGS_SAVED"
    fi
])
