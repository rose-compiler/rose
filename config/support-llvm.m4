# ASR (06-09-2010) Adding LLVM Support macro
# Begin macro ROSE_SUPPORT_LLVM
# llvm_path stores the prefix

AC_DEFUN([ROSE_SUPPORT_LLVM],
[

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


]
)
