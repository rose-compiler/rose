# ASR (06-09-2010) Adding LLVM Support macro
# Begin macro ROSE_SUPPORT_LLVM
# llvm_path stores the prefix

AC_DEFUN([ROSE_SUPPORT_LLVM],
[
ROSE_CONFIGURE_SECTION([Checking LLVM])

AC_ARG_WITH(llvm,
[  --with-llvm=PATH	Specify the prefix where LLVM (and opt) is installed],
,
if test ! "$with_llvm" ; then
   with_llvm=no
fi
)

AC_MSG_NOTICE([in ROSE SUPPORT MACRO: with_llvm = "$with_llvm"])

if test "$with_llvm" = no; then
   AC_MSG_NOTICE([skipping use of LLVM support (and opt)])
else
   llvm_path=$with_llvm
   AC_MSG_NOTICE([setup LLVM support in ROSE. path = "$llvm_path"])
   AC_DEFINE([USE_ROSE_LLVM_SUPPORT],1,[Controls use of ROSE support for LLVM.])
fi

AC_SUBST(llvm_path)

LLVM_CONFIG=$llvm_path/bin/llvm-config
AC_SUBST(LLVM_CONFIG)
LLVM_COMPILER_MAJOR_VERSION_NUMBER=`echo|$LLVM_CONFIG --version 2>/dev/null| head -1 | cut -f2 -d\) | tr -d \  | cut -d\. -f1`
LLVM_COMPILER_MINOR_VERSION_NUMBER=`echo|$LLVM_CONFIG --version 2>/dev/null| head -1 | cut -f2 -d\) | tr -d \  | cut -d\. -f2`

# Pei-Hung (02/15/2017): LLVM version 4.0.1 for RoseToLLVM tool 
# Philippe (01/05/2018): LLVM version 5.0.0 for RoseToLLVM tool 
llvm_version_6_0_0=no
if test x$LLVM_COMPILER_MAJOR_VERSION_NUMBER == x6; then
   if test x$LLVM_COMPILER_MINOR_VERSION_NUMBER == x0; then
      AC_MSG_NOTICE([we have identified version 6.0.0 of LLVM])
      llvm_version_6_0_0=yes
   fi
fi
AM_CONDITIONAL(ROSE_USING_LLVM_6_0_0, [test "x$llvm_version_6_0_0" = "xyes"])

LLVM_LIB_DIR="`${LLVM_CONFIG} --libdir 2>/dev/null`"
LLVM_CPPFLAGS="`${LLVM_CONFIG} --cppflags 2>/dev/null | sed s/-I/-isystem\ /`"
LLVM_CPPFLAGS+=" -DLLVMVERSION="
LLVM_CPPFLAGS+="`${LLVM_CONFIG} --version 2>/dev/null`"
LLVM_LDFLAGS="`${LLVM_CONFIG} --ldflags 2>/dev/null` -R${LLVM_LIB_DIR}"
LLVM_LIBS="`${LLVM_CONFIG} --libs all 2>/dev/null`"
LLVM_LIBS+=" `${LLVM_CONFIG} --system-libs 2>/dev/null`"
AC_SUBST(LLVM_CPPFLAGS)
AC_SUBST(LLVM_LDFLAGS)
AC_SUBST(LLVM_LIBS)
]
)
