# ASR (06-09-2010) Adding LLVM Support macro
# Begin macro ROSE_SUPPORT_LLVM
# llvm_path stores the prefix

AC_DEFUN([ROSE_SUPPORT_LLVM],
[
ROSE_CONFIGURE_SECTION([LLVM])

AC_ARG_WITH(llvm,
[  --with-llvm=PATH	Specify the prefix where LLVM (and opt) is installed],
,
if test ! "$with_llvm" ; then
   with_llvm=no
fi
)

echo "In ROSE SUPPORT MACRO: with_llvm $with_llvm"

if test "$with_llvm" = no; then
   echo "Skipping use of LLVM support (and opt)!"
else
   llvm_path=$with_llvm
   echo "Setup LLVM support in ROSE! path = $llvm_path"
   AC_DEFINE([USE_ROSE_LLVM_SUPPORT],1,[Controls use of ROSE support for LLVM.])
fi

AC_SUBST(llvm_path)

LLVM_CONFIG=$llvm_path/bin/llvm-config
AC_SUBST(LLVM_CONFIG)
LLVM_COMPILER_MAJOR_VERSION_NUMBER=`echo|$LLVM_CONFIG --version | head -1 | cut -f2 -d\) | tr -d \  | cut -d\. -f1`
LLVM_COMPILER_MINOR_VERSION_NUMBER=`echo|$LLVM_CONFIG --version | head -1 | cut -f2 -d\) | tr -d \  | cut -d\. -f2`

# Pei-Hung (02/15/2017): LLVM version 4.0.1 for RoseToLLVM tool 
llvm_version_4_0_1=no
if test x$LLVM_COMPILER_MAJOR_VERSION_NUMBER == x4; then
   if test x$LLVM_COMPILER_MINOR_VERSION_NUMBER == x0; then
      echo "Note: we have identified version 4.0.1 of LLVM!"
      llvm_version_4_0_1=yes
   fi
fi
AM_CONDITIONAL(ROSE_USING_LLVM_4_0_1, [test "x$llvm_version_4_0_1" = "xyes"])

LLVM_LIB_DIR="`${LLVM_CONFIG} --libdir`"
LLVM_CPPFLAGS="`${LLVM_CONFIG} --cppflags | sed s/-I/-isystem\ /`"
LLVM_CPPFLAGS+=" -DLLVMVERSION="
LLVM_CPPFLAGS+="`${LLVM_CONFIG} --version`"
LLVM_LDFLAGS="`${LLVM_CONFIG} --ldflags` -R${LLVM_LIB_DIR}"
LLVM_LIBS="`${LLVM_CONFIG} --libs all`"
LLVM_LIBS+=" `${LLVM_CONFIG} --system-libs`"
AC_SUBST(LLVM_CPPFLAGS)
AC_SUBST(LLVM_LDFLAGS)
AC_SUBST(LLVM_LIBS)
]
)
