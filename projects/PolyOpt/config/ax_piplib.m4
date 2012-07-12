

AC_DEFUN([AX_PIPLIB],
[
  AC_ARG_WITH(piplib, AS_HELP_STRING([--with-piplib=DIR], [specify the path to the PipLib development files]), piplib_prefix="$withval", piplib_prefix="")
  AC_ARG_WITH(piplib-libdir, AS_HELP_STRING([--with-piplib-libdir=DIR], [specify the path to the Clan library files]),
    [
      if [ -d $withval ] ; then
        piplib_libdir="$withval"
      else
        AC_MSG_ERROR(--with-piplib-libdir requires a valid directory path)
      fi
    ],
    piplib_libdir="")
  AC_ARG_WITH(piplib-incdir, AS_HELP_STRING([--with-piplib-incdir=DIR], [specify the path to the Clan include files]),
    [
      if [ -d $withval ] ; then
        piplib_incdir="$withval"
      else
        AC_MSG_ERROR(--with-piplib-incdir requires a valid directory path)
      fi
    ],
    piplib_incdir="")
  
  AC_MSG_CHECKING([for PipLib])
  
  PIPLIB_CFLAGS=""
  PIPLIB_CXXFLAGS=""
  PIPLIB_LDFLAGS="-lpiplib64"
  
  if test "$piplib_prefix" != "" ; then
    PIPLIB_CFLAGS="$PIPLIB_CFLAGS -I$piplib_prefix/include"
    PIPLIB_LDFLAGS="$PIPLIB_LDFLAGS -L$piplib_prefix/lib"
  fi
  
  if test "$piplib_libdir" != "" ; then
    PIPLIB_LDFLAGS="$PIPLIB_LDFLAGS -L$piplib_libdir"
  fi
  
  CFLAGS_OLD="$CFLAGS"
  CFLAGS="$CFLAGS $PIPLIB_CFLAGS"
  export CFLAGS
  
  LDFLAGS_OLD="$LDFLAGS"
  LDFLAGS="$LDFLAGS $PIPLIB_LDFLAGS"
  export LDFLAGS
  
  AC_LANG_PUSH([C])
  
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
      @%:@include <stdio.h>
      @%:@include <piplib/piplibMP.h>
    ]], [[
    
    ]])], [AC_MSG_RESULT(yes)], 
    [
      AC_MSG_RESULT(not found)
      $2
    ])
  AC_LANG_POP([C])
  
  CFLAGS="$CFLAGS_OLD"
  LDFLAGS="$LDFLAGS_OLD"
  
  PIPLIB_CXXFLAGS="$PIPLIB_CFLAGS"
  
  AC_SUBST(PIPLIB_CFLAGS)
  AC_SUBST(PIPLIB_CXXFLAGS)
  AC_SUBST(PIPLIB_LDFLAGS)
])

