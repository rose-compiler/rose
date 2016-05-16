dnl This file contains multiple macros.


dnl ========================================================================================================================
dnl This macro looks at the specified compiler command and vendor and tries to obtain a version number which is split into
dnl three parts: major, minor, patch.  The macro arguments are:
dnl   argument 1: the C++ compiler command
dnl   argument 2: the C++ compiler vendor, lower case
dnl The results are conveyed to the caller in shell variables:
dnl   CXX_COMPILER_COMMAND:   first argument
dnl   CXX_COMPILER_VENDOR:    second argument
dnl   CXX_VERSION_MAJOR:      first part of the version number (X in X.Y.Z)
dnl   CXX_VERSION_MINOR:      second part of the version number (Y in X.Y.Z)
dnl   CXX_VERSION_PATCH:      third part of the version number (Z in X.Y.Z) or "0" if not present
dnl
dnl This macro may have been first written by Bobby Philip 11/25/2001 and has been extensively modified since then.
dnl ========================================================================================================================
AC_DEFUN([GET_CXX_VERSION_INFO],[
    AC_LANG_PUSH(C++)

    CXX_COMPILER_COMMAND="$1"
    CXX_COMPILER_VENDOR="$2"

    case "$CXX_COMPILER_VENDOR" in
        clang)
            CXX_VERSION_MAJOR=$($CXX_COMPILER_COMMAND --version 2>&1 |\
                grep -Po '(?<=version )@<:@^ ;@:>@+' |\
                cut -d. -f1 |\
                cut -d\( -f1)
            CXX_VERSION_MINOR=$($CXX_COMPILER_COMMAND --version 2>&1 |\
                grep -Po '(?<=version )@<:@^ ;@:>@+' |\
                cut -d. -f2 |\
                cut -d\( -f1)
            CXX_VERSION_PATCH=$($CXX_COMPILER_COMMAND --version 2>&1 |\
                grep -Po '(?<=version )@<:@^ ;@:>@+' |\
                cut -d. -f3 |\
                cut -d\( -f1)
            ;;

        gnu)
            # Note that "--version" spits out X.Y.Z but is harder to parse than "-dumpversion" which shows only X.Y
            CXX_VERSION_MAJOR=$($CXX_COMPILER_COMMAND -dumpversion |cut -d. -f1)
            CXX_VERSION_MINOR=$($CXX_COMPILER_COMMAND -dumpversion |cut -d. -f2)
            CXX_VERSION_PATCH=$($CXX_COMPILER_COMMAND -dumpversion |cut -d. -f3)
            ;;

        intel)
            CXX_VERSION_MAJOR=$($CXX_COMPILER_COMMAND -dumpversion |cut -d. -f1)
            CXX_VERSION_MINOR=$($CXX_COMPILER_COMMAND -dumpversion |cut -d. -f2)
            CXX_VERSION_PATCH=$($CXX_COMPILER_COMMAND -dumpversion |cut -d. -f3)
            ;;

         rose)
            AC_MSG_ERROR([to compile ROSE using ROSE the backend should be specificed to NOT be ROSE (e.g. use gcc/g++)])
            ;;

         *)
            AC_MSG_ERROR([unknown vendor ($CXX_COMPILER_VENDOR) for backend compiler ($CXX_COMPILER_COMMAND)])
            ;;
    esac

    if test "$CXX_VERSION_PATCH" = ""; then
        AC_MSG_WARN([cannot extract the version patch level for $CXX_COMPILER_COMMAND (assuming 0)])
        CXX_VERSION_PATCH=0
    fi

    CXX_VERSION_TRIPLET="$CXX_VERSION_MAJOR.$CXX_VERSION_MINOR.$CXX_VERSION_PATCH"

    AC_LANG_POP(C++)
])

