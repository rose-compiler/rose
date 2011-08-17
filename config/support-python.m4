AC_DEFUN([ROSE_SUPPORT_PYTHON],
[

if test "$support_python_language" != yes; then
   AC_MSG_NOTICE([Skipping use of Python support!])
else
   AC_DEFINE([USE_ROSE_PYTHON_SUPPORT],
             1,
             [Controls use of ROSE support for Python.])

   # Determine settings for the python development environment.
   # Provides:
   #   PYTHON_CPPFLAGS
   #   PYTHON_EXTRA_LDFLAGS
   #   PYTHON_EXTRA_LIBS
   #   PYTHON_LDFLAGS
   #   PYTHON_SITE_PKG
   #   PYTHON_VERSION
   AX_PYTHON_DEVEL([2.7.0], [3.0.0])
fi

# End macro ROSE_SUPPORT_PYTHON.
]
)
