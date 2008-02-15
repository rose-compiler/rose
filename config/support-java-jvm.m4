AC_DEFUN([ROSE_SUPPORT_JAVA_JVM],
[
# Begin macro ROSE_SUPPORT_JAVA_JVM.

AC_MSG_CHECKING(for JavaJVM)
AC_ARG_WITH(JavaJVM,
[  --with-JavaJVM=PATH	Specify the prefix where JavaJVM is installed],
,
if test ! "$with_JavaJVM" ; then
   with_JavaJVM=no
fi
)

echo "In ROSE SUPPORT MACRO: with_JavaJVM $with_JavaJVM"

if test "$with_JavaJVM" = no; then
   # If JavaJVM is not specified, then don't use it.
   echo "Skipping use of JavaJVM (and Fortran2003 support)!"
else
   JAVA_JVM_PATH=$with_JavaJVM
   echo "Setup JavaJVM support in ROSE! path = $JAVA_JVM_PATH"
   AC_DEFINE([USE_ROSE_JAVA_JVM_SUPPORT],1,[Controls use of ROSE support for JavaJVM (required for the Open Fortran Parser from LANL).])
fi

AC_SUBST(JAVA_JVM_PATH)

# End macro ROSE_SUPPORT_FORTRAN.
]
)
