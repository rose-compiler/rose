AC_DEFUN([ROSE_SUPPORT_LIBFFI],
[

# DQ (9/21/2009): Added pkg.m4 to local ROSE/aclocal
# DQ (9/19/2009): This fails on OSX (ninjai.llnl.gov) (added by Peter to support ROSE interpreter).
PKG_CHECK_MODULES([LIBFFI],[libffi],[with_libffi=yes],[with_libffi=no])

# DQ (9/19/2009): This fails on Mac OSX (this variable is needed
# for an automake conditional that will be built in configure.in)
# with_libffi=no

]
)
