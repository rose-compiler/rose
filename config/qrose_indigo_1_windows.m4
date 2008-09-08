AC_DEFUN([AC_CHECK_WINDOWS],
[
AC_REQUIRE([AC_CANONICAL_HOST])
AC_MSG_CHECKING([for windows])
USING_MINGW32=0
if test "$host_os" = cygwin; then   
   USING_WINDOWS=1
   AC_MSG_RESULT([yes: cygwin. Use --build=mingw32 for dll library!])
else
   if test "$host_os" = mingw32; then
      USING_WINDOWS=1
      USING_MINGW32=1
      AC_MSG_RESULT([yes: mingw32])
   else   
      AC_MSG_RESULT([no: $host_os])
      USING_WINDOWS=0
   fi
fi

AC_SUBST(USING_WINDOWS)
AM_CONDITIONAL(QROSE_USE_MINGW32, [test $USING_MINGW32 = 1])
AM_CONDITIONAL(QROSE_USE_WINDOWS, [test $USING_WINDOWS = 1])
])



