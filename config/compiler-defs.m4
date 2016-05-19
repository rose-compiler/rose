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
dnl This macro runs the C++ compiler ($CXX) to figure out what macros are defined that relate to the compiler version
dnl number.  The macros are stored in the CXX_VERSION_DEFINES shell variable as a space-separated list of elements where
dnl each element has the format "-Dname=value".
dnl
dnl Arguments:
dnl     none
dnl Input:
dnl     $CXX
dnl Output:
dnl     $CXX_VERSION_MACROS
dnl ========================================================================================================================
AC_DEFUN([GET_CXX_VERSION_MACROS],[

    # Prior to 2016-05-18 we set various version-related macros using the version information that was detected by the
    # GET_CXX_VERSION_INFO macro. However, that info was sometimes wrong (e.g., "g++ -dumpversion" sometimes doesn't
    # report the patch level, in which case we incorrectly define __GNUC_PATCHLEVEL__ to zero).  Dan determined that
    # the "-dM" switch is accepted by GNU g++, LLVM clang++, and Intel icpc, so we now use that instead. This gives
    # correct values for the macros and it makes this function much smaller.
    tmp_input=/tmp/compiler-defs-$$.C
    tmp_output=/tmp/compiler-defs-$$.out
    touch $tmp_input
    if ! $CXX -E -dM $tmp_input >$tmp_output; then
        AC_MSG_ERROR([compiler command failed: $CXX -E -dM $tmp_input])
    fi

    CXX_VERSION_MACROS=
    for symbol in __GNUC__ __GNUC_MINOR__ __GNUC_PATCHLEVEL__; do
        value="$(sed -ne "s/#define $symbol //p" <$tmp_output)"
        if test "$value" != ""; then
            if test "$CXX_VERSION_MACROS" = ""; then
                CXX_VERSION_MACROS="-D$symbol=$value"
            else
                CXX_VERSION_MACROS="$CXX_VERSION_MACROS -D$symbol=$value"
            fi
        fi
    done

    rm -f $tmp_input $tmp_output
])

dnl ========================================================================================================================
dnl Squirrels away the results of GET_CXX_VERSION_INFO into shell variables by prepending them with other characters. For
dnl example, one might call GET_CXX_VERSION_INFO and then save the results by prepending "BACKEND_" to their names.
dnl Arguments:
dnl     argument 1: string to prepend, without the trailing underscore. E.g., "BACKEND".
dnl ========================================================================================================================
AC_DEFUN([SAVE_CXX_VERSION_INFO], [
    AC_MSG_NOTICE([    c++ compiler command         $1[]_CXX_COMPILER_COMMAND = "$CXX_COMPILER_COMMAND"])
    AC_MSG_NOTICE([    c++ compiler vendor           $1[]_CXX_COMPILER_VENDOR = "$CXX_COMPILER_VENDOR"])
    AC_MSG_NOTICE([    c++ compiler version triplet  $1[]_CXX_VERSION_TRIPLET = "$CXX_VERSION_TRIPLET"])
    AC_MSG_NOTICE([    c++ compiler major version      $1[]_CXX_VERSION_MAJOR = "$CXX_VERSION_MAJOR"])
    AC_MSG_NOTICE([    c++ compiler minor version      $1[]_CXX_VERSION_MINOR = "$CXX_VERSION_MINOR"])
    AC_MSG_NOTICE([    c++ compiler patch version      $1[]_CXX_VERSION_PATCH = "$CXX_VERSION_PATCH" (0 means not reported)])
    AC_MSG_NOTICE([    c++ compiler version macros    $1[]_CXX_VERSION_MACROS = "$CXX_VERSION_MACROS"])

    $1[]_CXX_COMPILER_COMMAND="$CXX_COMPILER_COMMAND"
    $1[]_CXX_COMPILER_VENDOR="$CXX_COMPILER_VENDOR"
    $1[]_CXX_VERSION_MAJOR="$CXX_VERSION_MAJOR"
    $1[]_CXX_VERSION_MINOR="$CXX_VERSION_MINOR"
    $1[]_CXX_VERSION_PATCH="$CXX_VERSION_PATCH"
    $1[]_CXX_VERSION_TRIPLET="$CXX_VERSION_TRIPLET"
    $1[]_CXX_VERSION_MACROS="$CXX_VERSION_MACROS"
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
dnl     BACKEND_CXX_VERSION_MACROS
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
dnl This function gets a list of macros (compiler -D switches) that are defined by the backend compiler and which therefore
dnl must also be defined when ROSE runs the C++ frontend.  This function handles only the version-related macros, but in
dnl theory, the frontend should pre-define all the same things as the backend so that the frontend and backend both follow
dnl the same paths through header files.
dnl
dnl Arguments:
dnl     None
dnl Input:
dnl     BACKEND_CXX_* variables saved by previous call to SAVE_VERSION_INFO
dnl Output:
dnl     The result is conveyed to the caller as a shell variable, macroString which is a string of comma-separated, quoted
dnl     compiler arguments with the whole string enclosed in curly braces. It is done this way so it can be interpolated
dnl     directly in to C++ code that initializes a "char const *[]".
dnl ========================================================================================================================
AC_DEFUN([GET_BACKEND_COMPILER_MACROS],[
    if test "$BACKEND_CXX_COMPILER_COMMAND" = ""; then
        AC_MSG_ERROR([should have determined backend compiler characteristics already])
    fi

    # Convert space-separated list of "-Dname=value" pairs to a C++ char*[] initializer stored in macroString
    # E.g., convert:
    #     -Dapple=red -Dbanana=yellow
    # to
    #     {"-Dapple=red", "-Dbanana=yellow"}
    macroString=""
    for macro in $BACKEND_CXX_VERSION_MACROS --preinclude rose_edg_required_macros_and_functions.h; do
        if test "$macroString" = ""; then
            macroString="\"$macro\""
        else
            macroString="$macroString \"$macro\""
        fi
    done
    macroString="{$macroString}"
    AC_MSG_NOTICE([    c++ backend macros                            macroString = $macroString])
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
    GET_CXX_VERSION_MACROS
    SAVE_CXX_VERSION_INFO(FRONTEND)

    dnl ------------------------------------------------
    dnl --- Characteristics of the backend compiler  ---
    dnl ------------------------------------------------

    ROSE_CONFIGURE_SECTION([Backend compiler version])

    backendCompilerBaseName=$(basename "$BACKEND_CXX_COMPILER")
    GET_CXX_VERSION_INFO([$BACKEND_CXX_COMPILER], [$BACKEND_CXX_COMPILER_VENDOR])
    GET_CXX_VERSION_MACROS
    SAVE_BACKEND_VERSION_INFO

    # Use the info from GET_CXX_VERSION_INFO to get a list of backend switches in macroStrings.
    GET_BACKEND_COMPILER_MACROS


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
