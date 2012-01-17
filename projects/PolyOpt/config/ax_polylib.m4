

AC_DEFUN([AX_POLYLIB],
[
  AC_ARG_WITH(polylib, AS_HELP_STRING([--with-polylib=DIR], [specify the path to the PolyLib development files]), polylib_prefix="$withval", polylib_prefix="")
  AC_ARG_WITH(polylib-libdir, AS_HELP_STRING([--with-polylib-libdir=DIR], [specify the path to the Clan library files]),
    [
      if [ -d $withval ] ; then
        polylib_libdir="$withval"
      else
        AC_MSG_ERROR(--with-polylib-libdir requires a valid directory path)
      fi
    ],
    polylib_libdir="")
  AC_ARG_WITH(polylib-incdir, AS_HELP_STRING([--with-polylib-incdir=DIR], [specify the path to the Clan include files]),
    [
      if [ -d $withval ] ; then
        polylib_incdir="$withval"
      else
        AC_MSG_ERROR(--with-polylib-incdir requires a valid directory path)
      fi
    ],
    polylib_incdir="")
  
  AC_MSG_CHECKING([for PolyLib])
  
  POLYLIB_CFLAGS=""
  POLYLIB_CXXFLAGS=""
  POLYLIB_LDFLAGS="-lpolylib64"
  
  if test "$polylib_prefix" != "" ; then
    POLYLIB_CFLAGS="$POLYLIB_CFLAGS -I$polylib_prefix/include"
    POLYLIB_LDFLAGS="$POLYLIB_LDFLAGS -L$polylib_prefix/lib"
  fi
  
  if test "$polylib_libdir" != "" ; then
    POLYLIB_LDFLAGS="$POLYLIB_LDFLAGS -L$polylib_libdir"
  fi
  
  CFLAGS_OLD="$CFLAGS"
  CFLAGS="$CFLAGS $POLYLIB_CFLAGS"
  export CFLAGS
  
  LDFLAGS_OLD="$LDFLAGS"
  LDFLAGS="$LDFLAGS $POLYLIB_LDFLAGS"
  export LDFLAGS
  
  AC_LANG_PUSH([C])
  
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
      @%:@include <stdio.h>
      @%:@include <polylib/polylib64.h>
    ]], [[
    
    ]])], [AC_MSG_RESULT(yes)], 
    [
      AC_MSG_RESULT(not found)
      $2
    ])
  AC_LANG_POP([C])
  
  CFLAGS="$CFLAGS_OLD"
  LDFLAGS="$LDFLAGS_OLD"
  
  POLYLIB_CXXFLAGS="$POLYLIB_CFLAGS"
  
  AC_SUBST(POLYLIB_CFLAGS)
  AC_SUBST(POLYLIB_CXXFLAGS)
  AC_SUBST(POLYLIB_LDFLAGS)
])

