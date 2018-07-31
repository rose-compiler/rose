AC_DEFUN([ROSE_SUPPORT_PYTHON_API],
[
    AC_MSG_CHECKING([whether to enable Python API])
    enabled=no


    AC_ARG_ENABLE([python-api],
        [AS_HELP_STRING([--enable-python-api],
	    [Causes a Python API to be created fro commonly used parts of the ROSE library.  In order to build a Python
	     API, you must also specify --with-python and have the Python development system installed on your machine.
	     For Debian-based systems, the Python development package is named "python3-dev" and can be installed with
	     the command "sudo apt-get install python3-dev".])],
	[enabled=$enable_python_api])

    case "$enabled" in
        yes)
	    if test "$ROSE_USE_PYTHON" = no; then
	        AC_MSG_FAILURE([--with-python is required when --enable-python-api is in effect])
	    fi

            AC_DEFINE(ROSE_ENABLE_PYTHON_API, 1, [Define if you want a Python API])
	    AC_MSG_RESULT(enabled)
	    ;;
	*)
	    AC_MSG_RESULT(disabled)
	    ;;
    esac

    AM_CONDITIONAL([ROSE_ENABLE_PYTHON_API], [test "$enabled" = yes])
])
