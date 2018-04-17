AC_DEFUN([ROSE_SUPPORT_SANITIZER],
[
# Begin macro ROSE_SUPPORT_SANITIZER (Google project: Adress Sanitizer, Memory Sanitizer, etc.).

# We may be asked to guess the correct flags,
# so we must know our host and the compiler used.
AC_REQUIRE([AC_CANONICAL_HOST])
AC_REQUIRE([AC_PROG_CXX])

dnl *********************************************************************
dnl * Set the sanitizer tool compiler flags in CXXFLAGS and CFLAGS
dnl *********************************************************************

echo "Setup CXXFLAGS and CFLAGS for CXX = $CXX"

AC_ARG_ENABLE(sanitizer, AS_HELP_STRING([--enable-sanitizer], [Support address sanitizer and related sanitizer tools (compiler options) for ROSE development]),[enableval=yes],[enableval=no])

echo "enableval = $enableval"
echo "enable_sanitizer = $enable_sanitizer"

if test "x$enable_sanitizer" = "x"; then
   if test "x$enableval" = "xyes"; then
      enable_sanitizer=$enableval
   else 
      if test "x$enableval" = "xno"; then
         enable_sanitizer=$enableval
      fi
   fi
else
   echo "enable_sanitizer is explictly set to: $enable_sanitizer"
fi

echo "After initialization: enable_sanitizer = $enable_sanitizer"

if test "x$enable_sanitizer" = "xyes"; then
# enable_sanitizer was activated but not specified, so set it.
  echo "Using default options for maximal warnings (true case)"
  case $CXX in
    g++)
    # For GNU the support for sanitizer captured using a single options (I think).
      CXXFLAGS+=" -fsanitize=address"
      CFLAGS+=" -fsanitize=address"
      ;;
    clang)
    # For LLVM the support for sanitizer is the best (but add this later).
      echo "Clang compilers do support sanitizer tools (but support in ROSE not implemented)."
      ;;
    icpc)
    # For Intel turn on 64bit migration/portability warnings
      echo "Intel compilers don't support sanitizer tools."
      ;;
  esac
fi

# The strings used to setup the environment are specificed in config/Makefile.for.ROSE.includes.and.libs
# AC_SUBST(CXX_WARNINGS)

# This is where we could set up the use of a prefix to CC and CXX if that is a better implementation in the future.
# if test "$CXXFLAGS"; then CXXFLAGS="$CXXFLAGS $CXX_WARNINGS"; fi

AM_CONDITIONAL(ROSE_USE_SANITIZER,test "$enable_sanitizer" = yes)

echo "debugging: CXXFLAGS = $CXXFLAGS"
echo "debugging: CFLAGS   = $CFLAGS"

# exit 1

# End macro ROSE_SUPPORT_SANITIZER.
]
)





