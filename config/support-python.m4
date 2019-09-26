AC_DEFUN([ROSE_SUPPORT_PYTHON],
[
    ROSE_CONFIGURE_SECTION([Checking Python])

    # Determine settings for the python development environment.
    # Provides:
    #   PYTHON_CPPFLAGS
    #   PYTHON_EXTRA_LDFLAGS
    #   PYTHON_EXTRA_LIBS
    #   PYTHON_LDFLAGS
    #   PYTHON_SITE_PKG
    #   PYTHON_VERSION
    # Version interval is half-open, i.e., [x, y)         this ] needed for m4
    AX_PYTHON_DEVEL([2.6.6], [4.0.0])

    if test "$PYTHON" != ""; then
        AC_DEFINE([USE_ROSE_PYTHON_SUPPORT], 1, [Controls use of ROSE support for Python.])
        PYTHON_VERSION_MAJOR_VERSION="`echo $ac_python_version | cut -d. -f1`"
        PYTHON_VERSION_MINOR_VERSION="`echo $ac_python_version | cut -d. -f2`"
        PYTHON_VERSION_PATCH_VERSION="`echo $ac_python_version | cut -d. -f3`"
    fi

    BACKEND_PYTHON_INTERPRETER_NAME_WITH_PATH="$PYTHON"
    AC_DEFINE_UNQUOTED([BACKEND_PYTHON_INTERPRETER_NAME_WITH_PATH],
        "$BACKEND_PYTHON_INTERPRETER_NAME_WITH_PATH",
        [Path to the backend python interpreter])

    # ROSE_USE_PYTHON is defined if there's a suitable python executable
    ROSE_USE_PYTHON=no
    test "$PYTHON" != "" && ROSE_USE_PYTHON=yes
    AM_CONDITIONAL(ROSE_USE_PYTHON, test "$ROSE_USE_PYTHON" = yes)
    
    # ROSE_USE_PYTHON_DEV is defined if there's a suitable python development environment
    ROSE_USE_PYTHON_DEV=$PYTHON_DEV_EXISTS
    AM_CONDITIONAL(ROSE_USE_PYTHON_DEV, test "$ROSE_USE_PYTHON_DEV" = yes)

    # The original code had a check for the python version which was an exact duplicat of the
    # check already performed by AX_PYTHON_DEVEL. I've removed the duplicate and replaced it with "true".
    AM_CONDITIONAL(ROSE_APPROVED_PYTHON_VERSION, true)

    AC_MSG_NOTICE([Python detection results:])
    AC_MSG_NOTICE([    PYTHON                       = $PYTHON])
    AC_MSG_NOTICE([    PYTHON_VERSION_MAJOR_VERSION = $PYTHON_VERSION_MAJOR_VERSION])
    AC_MSG_NOTICE([    PYTHON_VERSION_MINOR_VERSION = $PYTHON_VERSION_MINOR_VERSION])
    AC_MSG_NOTICE([    PYTHON_VERSION_PATCH_VERSION = $PYTHON_VERSION_PATCH_VERSION])
    AC_MSG_NOTICE([    PYTHON_SITE_PKG              = $PYTHON_SITE_PKG])
    AC_MSG_NOTICE([    PYTHON_DEV_EXISTS            = $PYTHON_DEV_EXISTS])
    AC_MSG_NOTICE([    PYTHON_CPPFLAGS              = $PYTHON_CPPFLAGS])
    AC_MSG_NOTICE([    PYTHON_LDFLAGS               = $PYTHON_LDFLAGS])
    AC_MSG_NOTICE([    PYTHON_EXTRA_LIBS            = $PYTHON_EXTRA_LIBS])
    AC_MSG_NOTICE([    PYTHON_EXTRA_LDFLAGS         = $PYTHON_EXTRA_LDFLAGS])
    AC_MSG_NOTICE([    ROSE_USE_PYTHON              = $PYTHON])
    AC_MSG_NOTICE([    ROSE_USE_PYTHON_DEV          = $ROSE_USE_PYTHON_DEV])

])
