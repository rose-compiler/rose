AC_DEFUN([ROSE_SUPPORT_LCOV],
[
# Begin macro ROSE_SUPPORT_LCOV (Google project: Adress Sanitizer, Memory Sanitizer, etc.).

# We may be asked to guess the correct flags,
# so we must know our host and the compiler used.
AC_REQUIRE([AC_CANONICAL_HOST])
AC_REQUIRE([AC_PROG_CXX])

dnl *********************************************************************
dnl * Set the lcov tool compiler flags in CXXFLAGS and CFLAGS
dnl *********************************************************************

AC_MSG_NOTICE([setup CXXFLAGS and CFLAGS for CXX = "$CXX"])

AC_ARG_ENABLE(lcov, AS_HELP_STRING([--enable-lcov], [Support Linux coverage test tools (compiler options) for ROSE development]),[enableval=yes],[enableval=no])

AC_MSG_NOTICE([enable_lcov = "$enable_lcov"])

if test "x$enable_lcov" = "x"; then
   if test "x$enableval" = "xyes"; then
      enable_lcov=$enableval
   else 
      if test "x$enableval" = "xno"; then
         enable_lcov=$enableval
      fi
   fi
else
   AC_MSG_NOTICE([enable_lcov is explictly set to: "$enable_lcov"])
fi

AC_MSG_NOTICE([after initialization: enable_lcov = "$enable_lcov"])

if test "x$enable_lcov" = "xyes"; then
# enable_lcov was activated but not specified, so set it.
  AC_MSG_NOTICE([using default options for maximal warnings (true case)])
  case $CXX in
    g++)
      CXXFLAGS+=" -g -fprofile-arcs -ftest-coverage"
      CFLAGS+=" -g -fprofile-arcs -ftest-coverage"
      ;;
    clang)
    # For LLVM the support for lcov .
      AC_MSG_NOTICE([Clang compilers do support lcov tools])
      ;;
    icpc)
    # For Intel turn on 64bit migration/portability warnings
      AC_MSG_NOTICE([Intel compilers don't support lcov tools])
      ;;
  esac
fi

# The strings used to setup the environment are specificed in config/Makefile.for.ROSE.includes.and.libs
# AC_SUBST(CXX_WARNINGS)

# This is where we could set up the use of a prefix to CC and CXX if that is a better implementation in the future.
# if test "$CXXFLAGS"; then CXXFLAGS="$CXXFLAGS $CXX_WARNINGS"; fi

AM_CONDITIONAL(ROSE_USE_LCOV,test "$enable_lcov" = yes)

AC_MSG_NOTICE([debugging: CXXFLAGS = "$CXXFLAGS"])
AC_MSG_NOTICE([debugging: CFLAGS   = "$CFLAGS"])

# exit 1

# End macro ROSE_SUPPORT_LCOV.
]
)





