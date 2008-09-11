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