dnl ========================================================================================================================
dnl Squirrels away the results of GET_CXX_VERSION_INFO into shell variables by prepending them with other characters. For
dnl example, one might call GET_CXX_VERSION_INFO and then save the results by prepending "BACKEND_" to their names.
dnl Arguments:
dnl     argument 1: string to prepend, without the trailing underscore. E.g., "BACKEND".
dnl ========================================================================================================================
AC_DEFUN([SAVE_CXX_VERSION_INFO], [
    AC_MSG_NOTICE([    c++ compiler command         ($1[]_CXX_COMPILER_COMMAND) = "$CXX_COMPILER_COMMAND"])
    AC_MSG_NOTICE([    c++ compiler vendor           ($1[]_CXX_COMPILER_VENDOR) = "$CXX_COMPILER_VENDOR"])
    AC_MSG_NOTICE([    c++ compiler version triplet  ($1[]_CXX_VERSION_TRIPLET) = "$CXX_VERSION_TRIPLET"])
    AC_MSG_NOTICE([    c++ compiler major version      ($1[]_CXX_VERSION_MAJOR) = "$CXX_VERSION_MAJOR"])
    AC_MSG_NOTICE([    c++ compiler minor version      ($1[]_CXX_VERSION_MINOR) = "$CXX_VERSION_MINOR"])
    AC_MSG_NOTICE([    c++ compiler patch version      ($1[]_CXX_VERSION_PATCH) = "$CXX_VERSION_PATCH"])

    $1[]_CXX_COMPILER_COMMAND="$CXX_COMPILER_COMMAND"
    $1[]_CXX_COMPILER_VENDOR="$CXX_COMPILER_VENDOR"
    $1[]_CXX_VERSION_MAJOR="$CXX_VERSION_MAJOR"
    $1[]_CXX_VERSION_MINOR="$CXX_VERSION_MINOR"
    $1[]_CXX_VERSION_PATCH="$CXX_VERSION_PATCH"
    $1[]_CXX_VERSION_TRIPLET="$CXX_VERSION_TRIPLET"
])

dnl ========================================================================================================================
dnl This macro defines backward compatible shell variables representing version information for the backend compiler.
dnl
dnl Inputs set by GET_CXX_VERSION_INFO above:
dnl     CXX_COMPILER_VENDOR: C++ compiler vendor, lower case
dnl     CXX_VERSION_MAJOR:   X in X.Y.Z
dnl     CXX_VERSION_MINOR:   Y in X.Y.Z
dnl     CXX_VERSION_PATCH:   Z in X.Y.Z, or zero
dnl
dnl Outputs (backward compatible):
dnl     BACKEND_VERSION_MAJOR:          same as CXX_VERSION_MAJOR
dnl     BACKEND_VERSION_MINOR:          same as CXX_VERSION_MINOR
dnl     BACKEND_VERSION_PATCHLEVEL:     same as CXX_VERSION_PATCH
dnl     BACKEND_GCC_MAJOR:              same as CXX_VERSION_MAJOR if CXX_COMPILER_VENDOR is "gcc" or "clang"
dnl     BACKEND_GCC_MINOR:              same as CXX_VERSION_MINOR if CXX_COMPILER_VENDOR is "gcc" or "clang"
dnl     BACKEND_GCC_PATCHLEVEL:         same as CXX_VERSION_PATCH if CXX_COMPILER_VENDOR is "gcc" or "clang"
dnl     BACKEND_INTEL_MAJOR:            same as CXX_VERSION_MAJOR if CXX_COMPILER_VENDOR is "intel"
dnl     BACKEND_INTEL_MINOR:            same as CXX_VERSION_MINOR if CXX_COMPILER_VENDOR is "intel"
dnl     BACKEND_INTEL_PATCHLEVEL:       same as CXX_VERSION_PATCH if CXX_COMPILER_VENDOR is "intel"
dnl
dnl Outputs (new format):
dnl     BACKEND_CXX_COMPILER_COMMAND
dnl     BACKEND_CXX_COMPILER_VENDOR
dnl     BACKEND_CXX_VERSION_MAJOR
dnl     BACKEND_CXX_VERSION_MINOR
dnl     BACKEND_CXX_VERSION_PATCH
dnl     BACKEND_CXX_VERSION_TRIPLET
dnl ========================================================================================================================
AC_DEFUN([SAVE_BACKEND_VERSION_INFO],[
    SAVE_CXX_VERSION_INFO(BACKEND)

    BACKEND_VERSION_MAJOR="$CXX_VERSION_MAJOR"
    BACKEND_VERSION_MINOR="$CXX_VERSION_MINOR"
    BACKEND_VERSION_PATCHLEVEL="$CXX_VERSION_PATCH"

    case "$CXX_COMPILER_VENDOR" in
        clang|gnu)
            BACKEND_GCC_MAJOR="$CXX_VERSION_MAJOR"
            BACKEND_GCC_MINOR="$CXX_VERSION_MINOR"
            BACKEND_GCC_PATCHLEVEL="$CXX_VERSION_PATCH"
            ;;
        intel)
            BACKEND_INTEL_MAJOR="$CXX_VERSION_MAJOR"
            BACKEND_INTEL_MINOR="$CXX_VERSION_MINOR"
            BACKEND_INTEL_PATCHLEVEL="$CXX_VERSION_PATCH"
            ;;
    esac
])
            
