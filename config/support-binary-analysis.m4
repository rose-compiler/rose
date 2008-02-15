AC_DEFUN([ROSE_SUPPORT_BINARY_ANALYSIS],
[
# Begin macro ROSE_SUPPORT_BINARY_ANALYSIS.

# Inclusion of support for Binary Analysis in ROSE.

AC_MSG_CHECKING(for binary analysis support (internal))
AC_ARG_WITH(binary_analysis,
[  --with-binary_analysis	Specify if ROSE should be configured to include binary analyssis support (makes ROSE 40% larger)],
,
with_binary_analysis=yes
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


AC_DEFUN([ROSE_SUPPORT_WINDOWS_BINARY_ANALYSIS],
[
# Begin macro ROSE_SUPPORT_WINDOWS_BINARY_ANALYSIS.

AC_MSG_CHECKING(for Wine -- Wine is not an emulator)
AC_ARG_WITH(wine,
[  --with-wine=PATH	Specify the prefix where Wine is installed],
,
if test ! "$with_wine" ; then
   with_wine=no
fi
)

echo "In ROSE SUPPORT MACRO: with_wine $with_wine"

if test "$with_wine" = no; then
   # If wine is not specified, then don't use it.
   echo "Skipping use of Wine for Windows Binary Analysis!"
else
   wine_path=$with_wine
   echo "Setup Wine support in ROSE! path = $wine_path"
   AC_DEFINE([USE_ROSE_WINDOWS_BINARY_ANALYSIS_SUPPORT],1,[Wine provides access to Windows specific binary file format headers.])
fi

AC_SUBST(wine_path)

# End macro ROSE_SUPPORT_WINDOWS_BINARY_ANALYSIS.
]
)
