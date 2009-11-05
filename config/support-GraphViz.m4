AC_DEFUN([ROSE_SUPPORT_GRAPHVIZ],
[
# Begin macro ROSE_SUPPORT_GRAPHVIZ.

# The purpose of this macro is to build a automake conditional that 
# communicates if "dot" is available in the user's path so that 
# the PDF files generated in the ROSE/tutorial directory can be 
# tested for their existence.  If "dot" is available then the 
# PDF should be generated, else they will not be generated
# and we should not test for their existance.  The PDF's
# are used in the generation of the ROSE documentation.
# We don't require that the documentation generated
# and since we distribute the documentation with ROSE
# (and it is available on the web) we don't want to
# require these dependences.

AC_MSG_CHECKING([for GraphViz's dot program in path])

DOT_AVAILABLE="no"
DOT_PATH="`which dot > /dev/null`"
if test $? -eq 0; then
   DOT_AVAILABLE="yes"
   AC_MSG_RESULT(yes)
else
   # Dot is not found.
   AC_MSG_RESULT([not found in user's path])
   echo "GraphViz's dot program not found in the user's path!"
fi

AM_CONDITIONAL([ROSE_USE_GRAPHVIZ_DOT],[ test "x$DOT_AVAILABLE" = xyes ] )

# End macro ROSE_SUPPORT_MAPLE.
]
)