dnl ========================================================================================================================
dnl This macro gets a list of arguments that need to be passed to a compiler when the compiler is used as the backend
dnl for ROSE.  This macro uses shell variables that have been initialized by GET_CXX_VERSION_INFO above, namely:
dnl     CXX_COMPILER_VENDOR
dnl     CXX_VERSION_MAJOR
dnl     CXX_VERSION_MINOR
dnl     CXX_VERSION_PATCH
dnl The result is conveyed to the caller as a shell variable, macroString which is a string of comma-separated, quoted
dnl compiler arguments with the whole string enclosed in curly braces.
dnl ========================================================================================================================
AC_DEFUN([GET_BACKEND_COMPILER_MACROS],[
    macroString=
    case "$CXX_COMPILER_VENDOR" in
        clang)
            macroString="\"-D__GNUG__=$CXX_VERSION_MAJOR\""
            macroString="$macroString, \"-D__GNUC__=$CXX_VERSION_MAJOR\""
            macroString="$macroString, \"-D__GNUC_MINOR__=$CXX_VERSION_MINOR\""
            macroString="$macroString, \"-D__GNUC_PATCHLEVEL__=$CXX_VERSION_PATCH\""
            macroString="$macroString, \"--preinclude\", \"rose_edg_required_macros_and_functions.h\""
            macroString="{${macroString}}"
            ;;

        gnu)
            macroString="\"-D__GNUG__=$CXX_VERSION_MAJOR\""
            macroString="$macroString, \"-D__GNUC__=$CXX_VERSION_MAJOR\""
            macroString="$macroString, \"-D__GNUC_MINOR__=$CXX_VERSION_MINOR\""
            macroString="$macroString, \"-D__GNUC_PATCHLEVEL__=$CXX_VERSION_PATCH\""
            macroString="$macroString, \"--preinclude\", \"rose_edg_required_macros_and_functions.h\""
            macroString="{${macroString}}"
            ;;

        intel)
            tmpFile="/tmp/tmpICCMacroExtraction`uname -n`$$.C"
            echo "int main(int argc, char **argv){return 0;}" > "$tmpFile"
            extra_icc_defines=$("icpc" -# "test.C" 2>&1 |\
                grep "\-D" |\
                grep "GNUG\|__GNUC_PATCHLEVEL__\|__GNUC_MINOR__\|__GNUC__" |\
                sed ':a; /\\$/N; s/\\\n//; ta' |\
                sed 's/\\\//')
            rm -f "$tmpFile"
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
            # macroString="-D__PURE_INTEL_C99_HEADERS__ ${tmp_macro} \
            #    --preinclude rose_edg_macros_and_functions_required_for_icc.h"
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

        *)
            AC_MSG_ERROR([cannot set backend compiler macros for $CXX_COMPILER_VENDOR $CXX_COMPILER_COMMAND command])
            ;;
    esac
    AC_MSG_NOTICE([    c++ backend macros                         (macroString) = $macroString])
])

