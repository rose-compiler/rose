

AC_DEFUN([AX_CLOOG],
[
  AC_ARG_WITH(cloog, AS_HELP_STRING([--with-cloog=DIR], [specify the path to the Cloog development files]), cloog_prefix="$withval", cloog_prefix="")
  AC_ARG_WITH(cloog-libdir, AS_HELP_STRING([--with-cloog-libdir=DIR], [specify the path to the Cloog library files]),
    [
      if [ -d $withval ] ; then
        cloog_libdir="$withval"
      else
        AC_MSG_ERROR(--with-cloog-libdir requires a valid directory path)
      fi
    ],
    cloog_libdir="")
  AC_ARG_WITH(cloog-incdir, AS_HELP_STRING([--with-cloog-incdir=DIR], [specify the path to the Cloog include files]),
    [
      if [ -d $withval ] ; then
        cloog_incdir="$withval"
      else
        AC_MSG_ERROR(--with-cloog-incdir requires a valid directory path)
      fi
    ],
    cloog_incdir="")
  
  AC_MSG_CHECKING([for Cloog])
  
  CLOOG_CFLAGS=""
  CLOOG_CXXFLAGS=""
  CLOOG_LDFLAGS="-lcloog-polylib"
  
  if test "$cloog_prefix" != "" ; then
    CLOOG_CFLAGS="$CLOOG_CFLAGS -I$cloog_prefix/include $PIPLIB_CXXFLAGS"
    CLOOG_LDFLAGS="$CLOOG_LDFLAGS -L$cloog_prefix/lib"
  fi
  
  if test "$cloog_libdir" != "" ; then
    CLOOG_LDFLAGS="$CLOOG_LDFLAGS -L$cloog_libdir"
  fi
  
  CFLAGS_OLD="$CFLAGS"
  CFLAGS="$CFLAGS $CLOOG_CFLAGS"
  export CFLAGS
  
  LDFLAGS_OLD="$LDFLAGS"
  LDFLAGS="$LDFLAGS $CLOOG_LDFLAGS"
  export LDFLAGS
  
  AC_LANG_PUSH([C])
  
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
      @%:@include <stdio.h>
      @%:@include <cloog/cloog.h>
    ]], [[

    ]])], [AC_MSG_RESULT(yes)], 
    [
      AC_MSG_RESULT(not found)
      $2
    ])
  AC_LANG_POP([C])
  
  CFLAGS="$CFLAGS_OLD"
  LDFLAGS="$LDFLAGS_OLD"
  
  CLOOG_CXXFLAGS="$CLOOG_CFLAGS"
  
  AC_SUBST(CLOOG_CFLAGS)
  AC_SUBST(CLOOG_CXXFLAGS)
  AC_SUBST(CLOOG_LDFLAGS)
])

