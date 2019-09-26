AC_DEFUN([ROSE_SUPPORT_LIBFFI],
[

# DQ (9/21/2009): Added pkg.m4 to local ROSE/aclocal
# DQ (9/19/2009): This fails on OSX (ninjai.llnl.gov) (added by Peter to support ROSE interpreter).
PKG_CHECK_MODULES([LIBFFI],[libffi],[with_libffi=yes],[with_libffi=no])

# DQ (9/19/2009): This fails on Mac OSX (this variable is needed
# for an automake conditional that will be built in configure.in)
# with_libffi=no
save_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $LIBFFI_CFLAGS"
AC_CHECK_HEADERS(ffi.h,[with_ffi_header=yes],[with_ffi_header=no])
CPPFLAGS="$save_CPPFLAGS"

AC_MSG_NOTICE([intermediate test debugging info for libffi support:])
AC_MSG_NOTICE([   with_libffi     = "$with_libffi"])
AC_MSG_NOTICE([   with_ffi_header = "$with_ffi_header"])
AC_MSG_NOTICE([   LIBFFI_LIBS     = "$LIBFFI_LIBS"])
AC_MSG_NOTICE([   LIBFFI_CFLAGS   = "$LIBFFI_CFLAGS"])

if [test ! "$with_libffi" = no]; then
   if [test ! "$with_ffi_header" = yes]; then
      AC_MSG_WARN([libffi found, but ffi.h header file not found (reset with_libffi to disable LIBFFI support)])
      with_ffi_header=no
      with_libffi=no
   fi
fi

AC_MSG_NOTICE([debugging info for libffi support:])
AC_MSG_NOTICE([   with_libffi     = "$with_libffi"])
AC_MSG_NOTICE([   with_ffi_header = "$with_ffi_header"])
AC_MSG_NOTICE([   LIBFFI_LIBS     = "$LIBFFI_LIBS"])
AC_MSG_NOTICE([   LIBFFI_CFLAGS   = $LIBFFI_CFLAGS"])

]
)
