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

          # Rasmussen (11/19/2019): "grep -Po" not supported on Apple OSX
          if test "x$OS_vendor" != xapple ; then
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
          fi

          # DQ (12/3/2016): These variables were previously set in the ROSE configuration.
            #echo "Get CXX Version info: OS_vendor = $OS_vendor"
            #echo "Get CXX Version info: OS_release = $OS_release"

          # DQ (12/3/2016): If we are on a Linux OS then we have the version number of Clang 
          # directly, but if this is on a MAC (OSX) system then this is the version of 
          # XCode and we have to map it to the clang version number.
          # if test "x$build_vendor" = xapple; then
            if test "x$OS_vendor" = xapple && $($CXX_COMPILER_COMMAND --version | grep -q 'Apple'); then

              # On an OSX system, the version of Clang is not clear since the "--version" option will report the 
              # version number of XCode (not clang).  So either we map from the version of the OS to the version 
              # of Clang used in it's version of XCode, or we map from the version of XCode (defined by the current 
              # values of (CXX_VERSION_MAJOR,CXX_VERSION_MINOR, and CXX_VERSION_PATCH).  Below I have used the 
              # version of the OS, but I'm not certain that is the best solution.  Perhaps we can asset that
              # the version of the OS indead maps to a specific version of XCode to be more secure in our choice 
              # of Clang version number, or take it directly from the XCode version number if that is a better solution.

              # Rasmussen (2/20/2017): The grep -Po option is not available on Mac OSX without installing a new
              # version of grep.  In addition, man pages from gnu.org on grep don't provide confidence in using it:
              #   -P
              #   --perl-regex
              #      Interpret the pattern as a Perl-compatible regular expression (PCRE). This is highly experimental,
              #      particularly when combined with the -z (--null-data) option, and ‘grep -P’ may warn of
              #      unimplemented features.
              #
              # Tnerefore, grep -Po usage has been replaced by shell scripts.

                CXX_VERSION_MAJOR=`${srcdir}/config/getAppleCxxMajorVersionNumber.sh`
                CXX_VERSION_MINOR=`${srcdir}/config/getAppleCxxMinorVersionNumber.sh`
                CXX_VERSION_PATCH=`${srcdir}/config/getAppleCxxPatchVersionNumber.sh`

                XCODE_VERSION_MAJOR=$CXX_VERSION_MAJOR
                XCODE_VERSION_MINOR=$CXX_VERSION_MINOR
                XCODE_VERSION_PATCH=$CXX_VERSION_PATCH

              # I think the clang versions all have patch level equal to zero.
                CXX_VERSION_PATCH=0

                if test $XCODE_VERSION_MAJOR -eq 7; then

                  # The versions of clang all depend upon the minor version number of XCode (for major version number equal to 7).
                    CXX_VERSION_MAJOR=3
                    case "$XCODE_VERSION_MINOR" in
                        0)
                            CXX_VERSION_MINOR=7
                            ;;
                        3)
                            CXX_VERSION_MINOR=8
                            ;;
                        *)
                            AC_MSG_FAILURE([unknown or unsupported version of XCode (XCODE_VERSION_MINOR = $XCODE_VERSION_MINOR)])
                            ;;
                    esac
                elif test $XCODE_VERSION_MAJOR -eq 8; then
                    CXX_VERSION_MAJOR=3
                  # DQ (3/3//2017): Added latest version information from Craig.
                    case "$XCODE_VERSION_MINOR" in
                        0|1)
                            CXX_VERSION_MINOR=8
                            ;;
                        *)
                            AC_MSG_FAILURE([unknown or unsupported version of XCode (XCODE_VERSION_MINOR = $XCODE_VERSION_MINOR)])
                            ;;
                    esac
                elif test $XCODE_VERSION_MAJOR -eq 9; then
                  # Rasmussen (10/27//2017): Added results for clang --version 9.0.0
                  # Rasmussen (04/04//2018): Added results for clang --version 9.0.1
                  # See https://opensource.apple.com/source/clang/clang-800.0.42.1/src/CMakeLists.txt
                  # Pei-Hung (01/16/2019): Revised based on https://en.wikipedia.org/wiki/Xcode#Latest_versions
                    case "$XCODE_VERSION_MINOR" in
                        0|1)
                            CXX_VERSION_MAJOR=4
                            CXX_VERSION_MINOR=0
                            ;;
                        3|4)
                            CXX_VERSION_MAJOR=5
                            CXX_VERSION_MINOR=0
                            ;;
                        *)
                            AC_MSG_FAILURE([unknown or unsupported version of XCode (XCODE_VERSION_MINOR = "$XCODE_VERSION_MINOR")])
                            ;;
                    esac
                elif test $XCODE_VERSION_MAJOR -eq 10; then
                    CXX_VERSION_MAJOR=6
                  # Rasmussen (11/21/2018): Added results for clang --version 10.0.0
                  # see https://gist.github.com/yamaya/2924292
                  # see also https://github.com/apple/swift-llvm/blob/swift-4.2-branch/CMakeLists.txt
                  # NOTE that this is very tentative and don't know if it will work
                    case "$XCODE_VERSION_MINOR" in
                        0|1)
                            CXX_VERSION_MINOR=0
                            ;;
                        *)
                            AC_MSG_FAILURE([unknown or unsupported version of XCode (XCODE_VERSION_MINOR = "$XCODE_VERSION_MINOR")])
                            ;;
                    esac
                elif test $XCODE_VERSION_MAJOR -eq 11; then
                    CXX_VERSION_MAJOR=8
                  # Rasmussen (11/19/2019): Added results for clang --version 11.0.0
                  # see https://en.wikipedia.org/wiki/Xcode#11.x_series
                  # NOTE that this is very tentative and don't know if it will work
                    case "$XCODE_VERSION_MINOR" in
                        0|2)
                            CXX_VERSION_MINOR=0
                            ;;
                        *)
                            AC_MSG_FAILURE([unknown or unsupported version of XCode (XCODE_VERSION_MINOR = "$XCODE_VERSION_MINOR")])
                            ;;
                    esac
                else
                    AC_MSG_FAILURE([unknown or unsupported version of XCode (XCODE_VERSION_MAJOR = "$XCODE_VERSION_MAJOR")])
                fi

