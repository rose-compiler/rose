AC_DEFUN([HASH_ROSE_BINARY_SQL],
[

AC_MSG_CHECKING(for binary analysis support (internal))
AC_ARG_WITH(binarysql,
   [  --with-binarysql ... Enable MySQL for Binary Analysis work],with_binarysql=yes, with_binarysql=no,
with_binarysql=yes )

echo "In ROSE SUPPORT MACRO: with_binarysql = $with_binarysql"

if test "$with_binarysql" = no; then
   # If binary analysis support is not specified, then don't use it.
   echo "Skipping use of Binary SQL Support!"
else
   echo "Setup Binary SQL support in ROSE!"
   AC_DEFINE([HASH_ROSE_USE_BINARYSQL],1,[Controls use of ROSE support for Binary SQL.])
fi


]
)


AC_DEFUN([ROSE_SUPPORT_WINDOWS_ANALYSIS],
[
# Begin macro ROSE_SUPPORT_WINDOWS_ANALYSIS.

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
   AC_DEFINE([USE_ROSE_WINDOWS_ANALYSIS_SUPPORT],1,[Wine provides access to Windows specific binary file format headers.])
fi

AC_SUBST(wine_path)

# End macro ROSE_SUPPORT_WINDOWS_BINARY_ANALYSIS.
]
)
