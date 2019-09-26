AC_DEFUN([ROSE_SUPPORT_IDA],
[
# Begin macro ROSE_SUPPORT_IDA.

# Inclusion of test for the Interactive Disassembler IDA Pro package and it's location.

AC_MSG_CHECKING(for ida)
AC_ARG_WITH(ida,
[  --with-ida=PATH	Specify the prefix where IDA Pro is installed],
,
if test ! "$with_ida" ; then
   with_ida=no
fi
)

AC_MSG_NOTICE([in ROSE SUPPORT MACRO: with_ida = "$with_ida"])

if test "$with_ida" = no; then
   # If ida is not specified, then don't use it.
   AC_MSG_NOTICE([skipping use of Ida])
else
   ida_path=$with_ida
   AC_MSG_NOTICE([setup Ida support in ROSE. path = "$ida_path"])
   AC_DEFINE_UNQUOTED([IDA_PRO_PATH],"$ida_path",[Prefix path for use of IDA.])
   AC_DEFINE([USE_ROSE_IDA_SUPPORT],1,[Controls use of IDA support for disassembling.])

fi

AC_SUBST(ida_path)

# End macro ROSE_SUPPORT_IDA.
]
)
