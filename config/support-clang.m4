AC_DEFUN([ROSE_SUPPORT_CLANG],
[

AC_MSG_CHECKING([for enabled Clang Frontend support])

AC_ARG_ENABLE(clang-frontend, AS_HELP_STRING([--enable-clang-frontend], [Wether or not we use Clang as frontend for C/C++-like languages]))
# AM_CONDITIONAL(ROSE_USE_CLANG_FRONTEND, [test "x$enable_clang_frontend" = xyes])
if test "x$enable_clang_frontend" = "xyes"; then
  AC_DEFINE([ROSE_USE_CLANG_FRONTEND], [], [Use Clang for parsing C/C++-like languages])
fi

AC_SUBST(ROSE_USE_CLANG_FRONTEND)

AC_PATH_TOOL([LLVM_CONFIG], [llvm-config])

AC_MSG_CHECKING([for Clang fucking include path])
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
        llvm_ldflags="`$LLVM_CONFIG --ldflags` -lclangFrontendTool -lclangStaticAnalyzerFrontend -lclangStaticAnalyzerCheckers -lclangStaticAnalyzerCore -lclangIndex -lclangFrontend -lclangCodeGen  -lclangARCMigrate -lclangRewrite -lclangSerialization -lclangDriver -lclangParse -lclangSema -lclangAnalysis -lclangAST -lclangLex -lclangBasic "
        llvm_ldflags+=`$LLVM_CONFIG --libs engine ipo bitwriter linker asmparser instrumentation`
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

AC_DEFUN([INSTALL_CLANG_SPECIFIC_HEADERS],
[
mkdir -p ./include-staging/clang
cp ${srcdir}/src/frontend/CxxFrontend/Clang/clang-builtin-c.h ./include-staging/clang
cp ${srcdir}/src/frontend/CxxFrontend/Clang/clang-builtin-cpp.hpp ./include-staging/clang
cp ${srcdir}/src/frontend/CxxFrontend/Clang/clang-builtin-cuda.hpp ./include-staging/clang
cp ${srcdir}/src/frontend/CxxFrontend/Clang/clang-builtin-opencl.h ./include-staging/clang
]
)

