
AC_DEFUN([ROSE_SUPPORT_EDG],
[

# DQ (2/2/2010): New code to control use of different versions of EDG with ROSE.
AC_ARG_ENABLE(edg-version,
[  --enable-edg_version     major.minor version number for EDG (e.g. 4.9, 4.12, \[5.0\]).],
[])

AC_MSG_NOTICE([enable_edg_version = "$enable_edg_version"])
if test "x$enable_edg_version" = "x"; then
   AC_MSG_NOTICE([default version of EDG used (5.0)])
   edg_major_version_number=5
   edg_minor_version_number=0
else
   AC_MSG_NOTICE([specifying EDG version is not recommended])
   edg_major_version_number=`echo $enable_edg_version | cut -d\. -f1`
   edg_minor_version_number=`echo $enable_edg_version | cut -d\. -f2`
fi

AC_MSG_NOTICE([edg_version: major = "$edg_major_version_number", minor = "$edg_minor_version_number"])

if test "x$edg_major_version_number" = "x4"; then
  if test "x$edg_minor_version_number" = "x9"; then
    enable_edg_version49=yes
    AC_DEFINE([ROSE_USE_EDG_VERSION_4_9], [], [Whether to use the new EDG version 4.9])
  elif test "x$edg_minor_version_number" = "x12"; then
    enable_edg_version412=yes
    AC_DEFINE([ROSE_USE_EDG_VERSION_4_12], [], [Whether to use the new EDG version 4.12])
  else
    AC_MSG_FAILURE([could not identify the EDG minor version number])
  fi
elif test "x$edg_major_version_number" = "x5"; then
  if test "x$edg_minor_version_number" = "x0"; then
    enable_edg_version50=yes
    AC_DEFINE([ROSE_USE_EDG_VERSION_5_0], [], [Whether to use the new EDG version 5.0])
  else
    AC_MSG_FAILURE([could not identify the EDG minor version number])
  fi
else
  AC_MSG_FAILURE([could not identify the EDG major version number])
fi

enable_new_edg_interface=yes
AC_DEFINE([ROSE_USE_NEW_EDG_INTERFACE], [], [Whether to use the new interface to EDG])

AC_DEFINE_UNQUOTED([ROSE_EDG_MAJOR_VERSION_NUMBER], $edg_major_version_number , [EDG major version number])
AC_DEFINE_UNQUOTED([ROSE_EDG_MINOR_VERSION_NUMBER], $edg_minor_version_number , [EDG minor version number])

ROSE_EDG_MAJOR_VERSION_NUMBER=$edg_major_version_number
ROSE_EDG_MINOR_VERSION_NUMBER=$edg_minor_version_number

AC_SUBST(ROSE_EDG_MAJOR_VERSION_NUMBER)
AC_SUBST(ROSE_EDG_MINOR_VERSION_NUMBER)

AM_CONDITIONAL(ROSE_USE_EDG_VERSION_4_9, [test "x$enable_edg_version49" = xyes])
AM_CONDITIONAL(ROSE_USE_EDG_VERSION_4_12, [test "x$enable_edg_version412" = xyes])
AM_CONDITIONAL(ROSE_USE_EDG_VERSION_5_0, [test "x$enable_edg_version50" = xyes])
]
)