dnl ========================================================================================================================
dnl This is the original macro from 2016-05-08 that tries to figure out some things about the backend compiler. It sets
dnl up various automake condition variables and rose_config.h defines.  It would be better if most of these varibles
dnl had the string "BACKEND" in their name so as to not be confused with similar things for the frontend compiler, but
dnl so many things depend on these names that I'm not going to change them just yet. [Robb Matzke, 2016-05-09].
dnl ========================================================================================================================
AC_DEFUN([GET_COMPILER_SPECIFIC_DEFINES],[

    dnl ------------------------------------------------
    dnl --- Characteristics of the frontend compiler ---
    dnl ------------------------------------------------

    ROSE_CONFIGURE_SECTION([Frontend compiler version])
    GET_CXX_VERSION_INFO([$CXX], [$FRONTEND_CXX_COMPILER_VENDOR])
    SAVE_CXX_VERSION_INFO(FRONTEND)

    dnl ------------------------------------------------
    dnl --- Characteristics of the backend compiler  ---
    dnl ------------------------------------------------

    ROSE_CONFIGURE_SECTION([Backend compiler version])

    backendCompilerBaseName=$(basename "$BACKEND_CXX_COMPILER")
    GET_CXX_VERSION_INFO([$BACKEND_CXX_COMPILER], [$BACKEND_CXX_COMPILER_VENDOR])
    SAVE_BACKEND_VERSION_INFO()

    # Use the info from GET_CXX_VERSION_INFO to get a list of backend switches in macroStrings.
    GET_BACKEND_COMPILER_MACROS()


    dnl ------------------------------------------------
    dnl --- Results for automake and rose_config.h   ---
    dnl ------------------------------------------------

    # Frontend C++ compiler vendor and version triplet
    FRONTEND_CXX_VENDOR_AND_VERSION3="$FRONTEND_CXX_COMPILER_VENDOR-$FRONTEND_CXX_VERSION_TRIPLET"
    AC_SUBST(FRONTEND_CXX_VENDOR_AND_VERSION3)

    # Frontend C++ compiler vendor and version major.minor
    FRONTEND_CXX_VENDOR_AND_VERSION2="$FRONTEND_CXX_COMPILER_VENDOR-$FRONTEND_CXX_VERSION_MAJOR.$FRONTEND_CXX_VERSION_MINOR"
    AC_SUBST(FRONTEND_CXX_VENDOR_AND_VERSION2)

    # Backend C++ compiler vendor and version triplet
    BACKEND_CXX_VENDOR_AND_VERSION3="$BACKEND_CXX_COMPILER_VENDOR-$BACKEND_CXX_VERSION_TRIPLET"
    AC_SUBST(BACKEND_CXX_VENDOR_AND_VERSION3)

    # Backend C++ compiler vendor and version major.minor
    BACKEND_CXX_VENDOR_AND_VERSION2="$BACKEND_CXX_COMPILER_VENDOR-$BACKEND_CXX_VERSION_MAJOR.$BACKEND_CXX_VERSION_MINOR"
    AC_SUBST(BACKEND_CXX_VENDOR_AND_VERSION2)

    AC_DEFINE_UNQUOTED([CXX_SPEC_DEF], $macroString, [-D options to hand to EDG C++ front-end.])

    dnl DQ (2/1/2016): Fixed compiler vendor names to be consistatenyl lower case.
    dnl DQ (1/9/2010): Detect the type of compiler being used. This is used to add the library libimf with libm to support use of
    dnl the Intel compiler.  I have added AM conditional for GNU just for completeness.
    AM_CONDITIONAL(USING_INTEL_COMPILER, [test "$BACKEND_CXX_COMPILER_VENDOR" = "intel"])
    AM_CONDITIONAL(USING_GNU_COMPILER,   [test "$BACKEND_CXX_COMPILER_VENDOR" = "gnu"])
    AM_CONDITIONAL(USING_CLANG_COMPILER, [test "$BACKEND_CXX_COMPILER_VENDOR" = "clang"])

    dnl DQ (1/27/2010): Setup automake conditionals so that we can optionally skip files in ROSE that don't compile.
    if test \
        "$backendCompilerBaseName" = roseTranslator -o \
        "$backendCompilerBaseName" = testTranslator -o \
        "$backendCompilerBaseName" = roseCodeGeneration -o \
        "$backendCompilerBaseName" = testCodeGeneration -o \
        "$backendCompilerBaseName" = roseAnalysis -o \
        "$backendCompilerBaseName" = testAnalysis -o \
        "$backendCompilerBaseName" = roseAstFileIO -o \
        "$backendCompilerBaseName" = testAstFileIO; then
        ROSE_USING_ROSE=yes
    else
        ROSE_USING_ROSE=
    fi
    AM_CONDITIONAL(ROSE_USING_ROSE, [test "ROSE_USING_ROSE" != ""])

    AM_CONDITIONAL(ROSE_USING_ROSE_TRANSLATOR,
        [test "$backendCompilerBaseName" = roseTranslator -o "$backendCompilerBaseName" = testTranslator])

    dnl DQ (2/21/2010): If we are testing AST File I/O then set the flag to indicate that we are testing the code
    dnl generation as well.
    if test \
        "$backendCompilerBaseName" = roseCodeGeneration -o \
        "$backendCompilerBaseName" = testCodeGeneration -o \
        "$backendCompilerBaseName" = roseAstFileIO -o \
        "$backendCompilerBaseName" = testAstFileIO; then
        ROSE_USING_ROSE_CODE_GENERATION=yes
    else
        ROSE_USING_ROSE_CODE_GENERATION=
    fi
    AM_CONDITIONAL(ROSE_USING_ROSE_CODE_GENERATION, [test "$ROSE_USING_ROSE_CODE_GENERATION" != ""])

    dnl DQ (5/29/2010): Set ROSE_USING_ROSE_ANALYSIS automake conditional whenever ROSE_USING_ROSE_CODE_GENERATION is set
    if test \
        "$backendCompilerBaseName" = roseAnalysis -o \
        "$backendCompilerBaseName" = testAnalysis -o \
        "$backendCompilerBaseName" = roseCodeGeneration -o \
        "$backendCompilerBaseName" = testCodeGeneration -o \
        "$backendCompilerBaseName" = roseAstFileIO -o \
        "$backendCompilerBaseName" = testAstFileIO; then
        ROSE_USING_ROSE_ANALYSIS=yes
    else
        ROSE_USING_ROSE_ANALYSIS=
    fi
    AM_CONDITIONAL(ROSE_USING_ROSE_ANALYSIS, [test "$ROSE_USING_ROSE_ANALYSIS" != ""])

    if test \
        "$backendCompilerBaseName" = roseAstFileIO -o \
        "$backendCompilerBaseName" = testAstFileIO; then
        ROSE_USING_ROSE_AST_FILE_IO=yes
    else
        ROSE_USING_ROSE_AST_FILE_IO=
    fi
    AM_CONDITIONAL(ROSE_USING_ROSE_AST_FILE_IO, [test "$ROSE_USING_ROSE_AST_FILE_IO" != ""])

    if test "$backendCompilerBaseName" = roseAnalysis -o "$backendCompilerBaseName" = testAnalysis; then
        AC_MSG_NOTICE([Found the ROSE analysis tool being used as compiler for ROSE source code.])
        AC_DEFINE(CXX_IS_ROSE_ANALYSIS, 1,
            [Is this the ROSE Analizer (part of tests to compile ROSE for analysis only using ROSE)])
    fi

    dnl DQ (2/20/2010): Support for testing AST File I/O.
    if test "$backendCompilerBaseName" = roseAstFileIO -o "$backendCompilerBaseName" = testAstFileIO; then
        AC_MSG_NOTICE([Found the ROSE analysis tool being used as compiler for ROSE source code.])
        AC_DEFINE(CXX_IS_ROSE_AST_FILE_IO, 1,
            [Is this the ROSE AST File IO (part of tests to compile ROSE for AST File IO only using ROSE)])
        AC_DEFINE(CXX_IS_ROSE_CODE_GENERATION, 1,
            [Is this the ROSE Code Generator (part of tests to compile ROSE and generate code using ROSE)])
        AC_DEFINE(CXX_IS_ROSE_ANALYSIS, 1,
            [Is this the ROSE Analizer (part of tests to compile ROSE for analysis only using ROSE)])
    fi

    if test "$backendCompilerBaseName" = roseCodeGeneration -o "$backendCompilerBaseName" = testCodeGeneration; then
        AC_MSG_NOTICE([Found the ROSE code generation tool being used as compiler for ROSE source code.])
        AC_DEFINE(CXX_IS_ROSE_CODE_GENERATION, 1,
            [Is this the ROSE Code Generator (part of tests to compile ROSE and generate code using ROSE)])
        AC_DEFINE(CXX_IS_ROSE_ANALYSIS, 1,
            [Is this the ROSE Analizer (part of tests to compile ROSE for analysis only using ROSE)])
    fi

    if test "$backendCompilerBaseName" = roseTranslator -o "$backendCompilerBaseName" = testTranslator; then
        AC_MSG_NOTICE([Found the ROSE translator tool being used as compiler for ROSE source code.])
        AC_DEFINE(CXX_IS_ROSE_TRANSLATOR, 1,
            [Is this the ROSE translator (part of tests to compile ROSE using ROSE)])
        AC_DEFINE(CXX_IS_ROSE_CODE_GENERATION, 1,
            [Is this the ROSE Code Generator (part of tests to compile ROSE and generate code using ROSE)])
        AC_DEFINE(CXX_IS_ROSE_ANALYSIS, 1,
            [Is this the ROSE Analizer (part of tests to compile ROSE for analysis only using ROSE)])
    fi

    if test "$CXX_COMPILER_VENDOR" = intel; then
        AC_DEFINE(BACKEND_CXX_IS_INTEL_COMPILER, 1, [Is this an Intel compiler being used to compile ROSE generated code])
    fi

    if test "$CXX_COMPILER_VENDOR" = "gnu"; then
        AC_DEFINE(BACKEND_CXX_IS_GNU_COMPILER, 1, [Is this a GNU compiler being used to compile ROSE generated code])
    fi

    if test "$CXX_COMPILER_VENDOR" = clang; then
        AC_DEFINE(BACKEND_CXX_IS_CLANG_COMPILER, 1, [Is this a Clang compiler being used to compile ROSE generated code])
    fi

    dnl DQ (9/13/2009): Add information about the backend compiler (which matches the associated header files generated).
    dnl These values are used to control tests that are backend compiler and or compiler version dependent.
    dnl Matzke (5/9/2016): Keeping the same names and semantics, although the "GCC" names will be empty for intel compilers.
    BACKEND_COMPILER_VERSION_NAME="$BACKEND_CXX_COMPILER_VENDOR"
    BACKEND_COMPILER_VERSION_MAJOR_NUMBER="$BACKEND_GCC_MAJOR"
    BACKEND_COMPILER_VERSION_MINOR_NUMBER="$BACKEND_GCC_MINOR"
    BACKEND_COMPILER_VERSION_PATCHLEVEL_NUMBER="$BACKEND_GCC_PATCHLEVEL"
    GENERATED_COMPILER_NAME_AND_VERSION_SUFFIX="$BACKEND_COMPILER_VERSION_NAME-$BACKEND_COMPILER_VERSION_MAJOR_NUMBER.$BACKEND_COMPILER_VERSION_MINOR_NUMBER"
    AC_SUBST(GENERATED_COMPILER_NAME_AND_VERSION_SUFFIX)

    dnl DQ (9/30/2009): Exempt a set of old compiler versions from some testing. This is an attept to define a class of compiler
    dnl versions that we should not test because it can generate internal compiler errors.  Specifically I am turning off the
    dnl projects/interpretation directory when using GNU version 3.4 (this is an example of this class).
    dnl Matzke (5/9/2016): I doubt this is ever defined since vendor names are lower case.
    AM_CONDITIONAL(OLD_COMPILER_VERSION,
        [test "$BACKEND_COMPILER_VERSION_NAME" = "GNU" -a \
              "$BACKEND_COMPILER_VERSION_MAJOR_NUMBER" = 3 -a \
              "$BACKEND_COMPILER_VERSION_MINOR_NUMBER" = 4])

    AC_SUBST(BACKEND_COMPILER_VERSION_NAME)
    AC_SUBST(BACKEND_COMPILER_VERSION_MAJOR_NUMBER)
    AC_SUBST(BACKEND_COMPILER_VERSION_MINOR_NUMBER)
    AC_SUBST(BACKEND_COMPILER_VERSION_PATCHLEVEL_NUMBER)

])
