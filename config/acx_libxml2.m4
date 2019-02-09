dnl
dnl autoconf macro to test for the Gnome libxml2 library.
dnl
dnl Usage:
dnl
dnl    ACX_LIBXML2([ACTION-IF-FOUND], [ACTION-ELSE])
dnl
dnl This macro searches for a working copy of the Gnome Libxml2
dnl XML parsing library. It sets the shell variable acx_libxml2_ok
dnl to 'yes' if found, or 'no' or 'disable' otherwise.
dnl
dnl If found, this macro #defines HAVE_LIBXML2, and sets the
dnl following shell variables:
dnl   *  acx_libxml2_includes = Necessary C/C++ compiler flags
dnl      to specify the path to the libxml2 include files.
dnl   *  acx_libxml2_libs = Necessary libraries (specified as
dnl      flags) required to link against libxml2.
dnl
dnl If LIBXML2_INCLUDES or LIBXML2_LIBS are set in the current
dnl shell environment, these always override any automatically
dnl detected settings.
dnl 

AC_DEFUN([ACX_LIBXML2],
[AC_PREREQ(2.52)
AC_LANG_PUSH([C])

dnl Set this flag to "yes" only if we find valid libxml2 headers and libraries.
acx_libxml2_ok=no

dnl Points to the root of the libxml2 installation tree, or empty if none known
acx_libxml2_root=""

dnl Default search flags for libxml2 header files and libraries
acx_libxml2_includes="${LIBXML2_INCLUDES}"
acx_libxml2_libs="${LIBXML2_LIBS}"

dnl First, check for a user-specified root path
AC_ARG_WITH(libxml2,
  [AC_HELP_STRING([--with-libxml2=yes|no|<dir>],
    [parse XML using libxml2 installed in <dir>])
  ])
case $with_libxml2 in
	yes | "") ;;
	no) acx_libxml2_ok=disable ;; # no libxml2 support
  *) acx_libxml2_root="$with_libxml2" ;;
esac

dnl If user did not explicitly disable libxml2, try to
dnl auto-detect header file and set the shell variables
dnl    acx_found_libxml2_includes, acx_libxml2_includes
dnl
if test x"${acx_libxml2_ok}" != xdisable ; then
  acx_found_libxml2_includes=no
  if test -z "${acx_libxml2_includes}" ; then
    if test -n "${acx_libxml2_root}" ; then
      acx_libxml2_includes="-I${acx_libxml2_root}/include/libxml2"
    else
      acx_libxml2_includes="-I/usr/local/include/libxml2 -I/usr/include/libxml2"
    fi
  fi

  dnl Test
  dnl DQ (6/14/2006): Fix suggested by Jeremiah.
  dnl save_CFLAGS="${CFLAGS}"
  dnl CFLAGS="${acx_libxml2_includes} ${CFLAGS}"
  save_CPPFLAGS="${CPPFLAGS}"
  CPPFLAGS="${acx_libxml2_includes} ${CPPFLAGS}"
  AC_CHECK_HEADER([libxml/parser.h], [acx_found_libxml2_includes=yes])

  dnl DQ (6/14/2006): Fix suggested by Jeremiah.
  dnl CFLAGS="${save_CFLAGS}"
  CPPFLAGS="${save_CPPFLAGS}"

  dnl Check test results
  if test x"${acx_found_libxml2_includes}" != xyes ; then
    AC_MSG_NOTICE([could not find a valid libxml2 header])
    acx_libxml2_includes=""
    acx_libxml2_ok=disable
  fi
fi

dnl If user did not explicitly disable libxml2 and the
dnl previous header-check passed, auto-detect the library
dnl and set the shell variables
dnl    acx_found_libxml2_libs, acx_libxml2_libs
dnl
if test x"${acx_libxml2_ok}" != xdisable ; then
  acx_found_libxml2_libs=no
  if test -z "${acx_libxml2_libs}" ; then
    if test -n "${acx_libxml2_root}" ; then
      acx_libxml2_libs="-L${acx_libxml2_root}/lib -lxml2"
    else
      acx_libxml2_libs="-L/usr/lib -lxml2"
    fi
  fi

  dnl Name of some libxml2 routine to test for:
  libxml2func=xmlSAXUserParseFile

  dnl Test
  save_LIBS="${LIBS}"
  LIBS="${acx_libxml2_libs} ${LIBS}"
  AC_MSG_CHECKING([for ${libxml2func} in ${acx_libxml2_libs}])
  AC_TRY_LINK_FUNC(${libxml2func},
      [acx_found_libxml2_libs=yes], [acx_found_libxml2_libs=no])
  AC_MSG_RESULT(${acx_found_libxml2_libs})
  LIBS="$save_LIBS"

  dnl Check test results
  if test x"${acx_found_libxml2_libs}" != xyes ; then
    AC_MSG_NOTICE([could not find a libxml2 library to link against])
    acx_libxml2_libs=""
    acx_libxml2_ok=disable
  else
    acx_libxml2_ok=yes
  fi
fi

if test x"$acx_libxml2_ok" = xyes; then
  AC_MSG_NOTICE([found a seemingly valid libxml2 installation])
  AC_DEFINE(HAVE_LIBXML2, 1, [Define if you have libxml2.])
	:
  $1
else
	acx_libxml2_ok=no
	$2
fi

AC_LANG_POP([C])
]) dnl ACX_LIBXML2

dnl eof
