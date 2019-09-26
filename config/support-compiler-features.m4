# Detects various features of the C++ compiler.
#     Arguments:
#         $1  	     basic language, should be "c", "c++", or "fortran"
#         $2  	     compiler command and its switches
#         $3  	     optional prefix for output variables
#
#     Outputs:
#         Output variables are prefixed by optional macro argument $3
#         VENDOR:    Compiler vendor, one of "gnu", "llvm", or "intel"
#         VERSION:   Compiler version advertised to compiled programs (not --version output)
#         LANGUAGE:  The language, such as c++11, gnu++14, etc.
#
AC_DEFUN([ROSE_COMPILER_FEATURES],[
    quad="$($ac_abs_confdir/scripts/detect-compiler-characteristics --baselang=$1 --quad $2)"
    $3VENDOR="$(echo "$quad" |cut -d: -f1)"
    $3VERSION="$(echo "$quad" |cut -d: -f4)"
    $3LANGUAGE="$(echo "$quad" |cut -d: -f3)"
])
