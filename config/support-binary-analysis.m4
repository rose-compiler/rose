AC_DEFUN([ROSE_SUPPORT_BINARY_ANALYSIS],
[
# Begin macro ROSE_SUPPORT_BINARY_ANALYSIS.

# Inclusion of support for Binary Analysis in ROSE.

AC_MSG_CHECKING(for binary analysis support (internal))
AC_ARG_WITH(binary_analysis,
[  --with-binary_analysis	Specify if ROSE should be configured to include binary analyssis support (makes ROSE 40% larger)],
,
if test ! "$with_binary_analysis" ; then
   with_binary_analysis=no
fi
)

echo "In ROSE SUPPORT MACRO: with_binary_analysis = $with_binary_analysis"

if test "$with_binary_analysis" = no; then
   # If binary analysis support is not specified, then don't use it.
   echo "Skipping use of Binary Analysis Support!"
else
   echo "Setup Binary Analysis support in ROSE!"
   AC_DEFINE([USE_ROSE_BINARY_ANALYSIS_SUPPORT],1,[Controls use of ROSE support for Binary Analysis.])
fi

# End macro ROSE_SUPPORT_BINARY_ANALYSIS.
]
)
