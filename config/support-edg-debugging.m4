AC_DEFUN([ROSE_SUPPORT_EDG_DEBUGGING],
[
# Begin macro ROSE_SUPPORT_EDG_DEBUGGING.

# This optionally permits the unions in EDG to be converted to structs to support 
# debugging of data member field accesses in the EDG/Sage III translation.

AC_MSG_CHECKING(for specification of EDG union/struct debugging support)
AC_ARG_ENABLE(edg_union_struct_debugging,
[  --enable-edg_union_struct_debugging	Specify if EDG Union/Struct debugging support is to be used],
,
if test ! "$enable_edg_union_struct_debugging" ; then
   enable_edg_union_struct_debugging=no
fi
)

AC_MSG_NOTICE([in ROSE SUPPORT MACRO: enable_edg_union_struct_debugging = "$enable_edg_union_struct_debugging"])

if test "$enable_edg_union_struct_debugging" = no; then
   # If edg_union_struct_debugging is not specified, then don't use it.
   AC_MSG_NOTICE([skipping use of EDG Union/Struct debugging support])
   edg_union_struct_debugging_value="union"
else
   edg_union_struct_debugging_value="struct"
   AC_MSG_NOTICE([setup EDG Union/Struct debugging support in ROSE. edg_union_struct_debugging_value = "$edg_union_struct_debugging_value"])
fi

AC_DEFINE([USE_ROSE_EDG_DEBUGGING_SUPPORT],1,[Controls if EDG Union/Struct debugging support is to be used.])

AC_SUBST(edg_union_struct_debugging_value)

# End macro ROSE_SUPPORT_EDG_DEBUGGING.
]
)
