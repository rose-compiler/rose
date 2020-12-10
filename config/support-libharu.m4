AC_DEFUN([ROSE_SUPPORT_LIBHARU],
[
AC_MSG_CHECKING(for LIBHARU)
# Begin macro ROSE_SUPPORT_LIBHARU.

# There are currently no prerequisites
AC_ARG_WITH(libharu,
[  --with-libharu	Specify if we should build LIBHARU],
[with_libharu=$withval],
[with_libharu=yes]
)

AC_MSG_NOTICE([in ROSE SUPPORT MACRO: with_libharu = "$with_libharu"])

if test "$with_libharu" = no; then
   AC_MSG_NOTICE([skipping installation for LIBHARU])
else
   AC_CONFIG_SUBDIRS([src/3rdPartyLibraries/libharu-2.1.0])
   AC_DEFINE([ROSE_WITH_LIBHARU], [1], [libharu enabled])
fi
   AC_MSG_NOTICE([setup installation for LIBHARU in ROSE. with_libharu = "$with_libharu"])
   AM_CONDITIONAL(ROSE_WITH_LIBHARU, [test "x$with_libharu" = xyes])

# End macro ROSE_SUPPORT_LIBHARU.
]
)
