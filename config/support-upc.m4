AC_DEFUN([ROSE_SUPPORT_UPC],
[
# Begin macro ROSE_SUPPORT_UPC.
# Automake-UPC Support http://www.gnu.org/software/hello/manual/automake/Unified-Parallel-C-Support.html
# Handling new file extensions: https://idlebox.net/2009/apidocs/automake-1.11.zip/automake_18.html#SEC148

AC_MSG_CHECKING([for UPC])
AC_ARG_ENABLE([upc],AS_HELP_STRING([--enable-upc],[Enable UPC support in ROSE (default=no)]),,[enableval=no])
AM_CONDITIONAL(ROSE_WITH_UPC, [test "x$enableval" = "xyes"])
#if test "x$enableval" = "xyes" ; then
  AM_PROG_UPC
#fi
AC_MSG_RESULT([$enableval])
])
