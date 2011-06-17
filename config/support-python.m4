AC_DEFUN([ROSE_SUPPORT_PYTHON],
[

AC_ARG_WITH(python,
[  --with-python=PATH       Specify the path to the Python interpreter],
,
if test ! "$with_python" ; then
   with_python=no
fi
)

echo "In ROSE SUPPORT MACRO: with_python $with_python"

if test "$with_python" = no; then
   echo "Skipping use of Python support!"
else
   python_path=$with_python
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

AC_SUBST(python_path)

# End macro ROSE_SUPPORT_PYTHON.
]
)

