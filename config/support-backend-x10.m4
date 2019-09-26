AC_DEFUN([ROSE_SUPPORT_X10_BACKEND],
[
      AC_SUBST(BACKEND_X10_COMPILER)
      AC_DEFINE_UNQUOTED(
          [BACKEND_X10_COMPILER_NAME_WITH_PATH],
          [""],
          [Name of backend X10 compiler, including path])
])

