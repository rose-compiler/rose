AC_DEFUN([ROSE_SUPPORT_CLANG],
[

AC_MSG_NOTICE([Clang support enabled])
AC_DEFINE([ROSE_USE_CLANG], 1, [Use Clang for parsing C/C++-like languages])

AC_PATH_TOOL([LLVM_CONFIG], [llvm-config])

AC_MSG_CHECKING([for Clang include path])
    if test -z "$CLANG_CXXFLAGS"; then
        llvm_include_path=`$LLVM_CONFIG --cppflags`
        if test -n "${llvm_include_path}"; then
            llvm_include_path="$llvm_include_path"
        fi
        CLANG_CXXFLAGS=$llvm_include_path
    fi
AC_MSG_RESULT([$CLANG_CXXFLAGS])
AC_SUBST([CLANG_CXXFLAGS])

AC_MSG_CHECKING([for Clang ld flags])
    if test -z "$CLANG_LDFLAGS"; then
        llvm_ldflags="`$LLVM_CONFIG --ldflags` -lclangFrontend -lclangSerialization -lclangDriver -lclangSema -lclangAnalysis -lclangAST -lclangParse -lclangLex -lclangBasic "
        llvm_ldflags+=`$LLVM_CONFIG --libs`
        if test -n "${llvm_ldflags}"; then
            llvm_ldflags="$llvm_ldflags"
        fi
        CLANG_LDFLAGS=$llvm_ldflags
    fi
AC_MSG_RESULT([$CLANG_LDFLAGS])
AC_SUBST([CLANG_LDFLAGS])

CLANG_LIBDIR=`$LLVM_CONFIG --libdir`
AC_SUBST([CLANG_LIBDIR])

]
)

