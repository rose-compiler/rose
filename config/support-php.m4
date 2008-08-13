AC_DEFUN([ROSE_SUPPORT_PHP],
[

AC_ARG_WITH(php,
[  --with-php=PATH	Specify the prefix where PHP (and phc) is installed],
,
if test ! "$with_php" ; then
   with_php=no
fi
)

echo "In ROSE SUPPORT MACRO: with_php $with_php"

if test "$with_php" = no; then
   echo "Skipping use of PHP support (and phc)!"
else
   php_path=$with_php
   echo "Setup PHP support in ROSE! path = $php_path"
   AC_DEFINE([USE_ROSE_PHP_SUPPORT],1,[Controls use of ROSE support for PHP.])
fi

AC_SUBST(php_path)


]
)

