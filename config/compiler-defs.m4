dnl First written by Bobby Philip, 11/25/2001 define GET_COMPILER_SPECIFIC_DEFINES.  this macro tries to automatically
dnl determine the macros that are predefined by a specific compiler
AC_DEFUN([GET_COMPILER_SPECIFIC_DEFINES],
[

AC_LANG_PUSH(C++)

dnl DQ (4/1/2005): Modified to get compiler name from path
ROSE_CONFIGURE_SECTION([Backend compiler characteristics])
compilerName=$(basename "$BACKEND_CXX_COMPILER")
compilerVendorName="$ax_cv_cxx_compiler_vendor"

AC_MSG_NOTICE([    backend compiler        (BACKEND_CXX_COMPILER) = $BACKEND_CXX_COMPILER])
AC_MSG_NOTICE([    backend compiler vendor   (compilerVendorName) = $compilerVendorName])


dnl ========================================================================================================================
dnl === This section is where we test compilers and try to determine their characteristics, such as their version numbers.
dnl ========================================================================================================================

dnl Using 'changequote' is evil -- changing the m4 language midway through evaluating macros results in errors when expanding
dnl macros (like AC_MSG_NOTICE) that depend on the m4 language using square brackets for quoting. Instead, if you need to pass
dnl a special character (like a square bracket) through m4, use its quadrigraphs: a digraph enclosed in '@' signs. For instance,
dnl the quadrigraph "@<:@" is a left square bracket and "@:>@" is a right square bracket. Quadrigraphs are expanded as one of
dnl the last steps of m4 processing.

dnl DQ (2/1/2016): Change the switch to the vendor name computed from the macro call to AX COMPILER VENDOR (autoconf macro).
case "$compilerVendorName" in
    clang)
        BACKEND_GCC_MAJOR=$(echo |\
            $BACKEND_CXX_COMPILER --version 2>&1 |\
            grep -Po '(?<=version )@<:@^;@:>@+' |\
            cut -d. -f1 |\
            cut -d\( -f1)
        BACKEND_GCC_MINOR=$(echo |\
            $BACKEND_CXX_COMPILER --version 2>&1 |\
            grep -Po '(?<=version )@<:@^;@:>@+' |\
            cut -d. -f2 |\
            cut -d\( -f1)
        BACKEND_GCC_PATCHLEVEL=$(echo |\
            $BACKEND_CXX_COMPILER --version 2>&1 |\
            grep -Po '(?<=version )@<:@^;@:>@+' |\
            cut -d. -f3 |\
            cut -d\( -f1)
        if test "$BACKEND_GCC_PATCHLEVEL" = ""; then
            AC_MSG_WARN([cannot extract the patch level from $BACKEND_CXX_COMPILER -dumpversion (assuming 0)])
            BACKEND_GCC_PATCHLEVEL="0"
        fi

	BACKEND_VERSION_MAJOR="$BACKEND_GCC_MAJOR"
	BACKEND_VERSION_MINOR="$BACKEND_GCC_MINOR"
	BACKEND_VERSION_PATCH="$BACKEND_GCC_PATCHLEVEL"
	BACKEND_VERSION="$BACKEND_VERSION_MAJOR.$BACKEND_VERSION_MINOR.$BACKEND_VERSION_PATCH"

        macroString="\"-D__GNUG__=$BACKEND_GCC_MAJOR\""
        macroString="$macroString, \"-D__GNUC__=$BACKEND_GCC_MAJOR\""
        macroString="$macroString, \"-D__GNUC_MINOR__=$BACKEND_GCC_MINOR\""
        macroString="$macroString, \"-D__GNUC_PATCHLEVEL__=$BACKEND_GCC_PATCHLEVEL\""
        macroString="$macroString, \"--preinclude\", \"rose_edg_required_macros_and_functions.h\""
        macroString="{${macroString}}"
        ;;

    dnl Support for GNU gcc or g++ as a backend for compiling ROSE generated code
    gnu)
        BACKEND_GCC_MAJOR=$(echo |$BACKEND_CXX_COMPILER -dumpversion |cut -d. -f1)
        BACKEND_GCC_MINOR=$(echo |$BACKEND_CXX_COMPILER -dumpversion |cut -d. -f2)
        BACKEND_GCC_PATCHLEVEL=$(echo |$BACKEND_CXX_COMPILER -dumpversion |cut -d. -f3)
        if test "$BACKEND_GCC_PATCHLEVEL" = ""; then
            AC_MSG_WARN([cannot extract the patch level from $BACKEND_CXX_COMPILER -dumpversion (assuming 0)])
            BACKEND_GCC_PATCHLEVEL="0"
        fi

	BACKEND_VERSION_MAJOR="$BACKEND_GCC_MAJOR"
	BACKEND_VERSION_MINOR="$BACKEND_GCC_MINOR"
	BACKEND_VERSION_PATCH="$BACKEND_GCC_PATCHLEVEL"
	BACKEND_VERSION="$BACKEND_VERSION_MAJOR.$BACKEND_VERSION_MINOR.$BACKEND_VERSION_PATCH"

        macroString="\"-D__GNUG__=$BACKEND_GCC_MAJOR\""
        macroString="$macroString, \"-D__GNUC__=$BACKEND_GCC_MAJOR\""
        macroString="$macroString, \"-D__GNUC_MINOR__=$BACKEND_GCC_MINOR\""
        macroString="$macroString, \"-D__GNUC_PATCHLEVEL__=$BACKEND_GCC_PATCHLEVEL\""
        macroString="$macroString, \"--preinclude\", \"rose_edg_required_macros_and_functions.h\""
        macroString="{${macroString}}"
        ;;

    dnl Support for Intel icc as a backend for compiling ROSE generated code
    intel)
        BACKEND_INTEL_MAJOR=$(echo |$BACKEND_CXX_COMPILER -dumpversion |cut -d. -f1)
        BACKEND_INTEL_MINOR=$(echo |$BACKEND_CXX_COMPILER -dumpversion |cut -d. -f2)
        BACKEND_INTEL_PATCHLEVEL=$(echo |$BACKEND_CXX_COMPILER -dumpversion |cut -d. -f3)

	BACKEND_VERSION_MAJOR="$BACKEND_INTEL_MAJOR"
	BACKEND_VERSION_MINOR="$BACKEND_INTEL_MINOR"
	BACKEND_VERSION_PATCH="$BACKEND_INTEL_PATCHLEVEL"
	BACKEND_VERSION="$BACKEND_VERSION_MAJOR.$BACKEND_VERSION_MINOR.$BACKEND_VERSION_PATCH"

        tmpFile="/tmp/tmpICCMacroExtraction`uname -n`$$.C"
        echo "int main(int argc, char **argv){return 0;}" > "$tmpFile"
        extra_icc_defines=$("icpc" -# "test.C" 2>&1 |\
            grep "\-D" |\
            grep "GNUG\|__GNUC_PATCHLEVEL__\|__GNUC_MINOR__\|__GNUC__" |\
            sed ':a; /\\$/N; s/\\\n//; ta' |\
            sed 's/\\\//')
        tmp_macro=
        for macro_i in $extra_icc_defines; do
            tmp_macro="$tmp_macro, \"$macro_i\""
        done

        # Note that ${tmp_macro} starts with a comma, so I have padded the start of the macroString with
        # -D__ROSE_DUMMY_FIRST_MACRO__.
        macroString=" \"-D__ROSE_DUMMY_FIRST_MACRO__\" ${tmp_macro}"

        # DQ (4/4/2014): Commented this out again, we need this to get the /usr/include/math.h to be included 
        # by the #include_next directive in the Intel specific math.h, plus I think we support throw options better now.
        # The problem is demonstrated in test2014_36.C where M_PI is not defined because the #include_next is not 
        # processed to bring in the /usr/include/math.h file that is included using the #include_next directive.
        # macroString=" -D__PURE_INTEL_C99_HEADERS__ ${tmp_macro} --preinclude rose_edg_macros_and_functions_required_for_icc.h "
        # DQ (1/9/2010): I put this back and commented out the problem directly in the UPC file: lock.upc directly.
        # DQ (1/9/2010): This causes an error in math.h with an inconstant use of __THROW with the declaration of "abs()".
        #   from math.h _LIBIMF_EXT _LIBIMF_INT   _LIBIMF_PUBAPI abs( _LIBIMF_INT __x );
        # macroString="{\"-D__PURE_INTEL_C99_HEADERS__\" ${tmp_macro}"
        # macroString="{ \"-D__PURE_INTEL_C99_HEADERS__\" ${tmp_macro}"

        # DQ (11/1/2011): We need this same mechanism for C++'s use of EDG 4.x as we did for EDG 3.3 (but for C code
        # this was not required; and was simpler).
        # if test x$enable_new_edg_interface = xyes; then
        #   :
        # else
        #   macroString="${macroString}, \"--preinclude\", \"rose_edg_required_macros_and_functions.h\""
        # fi
        macroString="${macroString}, \"--preinclude\", \"rose_edg_required_macros_and_functions.h\""

        macroString="{${macroString}}"
        ;;

    dnl Support for ROSE "roseTranslator" etc. as a backend for compiling ROSE generated code
     rose)
        dnl DQ (2/1/2016): I think this is an error.  If we want to compile ROSE using ROSE then the backend should be
        dnl specificed to NOT be ROSE (e.g. gcc/g++).
        AC_MSG_ERROR([to compile ROSE using ROSE the backend should be specificed to NOT be ROSE (e.g. use gcc/g++)])
        ;;
 
     *)
        AC_MSG_ERROR([unknown vendor ($compilerVendorName) for backend compiler ($BACKEND_CXX_COMPILER)])
        ;;
