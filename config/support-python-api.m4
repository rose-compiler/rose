AC_DEFUN([ROSE_SUPPORT_PYTHON_API],
[
    AC_MSG_CHECKING([whether to enable Python API])
    enabled=yes


    AC_ARG_ENABLE([python-api],
        [AS_HELP_STRING([--disable-python-api],
	    [ROSE normally includes a Python API for commonly used parts of the library. Inclusion of the Python
	     API depends only on the Boost.Python C++ header files being available.  The Python API is enabled
	     by default but can be disabled with this switch.])],
	[enabled=$enable_python_api])

    case "$enabled" in
        yes)
            AC_DEFINE(ROSE_ENABLE_PYTHON_API, 1, [Define if you want a Python API])
	    AC_MSG_RESULT(enabled)
	    ;;
	*)
	    AC_MSG_RESULT(disabled)
	    ;;
    esac

    AM_CONDITIONAL([ROSE_ENABLE_PYTHON_API], [test "$enabled" = yes])
])
