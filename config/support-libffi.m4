AC_DEFUN([ROSE_SUPPORT_LIBFFI],
[

PKG_CHECK_MODULES([LIBFFI],[libffi],[with_libffi=yes],[with_libffi=no])

]
)
