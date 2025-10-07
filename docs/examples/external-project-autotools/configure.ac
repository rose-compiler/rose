AC_INIT([rose-external-example], [1.0], [support@example.com])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_PROG_CXX
AC_PROG_CC
AC_LANG([C++])

# Require C++14 or later
# Note: We manually check for C++14 support instead of using AX_CXX_COMPILE_STDCXX
# from autoconf-archive to avoid requiring an external dependency. This makes the
# example more portable and easier to use without requiring autoconf-archive to be
# installed on the build system.
m4_define([_AX_CXX_COMPILE_STDCXX_14_testbody], [
  AC_LANG_SOURCE([int main() { return 0; }])
])

AC_MSG_CHECKING([for C++14 support])
CXX14_FLAG=""
for flag in -std=c++14 -std=c++1y; do
  save_CXXFLAGS="$CXXFLAGS"
  CXXFLAGS="$CXXFLAGS $flag"
  AC_COMPILE_IFELSE([_AX_CXX_COMPILE_STDCXX_14_testbody],
    [CXX14_FLAG="$flag"; break],
    [])
  CXXFLAGS="$save_CXXFLAGS"
done

if test "x$CXX14_FLAG" = "x"; then
  AC_MSG_RESULT([no])
  AC_MSG_ERROR([C++14 support is required but not found])
else
  AC_MSG_RESULT([yes, using $CXX14_FLAG])
  CXXFLAGS="$CXXFLAGS $CXX14_FLAG"
fi

# Check for ROSE using pkg-config
PKG_CHECK_MODULES([ROSE], [rose >= 0.11], [
    have_rose=yes
    AC_DEFINE([HAVE_ROSE], [1], [Define if ROSE is available])
], [
    have_rose=no
    AC_MSG_ERROR([ROSE not found. Please install ROSE and set PKG_CONFIG_PATH.])
])

# Display ROSE information
AC_MSG_NOTICE([ROSE compiler flags: $ROSE_CFLAGS])
AC_MSG_NOTICE([ROSE linker flags: $ROSE_LIBS])

# Create Makefiles
AC_CONFIG_FILES([
    Makefile
])

AC_OUTPUT

# Summary
echo ""
echo "Configuration summary:"
echo "  ROSE:            $have_rose"
echo "  CXX:             $CXX"
echo "  ROSE_CFLAGS:     $ROSE_CFLAGS"
echo "  ROSE_LIBS:       $ROSE_LIBS"
echo ""
echo "Run 'make' to build the examples."
echo ""
