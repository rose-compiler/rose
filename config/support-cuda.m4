# m4 macro to detect CUDA support (nvcc)
# This is used to support code generation for GPUs.
#  The conditional variable set for automake is USE_CUDA =0 or 1
#
# Liao 5/23/2014
#
AC_DEFUN([ROSE_SUPPORT_CUDA],
[

AC_MSG_CHECKING(for NVIDIA CUDA)

AC_ARG_WITH([cuda],
	AS_HELP_STRING([--with-cuda=PATH],
		[use nvcc for CUDA support (default autodetect).  PATH is bin directory containing nvcc if given]),
	[cudasetting=$withval],
	[cudasetting=try])

USE_CUDA=0

if test "x$cudasetting" != xno; then
	if test "x$cudasetting" = xtry -o "x$cudasetting" = xyes ; then
# autodetect the NVCC bin PATH
		if which nvcc > /dev/null 2> /dev/null; then
			NVCC="`which nvcc`"
			NVCCBIN="`dirname $NVCC`"
			USE_CUDA=1
		elif test -d "/usr/local/cuda/bin" ; then
                        NVCCBIN="/usr/local/cuda/bin"
                        USE_CUDA=1
		elif test "x$cudasetting" = xyes ; then
			AC_MSG_ERROR([--with-cuda set but nvcc command not found in PATH])
		fi

	else
		if test -d "$cudasetting"; then
#Verification of the bin directory containing nvcc is deferred later
			NVCCBIN="$cudasetting"
			USE_CUDA=1
		else
			AC_MSG_ERROR([argument to --with-cuda must be path to bin directory, but argument is not a directory])
		fi
	fi
fi

if test $USE_CUDA = 1; then
	if test ! -x "$NVCCBIN/nvcc" ; then
		AC_MSG_ERROR([nvcc could not be found in CUDA bin directory $NVCCBIN])
	fi
	AC_MSG_RESULT([$NVCCBIN])

#We don't concern about nvcc version for now
# DQ (5/27/2010): We only support a specific version of GHC, since version 6.12 does not
# include the required packages.
#   echo "ghc_version = "`ghc --version`
#   ghc_major_version_number=`ghc --version | tr -d \ | cut -d\n -f3 | cut -d\. -f1`
#   ghc_minor_version_number=`ghc --version | tr -d \ | cut -d\n -f3 | cut -d\. -f2`
#   ghc_patch_version_number=`ghc --version | tr -d \ | cut -d\n -f3 | cut -d\. -f3`
#
#   echo "ghc_major_version_number = $ghc_major_version_number"
#   echo "ghc_minor_version_number = $ghc_minor_version_number"
#   echo "ghc_patch_version_number = $ghc_patch_version_number"
#
#   echo "*************************************"
#   echo "ghc packages required in ROSE: base, haskell98, syb, mtl, containers (see the Build-Depends entry in projects/haskellport/rose.cabal.in.in)."
#   echo "ghc packages supported:"
#   ghc -v
#   echo "*************************************"
#
#   if test "x$ghc_major_version_number" = "x6"; then
#      echo "Recognized an accepted major version number."
#      if test "x$ghc_minor_version_number" = "x10"; then
#         echo "Recognized an accepted minor version number."
#      else
#         echo "ERROR: Could not identify an acceptable Haskell gch minor version number (ROSE requires 6.10.x)."
#         exit 1
#      fi
#   else
#      echo "ERROR: Could not identify an acceptable Haskell gch major version number (ROSE requires 6.10.x)."
#      exit 1
#   fi

else
	AC_MSG_RESULT([no])
fi

AC_SUBST(NVCCBIN)
AM_CONDITIONAL(USE_CUDA,test "$USE_CUDA" = 1)

])
