AC_DEFUN([ROSE_SUPPORT_PYTHON],
[
    # Determine settings for the python development environment.
    # Provides:
    #   PYTHON_CPPFLAGS
    #   PYTHON_EXTRA_LDFLAGS
    #   PYTHON_EXTRA_LIBS
    #   PYTHON_LDFLAGS
    #   PYTHON_SITE_PKG
    #   PYTHON_VERSION
    # Version interval is half-open, i.e., [x, y)         this ] needed for m4
    AX_PYTHON_DEVEL([2.7.0], [4.0.0])

    if test "$PYTHON" != ""; then
        AC_DEFINE([USE_ROSE_PYTHON_SUPPORT], 1, [Controls use of ROSE support for Python.])
    fi

    AC_DEFINE_UNQUOTED([BACKEND_PYTHON_INTERPRETER_NAME_WITH_PATH],"$PYTHON",[Path to the backend python interpreter])

])