esac

AC_MSG_NOTICE([    backend compiler version         (BACKEND_VERSION)       = $BACKEND_VERSION])
AC_MSG_NOTICE([    backend compiler major version   (BACKEND_VERSION_MAJOR) = $BACKEND_VERSION_MAJOR])
AC_MSG_NOTICE([    backend compiler minor version   (BACKEND_VERSION_MINOR) = $BACKEND_VERSION_MINOR])
AC_MSG_NOTICE([    backend compiler patch version   (BACKEND_VERSION_PATCH) = $BACKEND_VERSION_PATCH])
AC_MSG_NOTICE([    backend macros                   (macroString) = $macroString])



dnl ========================================================================================================================
dnl === This section is where we use the compiler characteristics detected above in order to define variables used during
dnl === the build process.
dnl ========================================================================================================================




AC_DEFINE_UNQUOTED([CXX_SPEC_DEF], $macroString, [-D options to hand to EDG C++ front-end.])


dnl DQ (2/1/2016): Fixed compiler vendor names to be consistatenyl lower case.
dnl DQ (1/9/2010): Detect the type of compiler being used. This is used to add the library libimf with libm to support use of
dnl the Intel compiler.  I have added AM conditional for GNU just for completeness.
AM_CONDITIONAL(USING_INTEL_COMPILER, [test "$compilerVendorName" = "intel"])
AM_CONDITIONAL(USING_GNU_COMPILER,   [test "$compilerVendorName" = "gnu"])
AM_CONDITIONAL(USING_CLANG_COMPILER, [test "$compilerVendorName" = "clang"])