#              # Note "build_os" is a variable determined by autoconf.
#                case $build_os in
#                    darwin13*)
#                      # This is Mac OSX version 10.9 (not clear on what version of clang this maps to via XCode)
#                        CXX_VERSION_MAJOR=3
#                        CXX_VERSION_MINOR=6
#                        CXX_VERSION_PATCH=0
#                        ;;
#                    darwin14*)
#                      # This is Mac OSX version 10.10 (not clear on what version of clang this maps to via XCode)
#                        CXX_VERSION_MAJOR=3
#                        CXX_VERSION_MINOR=7
#                        CXX_VERSION_PATCH=0
#                        ;;
#                    darwin15*)
#                      # This is Mac OSX version 10.11
#                        CXX_VERSION_MAJOR=3
#                        CXX_VERSION_MINOR=8
#                        CXX_VERSION_PATCH=0
#                        ;;
#                    *)
#                        echo "Error: Apple Mac OSX version not recognized as either darwin13, 14, or darwin15 ... (build_os = $build_os)";
#                        exit 1;
#                esac

              # DQ (12/3/2016): Added debugging for LLVM on MACOSX.
                #echo "compilerVendorName = $compilerVendorName"
                #echo "CXX_VERSION_MAJOR = $CXX_VERSION_MAJOR"
                #echo "CXX_VERSION_MINOR = $CXX_VERSION_MINOR"
                #echo "CXX_VERSION_PATCH = $CXX_VERSION_PATCH"
            fi
            ;;

        gnu)
            # Trying out various way of getting GCC version number: after version 7 "-dumpversion" was replaced by "-dumpfullversion"
            #     CXX_VERSION_TRIPLET=$($CXX_COMPILER_COMMAND --dumpversion | grep "^gcc" | sed 's/^.*[^0-9]\([0-9]*\.[0-9]*\.[0-9]*\).*$/\1/')
            CXX_VERSION_TRIPLET=$($CXX_COMPILER_COMMAND -dumpfullversion -dumpversion 2> /dev/null)
            CXX_VERSION_MAJOR=$(echo $CXX_VERSION_TRIPLET |cut -d. -f1)
            CXX_VERSION_MINOR=$(echo $CXX_VERSION_TRIPLET |cut -s -d. -f2)
            CXX_VERSION_PATCH=$(echo $CXX_VERSION_TRIPLET |cut -s -d. -f3)
            ;;

        intel)
            CXX_VERSION_MAJOR=$($CXX_COMPILER_COMMAND -dumpversion |cut -d. -f1)
            CXX_VERSION_MINOR=$($CXX_COMPILER_COMMAND -dumpversion |cut -s -d. -f2)
            CXX_VERSION_PATCH=$($CXX_COMPILER_COMMAND -dumpversion |cut -s -d. -f3)
            ;;

         rose)
            AC_MSG_ERROR([to compile ROSE using ROSE the backend should be specificed to NOT be ROSE (e.g. use gcc/g++)])
            ;;

         *)
            AC_MSG_ERROR([unknown vendor ($CXX_COMPILER_VENDOR) for backend compiler ($CXX_COMPILER_COMMAND)])
            ;;
    esac

    if test "$CXX_VERSION_MINOR" = ""; then
        AC_MSG_ERROR([cannot extract the version minor level for $CXX_COMPILER_COMMAND])
    fi

    if test "$CXX_VERSION_PATCH" = ""; then
        AC_MSG_WARN([cannot extract the version patch level for $CXX_COMPILER_COMMAND (assuming 0)])
        CXX_VERSION_PATCH=0
    fi

    CXX_VERSION_TRIPLET="$CXX_VERSION_MAJOR.$CXX_VERSION_MINOR.$CXX_VERSION_PATCH"

  # echo "Exiting at base of CXX_VERSION_TRIPLET = $CXX_VERSION_TRIPLET"
  # exit 1

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
    for symbol in __GNUG__ __GNUC__ __GNUC_MINOR__ __GNUC_PATCHLEVEL__; do
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
dnl     The result is conveyed to the caller as a shell variable, macroString, which is a string of comma-separated, quoted
dnl     compiler arguments with the whole string enclosed in curly braces. It is done this way so it can be interpolated
dnl     directly into C++ code that initializes a "char const *[]".
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
            macroString="$macroString, \"$macro\""
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

    ROSE_CONFIGURE_SECTION([Checking frontend compiler])
    GET_CXX_VERSION_INFO([$CXX], [$FRONTEND_CXX_COMPILER_VENDOR])
    GET_CXX_VERSION_MACROS
    SAVE_CXX_VERSION_INFO(FRONTEND)

    dnl ------------------------------------------------
    dnl --- Characteristics of the backend compiler  ---
    dnl ------------------------------------------------

    ROSE_CONFIGURE_SECTION([Checking backend compiler])

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
    #echo FRONTEND_CXX_COMPILER_VENDOR=$FRONTEND_CXX_COMPILER_VENDOR
    #echo FRONTEND_CXX_VERSION_MAJOR=$FRONTEND_CXX_VERSION_MAJOR
    #echo FRONTEND_CXX_VERSION_MINOR=$FRONTEND_CXX_VERSION_MINOR
    if test $FRONTEND_CXX_COMPILER_VENDOR == "gnu" && test $FRONTEND_CXX_VERSION_MAJOR -ge 5; then
      FRONTEND_CXX_VENDOR_AND_VERSION2="$FRONTEND_CXX_COMPILER_VENDOR-$FRONTEND_CXX_VERSION_MAJOR"
    elif test $FRONTEND_CXX_COMPILER_VENDOR == "clang" && test $FRONTEND_CXX_VERSION_MAJOR -ge 4; then
      FRONTEND_CXX_VENDOR_AND_VERSION2="$FRONTEND_CXX_COMPILER_VENDOR-$FRONTEND_CXX_VERSION_MAJOR"
    elif test $FRONTEND_CXX_COMPILER_VENDOR == "intel"; then
      FRONTEND_CXX_VENDOR_AND_VERSION2="$FRONTEND_CXX_COMPILER_VENDOR-$FRONTEND_CXX_VERSION_MAJOR"
    else
      FRONTEND_CXX_VENDOR_AND_VERSION2="$FRONTEND_CXX_COMPILER_VENDOR-$FRONTEND_CXX_VERSION_MAJOR.$FRONTEND_CXX_VERSION_MINOR"
    fi
    #echo FRONTEND_CXX_VENDOR_AND_VERSION2=$FRONTEND_CXX_VENDOR_AND_VERSION2
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
        AC_MSG_NOTICE([found the ROSE analysis tool being used as compiler for ROSE source code])
        AC_DEFINE(CXX_IS_ROSE_ANALYSIS, 1,
            [Is this the ROSE Analizer (part of tests to compile ROSE for analysis only using ROSE)])
    fi

    dnl DQ (2/20/2010): Support for testing AST File I/O.
    if test "$backendCompilerBaseName" = roseAstFileIO -o "$backendCompilerBaseName" = testAstFileIO; then
        AC_MSG_NOTICE([found the ROSE analysis tool being used as compiler for ROSE source code])
        AC_DEFINE(CXX_IS_ROSE_AST_FILE_IO, 1,
            [Is this the ROSE AST File IO (part of tests to compile ROSE for AST File IO only using ROSE)])
        AC_DEFINE(CXX_IS_ROSE_CODE_GENERATION, 1,
            [Is this the ROSE Code Generator (part of tests to compile ROSE and generate code using ROSE)])
        AC_DEFINE(CXX_IS_ROSE_ANALYSIS, 1,
            [Is this the ROSE Analizer (part of tests to compile ROSE for analysis only using ROSE)])
    fi

    if test "$backendCompilerBaseName" = roseCodeGeneration -o "$backendCompilerBaseName" = testCodeGeneration; then
        AC_MSG_NOTICE([found the ROSE code generation tool being used as compiler for ROSE source code])
        AC_DEFINE(CXX_IS_ROSE_CODE_GENERATION, 1,
            [Is this the ROSE Code Generator (part of tests to compile ROSE and generate code using ROSE)])
        AC_DEFINE(CXX_IS_ROSE_ANALYSIS, 1,
            [Is this the ROSE Analizer (part of tests to compile ROSE for analysis only using ROSE)])
    fi

    if test "$backendCompilerBaseName" = roseTranslator -o "$backendCompilerBaseName" = testTranslator; then
        AC_MSG_NOTICE([found the ROSE translator tool being used as compiler for ROSE source code])
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
