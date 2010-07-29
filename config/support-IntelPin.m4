AC_DEFUN([ROSE_SUPPORT_INTEL_PIN],
[
# Begin macro ROSE_SUPPORT_INTEL_PIN.

# Inclusion of test for INTEL PIN Dynamic Instrumentation Package and it's location.

AC_MSG_CHECKING(for IntelPin)
AC_ARG_WITH(IntelPin,
[  --with-IntelPin=PATH	Specify the prefix where Intel Pin Package is installed],
,
if test ! "$with_IntelPin" ; then
   with_IntelPin=no
fi
)

echo "In ROSE SUPPORT MACRO: with_IntelPin $with_IntelPin"

if test "$with_IntelPin" = no; then
   # If --with-IntelPin is not specified, then don't use it.
   echo "Skipping use of Intel Pin package support!"
else
   IntelPin_path=$with_IntelPin
   echo "Setup IntelPin support in ROSE! path = $IntelPin_path"
   AC_DEFINE([USE_ROSE_INTEL_PIN_SUPPORT],1,[Controls use of ROSE support for Intel Pin Dynamic Instrumentation Package.])
fi

AC_SUBST(IntelPin_path)

# End macro ROSE_SUPPORT_INTEL_PIN.
]
)
