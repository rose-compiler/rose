AC_DEFUN([ROSE_SUPPORT_PURIFY],
[
# Begin macro ROSE_SUPPORT_PURIFY.

# We may be asked to guess the correct flags,
# so we must know our host and the compiler used.
AC_REQUIRE([AC_CANONICAL_HOST])
AC_REQUIRE([AC_PROG_CXX])
# DQ (9/26/2015): Making this code more portable with out warnings.
# AC_REQUIRE([BTNG_INFO_CXX_ID])
# AC_REQUIRE([BTNG_CXX_AR])

dnl *********************************************************************
dnl * Use PURIFY
dnl *********************************************************************
AC_ARG_ENABLE(purify-api,
  [  --enable-purify-api ....................... Enable purify API in code.])
AC_ARG_ENABLE(purify-linker,
  [  --enable-purify-linker ....................... Augment the linker with purify.])

if test "$enable_purify_api" = yes ; then
  AC_DEFINE([USE_PURIFY],[],[Support for Purify])
  AC_DEFINE([USE_PURIFY_API],[],[Support for Purify API (Compiling with the Purify library).])
fi

dnl DQ added so that EDG can detect use of linker and use the api 
dnl while the rest of ROSE does not.  EDG only supports use of API
dnl not just the linker option.
if test "$enable_purify_linker" = yes ; then
  AC_DEFINE([USE_PURIFY_LINKER],[],[Link-time support for Purify.])
fi



dnl *********************************************************************
dnl * Specify the location of PURIFY
dnl *********************************************************************
# Let user specify where to find purify.
# Specify by --with-purify= or setting purify_PREFIX.
AC_MSG_CHECKING(for purify)
AC_ARG_WITH(purify,
[  --with-purify=PATH	Specify the prefix where purify is installed],
, test "$purify_PREFIX" && with_purify="$purify_PREFIX")
test "$with_purify" && test ! "$with_purify" = no && purify_PREFIX="$with_purify"
AC_MSG_RESULT($purify_PREFIX)
AC_MSG_NOTICE([purify_PREFIX = "$purify_PREFIX"])
if test "$purify_PREFIX" ; then
  purify_INCLUDES="-I$purify_PREFIX"
  purify_LIBS="$purify_PREFIX/purify_stubs.a"
  purify_BIN="$purify_PREFIX/purify"
fi
AC_SUBST(purify_PREFIX)
AC_SUBST(purify_INCLUDES)
AC_SUBST(purify_LIBS)
AC_SUBST(purify_BIN)
AC_MSG_NOTICE([purify_INCLUDES = "$purify_INCLUDES"])
AC_MSG_NOTICE([purify_LIBS     = "$purify_LIBS"])
AC_MSG_NOTICE([purify_BIN      = "$purify_BIN"])



dnl Allows purify windows to be disabled, as it is too much load over the network.
AC_ARG_ENABLE(purify-windows,
  [  --enable-purify-windows ............... turn on use of PURIFY windows option])
test ! "$enable_purify_windows" = yes && purify_windows_option='-windows=no';



dnl *********************************************************************
dnl * Specify options for PURIFY
dnl *********************************************************************
dnl  Added by DQ to support PURIFY (using my favorite options and paths which might be platform specific)
dnl note that we have to use the ${} instead of $() for this to work on LINUX and IBM (other platforms do not seem to case)
dnl  PURIFY_RUN_AT_EXIT="-run-at-exit=\"if %z; then echo \\\"%v: %e errors, %l+%L bytes leaked.\\\"; fi\" "
AC_ARG_WITH(purify-options,
   [  --with-purify-options=ARG ................ manually set PURIFY options to ARG],
PURIFY_OPTIONS=$with_purify_options,
PURIFY_OPTIONS="$purify_windows_option -recursion-depth-limit=40000 -chain-length=24 -first-only=yes -leaks-at-exit=no -inuse-at-exit=no -always-use-cache-dir=yes -cache-dir=${HOME}/tmp -best-effort ${PURIFY_RUN_AT_EXIT}"
)
AC_SUBST(PURIFY_OPTIONS)


# if test "$enable_purify_linker" ; then
#   AUX_LINKER="${purify_BIN} ${PURIFY_OPTIONS}"
# fi
# AC_SUBST(AUX_LINKER)



if test "$enable_purify_linker" = yes ; then
# Setup compiler specific options for using purify
# Different compilers (e.g KCC) require options in different order.
  AUX_LINKER="${purify_BIN} ${PURIFY_OPTIONS}"
  case $CXX in
    g++)
    # See comment below about syntax change for Linux
      CCLD="$AUX_LINKER $CXX"
      CXXLD="$AUX_LINKER $CXX"
      ;;
    KCC | mpKCC)
    # purify specific option required and clear the AUX_LINKER
      CCLD="$CC --link_command_prefix '${purify_BIN} ${PURIFY_OPTIONS}' -o"
      CXXLD="$CXX --link_command_prefix '${purify_BIN} ${PURIFY_OPTIONS}' -o"
      AUX_LINKER=""
      ;;
    CC)
    # No purify specific option required
    # Can't use "$(xxx)" syntax since it only works when build proecess is run on SUNs
    # Linux requires a different syntax: "$xxx" instead.
    # CCLD="$(AUX_LINKER) $(CXX)"
    # CXXLD="$(AUX_LINKER) $(CXX)"
      CCLD="$AUX_LINKER $CXX"
      CXXLD="$AUX_LINKER $CXX"
    ;;
  esac
else
      CCLD="$CC"
# Liao: 2/17/2009, enable CXXLD environment variable to be accepted during configuration,
# especially when roseTranslator is used and it is not ready to be used as a linker
      CXXLD="$CXXLD"
     if test "x$CXXLD" = x; then
         CXXLD="$CXX"
      fi 
# BP : 10/29/2001, the above two lines were originally 
#      CCLD="$(CC)"
#      CXXLD="$(CXX)"
# but on Linux systems this seems to not work
fi

AC_SUBST(AUX_LINKER)
AC_MSG_NOTICE([in macro ROSE SUPPORT INSURE: CC    = "$CC"])
AC_MSG_NOTICE([in macro ROSE SUPPORT PURIFY: CCLD  = "$CCLD"])
AC_MSG_NOTICE([in macro ROSE SUPPORT INSURE: CXX   = "$CXX"])
AC_MSG_NOTICE([in macro ROSE SUPPORT PURIFY: CXXLD = "$CXXLD"])

AC_SUBST(CCLD)
AC_SUBST(CXXLD)

# End macro ROSE_SUPPORT_PURIFY.
]
)







