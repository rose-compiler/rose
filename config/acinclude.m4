dnl
dnl This file contains local autoconf functions (borrowed from PHP).
dnl
dnl echo "${1}..............." | awk '{ printf("%s", substr([$]$0, 0, 15)); }'"

dnl ----------------------------------------------------------------------------
dnl Output stylize macros for configure (help/runtime)
dnl ----------------------------------------------------------------------------

dnl
dnl ROSE_HELP_SEPARATOR(title)
dnl
dnl Adds separator title into the configure --help display.
dnl
AC_DEFUN([ROSE_HELP_SEPARATOR],[
AC_ARG_ENABLE([],[
$1
],[])
])

dnl
dnl ROSE_CONFIGURE_SECTION(title)
dnl
dnl Adds separator title configure output (idea borrowed from mm)
dnl
AC_DEFUN([ROSE_CONFIGURE_SECTION],[
  AC_MSG_RESULT()
  AC_MSG_RESULT([${T_BM}$1${T_NM}])
])