dnl DQ (1/27/2010): Setup automake conditionals so that we can optionally skip files in ROSE that don't compile.
AM_CONDITIONAL(ROSE_USING_ROSE, [test "$compilerName" = roseTranslator -o "$compilerName" = testTranslator -o "$compilerName" = roseCodeGeneration -o "$compilerName" = testCodeGeneration -o "$compilerName" = roseAnalysis -o "$compilerName" = testAnalysis -o "$compilerName" = roseAstFileIO -o "$compilerName" = testAstFileIO])
AM_CONDITIONAL(ROSE_USING_ROSE_TRANSLATOR, [test "$compilerName" = roseTranslator -o "$compilerName" = testTranslator])

dnl DQ (2/21/2010): If we are testing AST File I/O then set the flag to indicate that we are testing the code generation as well.
AM_CONDITIONAL(ROSE_USING_ROSE_CODE_GENERATION, [test "$compilerName" = roseCodeGeneration -o "$compilerName" = testCodeGeneration -o "$compilerName" = roseAstFileIO -o "$compilerName" = testAstFileIO])

dnl DQ (5/29/2010): Set ROSE_USING_ROSE_ANALYSIS automake conditional whenever ROSE_USING_ROSE_CODE_GENERATION is set
AM_CONDITIONAL(ROSE_USING_ROSE_ANALYSIS, [test "$compilerName" = roseAnalysis -o "$compilerName" = testAnalysis -o "$compilerName" = roseCodeGeneration -o "$compilerName" = testCodeGeneration -o "$compilerName" = roseAstFileIO -o "$compilerName" = testAstFileIO])

AM_CONDITIONAL(ROSE_USING_ROSE_AST_FILE_IO, [test "$compilerName" = roseAstFileIO -o "$compilerName" = testAstFileIO])

if test "$compilerName" = roseAnalysis -o "$compilerName" = testAnalysis; then
    AC_MSG_NOTICE([Found the ROSE analysis tool being used as compiler for ROSE source code.])
    AC_DEFINE(CXX_IS_ROSE_ANALYSIS, 1, [Is this the ROSE Analizer (part of tests to compile ROSE for analysis only using ROSE)])
fi

