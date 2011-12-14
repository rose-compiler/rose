

AC_DEFUN([AX_CLAN],
[
  AC_ARG_WITH(clan, AS_HELP_STRING([--with-clan=DIR], [specify the path to the Clan development files]), clan_prefix="$withval", clan_prefix="")
  AC_ARG_WITH(clan-libdir, AS_HELP_STRING([--with-clan-libdir=DIR], [specify the path to the Clan library files]),
    [
      if [ -d $withval ] ; then
        clan_libdir="$withval"
      else
        AC_MSG_ERROR(--with-clan-libdir requires a valid directory path)
      fi
    ],
    clan_libdir="")
  AC_ARG_WITH(clan-incdir, AS_HELP_STRING([--with-clan-incdir=DIR], [specify the path to the Clan include files]),
    [
      if [ -d $withval ] ; then
        clan_incdir="$withval"
      else
        AC_MSG_ERROR(--with-clan-incdir requires a valid directory path)
      fi
    ],
    clan_incdir="")
  
  AC_MSG_CHECKING([for Clan])
  
  CLAN_CFLAGS=""
  CLAN_CXXFLAGS=""
  CLAN_LDFLAGS="-lclan"
  
  if test "$clan_prefix" != "" ; then
    CLAN_CFLAGS="$CLAN_CFLAGS -I$clan_prefix/include"
    CLAN_LDFLAGS="$CLAN_LDFLAGS -L$clan_prefix/lib"
  fi
  
  if test "$clan_libdir" != "" ; then
    CLAN_LDFLAGS="$CLAN_LDFLAGS -L$clan_libdir"
  fi
  
  CFLAGS_OLD="$CFLAGS"
  CFLAGS="$CFLAGS $CLAN_CFLAGS"
  export CFLAGS
  
  LDFLAGS_OLD="$LDFLAGS"
  LDFLAGS="$LDFLAGS $CLAN_LDFLAGS"
  export LDFLAGS
  
  AC_LANG_PUSH([C])
  
  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
      @%:@include <clan/clan.h>
    ]], [[
      clan_scop_p scop;
    ]])], [AC_MSG_RESULT(yes)], 
    [
      AC_MSG_RESULT(not found)
      $2
    ])
  AC_LANG_POP([C])
  
  CFLAGS="$CFLAGS_OLD"
  LDFLAGS="$LDFLAGS_OLD"
  
  CLAN_CXXFLAGS="$CLAN_CFLAGS"
  
  AC_SUBST(CLAN_CFLAGS)
  AC_SUBST(CLAN_CXXFLAGS)
  AC_SUBST(CLAN_LDFLAGS)
])

