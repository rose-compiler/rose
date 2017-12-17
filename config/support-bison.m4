AC_DEFUN([ROSE_SUPPORT_BISON],
[
    ROSE_CONFIGURE_SECTION([Checking Bison version])

  # Find the Bison program
    AX_WITH_PROG(BISON,bison)

  # Check the Bison version
    AX_PROG_BISON_VERSION([2.7.1], [AC_MSG_NOTICE([[Bison version >= 2.7.1 has been detected]])],
                                   [AC_MSG_ERROR([[Bison >= 2.7.1 required for building ROSE]])])
])