dnl DQ (2/20/2010): Support for testing AST File I/O.
if test "$compilerName" = roseAstFileIO -o "$compilerName" = testAstFileIO; then
    AC_MSG_NOTICE([Found the ROSE analysis tool being used as compiler for ROSE source code.])
    AC_DEFINE(CXX_IS_ROSE_AST_FILE_IO, 1,
        [Is this the ROSE AST File IO (part of tests to compile ROSE for AST File IO only using ROSE)])
    AC_DEFINE(CXX_IS_ROSE_CODE_GENERATION, 1,
        [Is this the ROSE Code Generator (part of tests to compile ROSE and generate code using ROSE)])
    AC_DEFINE(CXX_IS_ROSE_ANALYSIS, 1,
        [Is this the ROSE Analizer (part of tests to compile ROSE for analysis only using ROSE)])
fi

if test "$compilerName" = roseCodeGeneration -o "$compilerName" = testCodeGeneration; then
    AC_MSG_NOTICE([Found the ROSE code generation tool being used as compiler for ROSE source code.])
    AC_DEFINE(CXX_IS_ROSE_CODE_GENERATION, 1,
        [Is this the ROSE Code Generator (part of tests to compile ROSE and generate code using ROSE)])
    AC_DEFINE(CXX_IS_ROSE_ANALYSIS, 1,
        [Is this the ROSE Analizer (part of tests to compile ROSE for analysis only using ROSE)])
fi

if test "$compilerName" = roseTranslator -o "$compilerName" = testTranslator; then
    AC_MSG_NOTICE([Found the ROSE translator tool being used as compiler for ROSE source code.])
    AC_DEFINE(CXX_IS_ROSE_TRANSLATOR, 1,
        [Is this the ROSE translator (part of tests to compile ROSE using ROSE)])
    AC_DEFINE(CXX_IS_ROSE_CODE_GENERATION, 1,
        [Is this the ROSE Code Generator (part of tests to compile ROSE and generate code using ROSE)])
    AC_DEFINE(CXX_IS_ROSE_ANALYSIS, 1,
        [Is this the ROSE Analizer (part of tests to compile ROSE for analysis only using ROSE)])
fi

if test "$compilerVendorName" = intel; then
    AC_DEFINE(BACKEND_CXX_IS_INTEL_COMPILER, 1, [Is this an Intel compiler being used to compile ROSE generated code])
fi

if test "$compilerVendorName" = "gnu"; then
    AC_DEFINE(BACKEND_CXX_IS_GNU_COMPILER, 1, [Is this a GNU compiler being used to compile ROSE generated code])
fi

if test "$compilerVendorName" = clang; then
    AC_DEFINE(BACKEND_CXX_IS_CLANG_COMPILER, 1, [Is this a Clang compiler being used to compile ROSE generated code])
fi

dnl DQ (9/13/2009): Add information about the backend compiler (which matches the associated header files generated).
dnl These values are used to control tests that are backend compiler and or compiler version dependent.
BACKEND_COMPILER_VERSION_NAME="$compilerVendorName"
BACKEND_COMPILER_VERSION_MAJOR_NUMBER="$BACKEND_GCC_MAJOR"
BACKEND_COMPILER_VERSION_MINOR_NUMBER="$BACKEND_GCC_MINOR"
BACKEND_COMPILER_VERSION_PATCHLEVEL_NUMBER="$BACKEND_GCC_PATCHLEVEL"

dnl DQ (9/14/2009): generate a name to use with the name of the ROSE EDG binary so that we can be version number specific.
GENERATED_COMPILER_NAME_AND_VERSION_SUFFIX="$BACKEND_COMPILER_VERSION_NAME-$BACKEND_COMPILER_VERSION_MAJOR_NUMBER.$BACKEND_COMPILER_VERSION_MINOR_NUMBER"

dnl DQ (9/30/2009): Exempt a set of old compiler versions from some testing. This is an attept to define a class of compiler
dnl versions that we should not test because it can generate internal compiler errors.  Specifically I am turning off the
dnl projects/interpretation directory when using GNU version 3.4 (this is an example of this class).
dnl I doubt this is ever defined since vendor names are lower case. [Robb Matzke, 2016-05-09]
AM_CONDITIONAL(OLD_COMPILER_VERSION,
    [test "$BACKEND_COMPILER_VERSION_NAME" = "GNU" -a \
          "$BACKEND_COMPILER_VERSION_MAJOR_NUMBER" = 3 -a \
          "$BACKEND_COMPILER_VERSION_MINOR_NUMBER" = 4])

AC_SUBST(BACKEND_COMPILER_VERSION_NAME)
AC_SUBST(BACKEND_COMPILER_VERSION_MAJOR_NUMBER)
AC_SUBST(BACKEND_COMPILER_VERSION_MINOR_NUMBER)
AC_SUBST(BACKEND_COMPILER_VERSION_PATCHLEVEL_NUMBER)

AC_SUBST(GENERATED_COMPILER_NAME_AND_VERSION_SUFFIX)

AC_LANG_POP(C++)
])
