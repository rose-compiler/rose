

AC_DEFUN([AX_CANDL],
[
  AC_ARG_WITH(candl, AS_HELP_STRING([--with-candl=DIR], [specify the path to the Candl development files]), candl_prefix="$withval", candl_prefix="")
  AC_ARG_WITH(candl-libdir, AS_HELP_STRING([--with-candl-libdir=DIR], [specify the path to the Candl library files]),
    [
      if [ -d $withval ] ; then
        candl_libdir="$withval"
      else
        AC_MSG_ERROR(--with-candl-libdir requires a valid directory path)
      fi
    ],
    candl_libdir="")
  AC_ARG_WITH(candl-incdir, AS_HELP_STRING([--with-candl-incdir=DIR], [specify the path to the Candl include files]),
    [
      if [ -d $withval ] ; then
        candl_incdir="$withval"
      else
        AC_MSG_ERROR(--with-candl-incdir requires a valid directory path)
      fi
    ],
    candl_incdir="")
  
  AC_MSG_CHECKING([for Candl])
  
  CANDL_CFLAGS=""
  CANDL_CXXFLAGS=""
  CANDL_LDFLAGS="-lcandl"
  
  if test "$candl_prefix" != "" ; then
    CANDL_CFLAGS="$CANDL_CFLAGS -I$candl_prefix/include $PIPLIB_CXXFLAGS $CLAN_CXXFLAGS"
    CANDL_LDFLAGS="$CANDL_LDFLAGS -L$candl_prefix/lib"
  fi
  
  if test "$candl_libdir" != "" ; then
    CANDL_LDFLAGS="$CANDL_LDFLAGS -L$candl_libdir"
  fi
  
  CFLAGS_OLD="$CFLAGS"
  CFLAGS="$CFLAGS $CANDL_CFLAGS"
  export CFLAGS
  
  LDFLAGS_OLD="$LDFLAGS"
  LDFLAGS="$LDFLAGS $CANDL_LDFLAGS"
  export LDFLAGS
  
  AC_LANG_PUSH([C])
  
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
      @%:@include <stdio.h>
      @%:@include <candl/candl.h>
    ]], [[

    ]])], [AC_MSG_RESULT(yes)], 
    [
      AC_MSG_RESULT(not found)
      $2
    ])
  AC_LANG_POP([C])
  
  CFLAGS="$CFLAGS_OLD"
  LDFLAGS="$LDFLAGS_OLD"
  
  CANDL_CXXFLAGS="$CANDL_CFLAGS"
  
  AC_SUBST(CANDL_CFLAGS)
  AC_SUBST(CANDL_CXXFLAGS)
  AC_SUBST(CANDL_LDFLAGS)
])

