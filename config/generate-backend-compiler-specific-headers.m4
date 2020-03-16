AC_DEFUN([GENERATE_BACKEND_CXX_COMPILER_SPECIFIC_HEADERS],
dnl DQ 12/17/2001 build from what Bobby put into configure.in directly 11/25/2001
dnl This builds the directories required for the back-end compiler specific header files.
dnl it depends upon the CHOOSE BACKEND COMPILER macro to have already been called.
[
 # BP : 11/20/2001, create a directory to store header files which are compiler specific
   compilerName="`basename $BACKEND_CXX_COMPILER`"

   # echo "In GENERATE BACKEND CXX COMPILER SPECIFIC HEADERS: Using back-end C++ compiler = \"$BACKEND_CXX_COMPILER\" compiler vendor name = $ax_cv_cxx_compiler_vendor for processing of unparsed source files from ROSE preprocessors."

   chmod u+x "${srcdir}/config/create_system_headers"
   if test "$ROSE_CXX_HEADERS_DIR" = ""; then
      dnl AC_MSG_NOTICE([ROSE_CXX_HEADERS_DIR not set ...])
      ROSE_CXX_HEADERS_DIR="${prefix}/include/${compilerName}_HEADERS"
   else
      AC_MSG_NOTICE([ROSE_CXX_HEADERS_DIR = "$ROSE_CXX_HEADERS_DIR"])
   fi

   saveCurrentDirectory="`pwd`"
   cd "$srcdir"
   absolutePath_srcdir="`pwd`"
   cd "$saveCurrentDirectory"

 # DQ (9/1/2009): Output the absolute path
   AC_MSG_NOTICE([absolutePath_srcdir = "${absolutePath_srcdir}"])

 # This is a way to make this a global shaell variable, but it is better to pass it as a parameter to the function.
 # export language="cxx"
 # This language name is used as a parameter to the backend compiler, so it must be "c" or "c++"
   language="c++"
 # echo "In generate backend compiler specific headers: language = $language"

 # Use the full path name to generate the header from the correctly specified version of the backend compiler
   mkdir -p "./include-staging/${compilerName}_HEADERS"

 # DQ (2/2/2016): Adding additional parameters to this function call.
 # "${srcdir}/config/create_system_headers" "${BACKEND_CXX_COMPILER}" "./include-staging/${compilerName}_HEADERS" "${absolutePath_srcdir}"
   VERBOSE="$verbose" "${srcdir}/config/create_system_headers" "${BACKEND_CXX_COMPILER}" "./include-staging/${compilerName}_HEADERS" "${absolutePath_srcdir}" "${language}" "$BACKEND_CXX_COMPILER_VENDOR"

   error_code=$?
   AC_MSG_NOTICE([error_code = "$error_code"])
   if test $error_code != 0; then
        AC_MSG_FAILURE([error in generate backend CXX compiler specific headers: call to ${srcdir}/config/create_system_headers: nonzero exit code returned to caller error_code = "$error_code"])
   fi

   AC_MSG_NOTICE([BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER = "$BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER"])
   AC_MSG_NOTICE([BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER = "$BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER"])

 # DQ (2/2/2016): Debugging new support for detected the compiler vendor.
 # echo "Exiting after call to create_system_headers in GENERATE BACKEND CXX COMPILER SPECIFIC HEADERS"
 # exit 1

 # DQ (8/14/2010): GNU 4.5 includes some code that will not compile and appears to not be valid C++ code.
 # We fixup a specific GNU 4.5 issues use of "return { __mask };"
   if test x$BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER == x4; then
      if test x$BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER == x5; then
         AC_MSG_NOTICE([we have identified version 4.5 of GNU C/C++ which triggers use of a modified copy of iomanip header file])
         cp ${srcdir}/config/iomanip-gnu-4.5 ./include-staging/iomanip-gnu-4.5
         AC_MSG_NOTICE([remove the links])
         rm ./include-staging/gcc_HEADERS/hdrs4/c++/4.5.0/iomanip;
         rm ./include-staging/g++_HEADERS/hdrs7/c++/4.5.0/iomanip;
         rm ./include-staging/g++_HEADERS/hdrs3/iomanip;
         AC_MSG_NOTICE([rebuild links to the modified file])
         ln -s ./include-staging/iomanip-gnu-4.5 ./include-staging/gcc_HEADERS/hdrs4/c++/4.5.0/iomanip
         ln -s ./include-staging/iomanip-gnu-4.5 ./include-staging/g++_HEADERS/hdrs7/c++/4.5.0/iomanip
         ln -s ./include-staging/iomanip-gnu-4.5 ./include-staging/g++_HEADERS/hdrs3/iomanip
      fi
   fi

 # DQ (9/19/2010): Copy the upc.h header file from the config directory to our include-staging/${BACKEND_CXX_COMPILER}_HEADERS directory.
 # It might be that these should be put into a UPC specific subdirectory (so that the C compiler can't accedentally find them), but this should be discussed.
   AC_MSG_NOTICE([copying UPC++ header files into ./include-staging/${compilerName}_HEADERS directory])
   cp ${srcdir}/config/upc.h ./include-staging/${compilerName}_HEADERS
   cp ${srcdir}/config/upc_io.h ./include-staging/${compilerName}_HEADERS
   cp ${srcdir}/config/upc_relaxed.h ./include-staging/${compilerName}_HEADERS
   cp ${srcdir}/config/upc_strict.h ./include-staging/${compilerName}_HEADERS
   cp ${srcdir}/config/upc_collective.h ./include-staging/${compilerName}_HEADERS
   cp ${srcdir}/config/bupc_extensions.h ./include-staging/${compilerName}_HEADERS

 # DQ (8/22/2011): Added support for SSE.
 # Copy alternative SSE and MMX headers to be seen by ROSE ahead of the originals.
   cp ${srcdir}/config/rose_specific_emmintrin.h ./include-staging/${compilerName}_HEADERS/emmintrin.h
   cp ${srcdir}/config/rose_specific_xmmintrin.h ./include-staging/${compilerName}_HEADERS/xmmintrin.h
 # DQ (8/29/2015): This file is also required since the one available in the Intel header files will 
 # not compie with EDG (requires MS decl_spec grammar).
   cp ${srcdir}/config/rose_specific_mmintrin.h  ./include-staging/${compilerName}_HEADERS/mmintrin.h

 # Phlin (6/18/2012): Added support for SSE4.2.
   cp ${srcdir}/config/rose_specific_ammintrin.h ./include-staging/${compilerName}_HEADERS/ammintrin.h
   cp ${srcdir}/config/rose_specific_nmmintrin.h ./include-staging/${compilerName}_HEADERS/nmmintrin.h
   cp ${srcdir}/config/rose_specific_pmmintrin.h ./include-staging/${compilerName}_HEADERS/pmmintrin.h
   cp ${srcdir}/config/rose_specific_smmintrin.h ./include-staging/${compilerName}_HEADERS/smmintrin.h
   cp ${srcdir}/config/rose_specific_tmmintrin.h ./include-staging/${compilerName}_HEADERS/tmmintrin.h

 # Phlin (6/18/2012): Added support for AVX.
 # Only GCC 4.6+ supports AVX instructions.
   if test x$BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER == x4; then
    # if test "$BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER" -ge "6"; then
      if test "$BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER" -ge "6"; then
         cp ${srcdir}/config/rose_specific_avxintrin.h ./include-staging/${compilerName}_HEADERS/avxintrin.h
      fi
   else
      if test "$BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER" -ge "5"; then
         cp ${srcdir}/config/rose_specific_avxintrin.h ./include-staging/${compilerName}_HEADERS/avxintrin.h
      fi
   fi

 # DQ (11/21/2016): EDG 4.12 can't handle a specific line of the GNU 6.1 vector.h header file. So build a modified version for this case.
 # The function calls: "_M_move_assign();" appear to be a problem for EDG 4.12 (seqfaults internally in il.c).
   if test "x$edg_major_version_number" = "x4"; then
      if test "$edg_minor_version_number" -eq "12"; then
         if test x$BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER == x6; then
            if test "$BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER" -ge "1"; then
               mkdir -p ./include-staging/${compilerName}_HEADERS/bits
               cp ${srcdir}/config/rose_specific_GNU_6_1_stl_vector.h ./include-staging/${compilerName}_HEADERS/bits/stl_vector.h
               cp ${srcdir}/config/rose_specific_GNU_6_1_stl_list.h ./include-staging/${compilerName}_HEADERS/bits/stl_list.h
               cp ${srcdir}/config/rose_specific_GNU_6_1_stl_deque.h ./include-staging/${compilerName}_HEADERS/bits/stl_deque.h
               cp ${srcdir}/config/rose_specific_GNU_6_1_hashtable.h ./include-staging/${compilerName}_HEADERS/bits/hashtable.h
            fi
         fi
      fi
   fi

   error_code=$?
   AC_MSG_NOTICE([error_code = "$error_code"])
   if test $error_code != 0; then
        AC_MSG_FAILURE([error copying upc.h header file: nonzero exit code returned to caller error_code = "$error_code"])
   fi

   if test "x$BACKEND_CXX_COMPILER_VENDOR" = "xclang"; then
      cp ${srcdir}/config/rose_specific_clang_atomic ./include-staging/${compilerName}_HEADERS/atomic
   fi

# DQ (2/21/2017): This is only required for C language support (not for C++).
# DQ (2/4/2017): Need to add required header file to support Intel compiler because we are using 
# the __INTEL_CLANG_COMPILER macro to use EDG with the Intel header files.
#  if test "x$BACKEND_CXX_COMPILER_VENDOR" = "xintel"; then
#     cp ${srcdir}/config/rose_specific_tgmath_clang.h ./include-staging/${compilerName}_HEADERS/tgmath_clang.h
#  fi

# DQ (1/15/2017): Debugging info to debug clange on Mac OSX.
AC_MSG_NOTICE([edg_major_version_number = "$edg_major_version_number"])
AC_MSG_NOTICE([compilerName = "${compilerName}"])
AC_MSG_NOTICE([BACKEND_CXX_COMPILER_VENDOR = "$BACKEND_CXX_COMPILER_VENDOR"])
AC_MSG_NOTICE([build_vendor = "$build_vendor"])

 # DQ (12/14/2016): We now want this to apply to EDG 4.12 because it does not handle C++11 constexpr 
 # return type of builtin functions properly. Note that this is only an issue when processing file 
 # generated via CPP (or using -E flags to the compiler) header files.
 # DQ (10/10/2016): Make the us of the ROSE generation of builtins dependent on the version of EDG.
 # This is because EDG 4.12 introduces a new mechanism to handle builtin functions and is thus more 
 # complete.  However, it is still missing __builtin_fxsave() functions, though this is not clear 
 # why since they are present in EDG 4.12's tables.
   if test "x$edg_major_version_number" = "x4"; then
#     if test "$edg_minor_version_number" -le "11"; then
         # DQ (9/12/2016): Added use of new support to specify constexpr specific builtin functions (uses an additional file, support added by Robb).
         # DQ (9/1/2016): Adding generated header file from new support for builtin functions.
           AC_MSG_NOTICE([now output the builtin generated file into build directory])

         # DQ (2/25/2017): We need to support a different version of the buildin function support for EDG 4.9 because later versions 
         # of this generated file cause the tests/roseTests/astFileIOTests makefile rule parallelMerge_short and parallelMerge_medium
         # to fail randomly.  This can only so far be traced to it failing for EDG 4.9, but not EDG 4.12.  So the only practical solution 
         # is to build a version of the rose_edg_required_macros_and_functions.h file specific for EDG 4.9 while we are in the transition 
         # from EDG 4.9 to EDG 4.12.  I can't figure out the bug that is in EDG 4.9 and since it is fixed in later versions of EDG it
         # is not pratical to focus more time on this issue.
         # ${srcdir}/scripts/builtinLlvmFunctions.pl ${srcdir}/config/Builtins.def > ./include-staging/${compilerName}_HEADERS/rose_generated_builtin_functions.h
         # ${srcdir}/scripts/builtinLlvmFunctions.pl --constexpr=${srcdir}/config/constexpr_builtins.def ${srcdir}/config/Builtins.def > ./include-staging/${compilerName}_HEADERS/rose_generated_builtin_functions.h
           if test "$edg_minor_version_number" -le "9"; then
              AC_MSG_NOTICE([building EDG 4.9 specific version of rose_generated_builtin_functions.h])
              ${srcdir}/scripts/builtinLlvmFunctions.pl --constexpr=${srcdir}/config/constexpr_builtins.def ${srcdir}/config/Builtins_EDG_49.def > ./include-staging/${compilerName}_HEADERS/rose_generated_builtin_functions.h
           else
              AC_MSG_NOTICE([building EDG 4.12 (and later) specific version of rose_generated_builtin_functions.h])
              ${srcdir}/scripts/builtinLlvmFunctions.pl --constexpr=${srcdir}/config/constexpr_builtins.def ${srcdir}/config/Builtins.def > ./include-staging/${compilerName}_HEADERS/rose_generated_builtin_functions.h
           fi
           AC_MSG_NOTICE([now use sed to edit the builtins into the ./include-staging/${compilerName}_HEADERS/rose_edg_required_macros_and_functions.h file using the file of builtin functions])

         # DQ (1/17/2017): Make this different for Mac OSX and other (Linux) systems.
         # DQ (1/15/2017): Note that on Mac OSX it is required to use the additional option to specify the backup file name (I think this is the more portable form).
         # sed -i "/REPLACE_ME_WITH_GENERATED_BUILTIN_FUNCTIONS/r./include-staging/${compilerName}_HEADERS/rose_generated_builtin_functions.h" "./include-staging/${compilerName}_HEADERS/rose_edg_required_macros_and_functions.h"
         # sed -i ".original" "/REPLACE_ME_WITH_GENERATED_BUILTIN_FUNCTIONS/r./include-staging/${compilerName}_HEADERS/rose_generated_builtin_functions.h" "./include-staging/${compilerName}_HEADERS/rose_edg_required_macros_and_functions.h"
           if test "x$build_vendor" = "xapple"; then
              sed -i ".original" "/REPLACE_ME_WITH_GENERATED_BUILTIN_FUNCTIONS/r./include-staging/${compilerName}_HEADERS/rose_generated_builtin_functions.h" "./include-staging/${compilerName}_HEADERS/rose_edg_required_macros_and_functions.h"
           else
            # DQ (2/25/2017): We need to support a different version of the buildin function support for EDG 4.9 because later versions 
            # of this generated file cause the tests/roseTests/astFileIOTests makefile rule parallelMerge_short and parallelMerge_medium
            # to fail randomly.  This can only so far be traced to it failing for EDG 4.9, but not EDG 4.12.  So the only practical solution 
            # is to build a version of the rose_edg_required_macros_and_functions.h file specific for EDG 4.9 while we are in the transition 
            # from EDG 4.9 to EDG 4.12.  I can't figure out the bug that is in EDG 4.9 and since it is fixed in later versions of EDG it
            # is not pratical to focus more time on this issue.
            # sed -i "/REPLACE_ME_WITH_GENERATED_BUILTIN_FUNCTIONS/r./include-staging/${compilerName}_HEADERS/rose_generated_builtin_functions.h" "./include-staging/${compilerName}_HEADERS/rose_edg_required_macros_and_functions.h"
              if test "$edg_minor_version_number" -le "9"; then
                 AC_MSG_NOTICE([building EDG 4.9 specific version of rose_edg_required_macros_and_functions.h])
                 sed -i "/REPLACE_ME_WITH_GENERATED_BUILTIN_FUNCTIONS/r./include-staging/${compilerName}_HEADERS/rose_generated_builtin_functions.h" "./include-staging/${compilerName}_HEADERS/rose_edg_required_macros_and_functions_EDG_49.h"
                 cp ./include-staging/${compilerName}_HEADERS/rose_edg_required_macros_and_functions_EDG_49.h ./include-staging/${compilerName}_HEADERS/rose_edg_required_macros_and_functions.h
              else
                 AC_MSG_NOTICE([building EDG 4.12 (and later) specific version of rose_edg_required_macros_and_functions.h])
                 sed -i "/REPLACE_ME_WITH_GENERATED_BUILTIN_FUNCTIONS/r./include-staging/${compilerName}_HEADERS/rose_generated_builtin_functions.h" "./include-staging/${compilerName}_HEADERS/rose_edg_required_macros_and_functions.h"
              fi
           fi

         # echo "ERROR: Could not identify the EDG minor version number."
         # exit 1
   else
     if test "x$edg_major_version_number" = "x5" || test "x$edg_major_version_number" = "x6"; then
       AC_MSG_NOTICE([building EDG 5.0 (and later) specific version of rose_generated_builtin_functions.h (same as EDG 4.12)])
       ${srcdir}/scripts/builtinLlvmFunctions.pl --constexpr=${srcdir}/config/constexpr_builtins.def ${srcdir}/config/Builtins.def > ./include-staging/${compilerName}_HEADERS/rose_generated_builtin_functions.h

       if test "x$build_vendor" = "xapple"; then
         sed -i ".original" "/REPLACE_ME_WITH_GENERATED_BUILTIN_FUNCTIONS/r./include-staging/${compilerName}_HEADERS/rose_generated_builtin_functions.h" "./include-staging/${compilerName}_HEADERS/rose_edg_required_macros_and_functions.h"
       else
         sed -i "/REPLACE_ME_WITH_GENERATED_BUILTIN_FUNCTIONS/r./include-staging/${compilerName}_HEADERS/rose_generated_builtin_functions.h" "./include-staging/${compilerName}_HEADERS/rose_edg_required_macros_and_functions.h"
       fi
     else
        AC_MSG_FAILURE([future versions of EDG 7.x and later version builtins maybe determined using a new mechanism that is more complete than older versions (so we don't require our ROSE specific built-in mechanism)])
     fi
   fi

 # "./include-staging/${compilerName}_HEADERS/rose_generated_builtin_functions.h"

# echo "Exiting as a test in GENERATE BACKEND CXX COMPILER SPECIFIC HEADERS"
# exit 1
])


AC_DEFUN([SETUP_BACKEND_CXX_COMPILER_SPECIFIC_REFERENCES],
dnl DQ 12/17/2001 build from what Bobby put into configure.in directly 11/25/2001
dnl This builds the directories required for the back-end compiler specific header files.
dnl it depends upon the CHOOSE BACKEND COMPILER macro to have already been called.
[
 # Now setup the include path that we will prepend to any user -I<dir> options so that the 
 # required compiler-specific header files can be found (these are often relocated versions 
 # of the compiler specific header files that have been processed so that EDG can read them)
 # It is unfortunate, but many compiler-specific files include compiler-specific code which
 # will not compile with a standard C++ compiler or can not be processed using a standard
 # C preprocessor (cpp) (an ugly fact of common compilers).

   chmod u+x "${srcdir}/$ROSE_HOME/config/dirincludes"

 #Mac OS X (and possibly other BSD-distros) does not support the echo -n option.
 #We need to detect this special case and use a "\c" in the end of the echo to not print a
 #newline.
   er=`echo -n ""`
   if test "X$er" = "X-n "
   then
     EC="\c"
     EO=""
   else
     EC=""
     EO="-n"
   fi

compilerNameCxx="`basename ${BACKEND_CXX_COMPILER}`"

 # DQ (11/1/2011): We need this same mechanism for C++'s use of EDG 4.x as we did for EDG 3.3 (but for C code this was not required; and was simpler).
 # Include the directory with the subdirectories of header files
 # if test "x$enable_new_edg_interface" = "xyes"; then
 #   includeString="{`${srcdir}/config/get_compiler_header_dirs ${BACKEND_CXX_COMPILER} | while read dir; do echo -n \\\"$dir\\\",\ ; done` \"/usr/include\"}"
 # else
 #   includeString="{\"${compilerNameCxx}_HEADERS\"`${srcdir}/$ROSE_HOME/config/dirincludes "./include-staging/" "${compilerNameCxx}_HEADERS"`, `${srcdir}/config/get_compiler_header_dirs ${BACKEND_CXX_COMPILER} | while read dir; do echo $EO \\\"$dir\\\",$EC\ ; done` \"/usr/include\"}"
 # fi
 # includeString="{\"${compilerNameCxx}_HEADERS\"`${srcdir}/$ROSE_HOME/config/dirincludes "./include-staging/" "${compilerNameCxx}_HEADERS"`, `${srcdir}/config/get_compiler_header_dirs ${BACKEND_CXX_COMPILER} | while read dir; do echo $EO \\\"$dir\\\",$EC\ ; done` \"/usr/include\"}"

 # Set new variables to pass in the function call below.
   compilerVendorName="$BACKEND_CXX_COMPILER_VENDOR"
   language="c++"

 # DQ (2/2/2016): Adding additional parameters to this function call.
 # if ! compilerHeaderDirs="$(${srcdir}/config/get_compiler_header_dirs ${BACKEND_CXX_COMPILER} | while read dir; do echo $EO \"$dir\",$EC\ ; done; exit ${PIPESTATUS[0]})"; then
 # [Matzke 2016-05-18]: Square brackets are quote characters in M4, so use "@<:@" and "@:>@" quadrigraphs instead. Ugly!
   if ! compilerHeaderDirs="$(env VERBOSE="$verbose" ${srcdir}/config/get_compiler_header_dirs ${BACKEND_CXX_COMPILER} ${language} ${compilerVendorName} | while read dir; do echo $EO \"$dir\",$EC\ ; done; exit ${PIPESTATUS@<:@0@:>@})"; then
      AC_MSG_FAILURE([$compilerHeaderDirs])
   fi

 # DQ (2/2/2016): Added error checking (though this already appears to be present).
 # [Matzke 2016-05-18]: Square brackets are quote characters in M4, so use "@<:@" and "@:>@" quadrigraphs instead. Ugly!
   if test "${PIPESTATUS@<:@0@:>@}" -ne 0; then
       AC_MSG_FAILURE([in SETUP BACKEND CXX COMPILER SPECIFIC REFERENCES: get_compiler_header_dirs failed])
   fi

 # echo "compilerHeaderDirs = $compilerHeaderDirs"
 # echo "{compilerNameCxx}_HEADERS = ${compilerNameCxx}_HEADERS"
 # temp_directory_list=`${srcdir}/$ROSE_HOME/config/dirincludes "./include-staging/" "${compilerNameCxx}_HEADERS"`
 # echo "temp_directory_list = $temp_directory_list"
 # echo "space"
 # echo "space"

 # temp_includeString="{\"${compilerNameCxx}_HEADERS\"`${srcdir}/$ROSE_HOME/config/dirincludes "./include-staging/" "${compilerNameCxx}_HEADERS"`, $compilerHeaderDirs"
 # echo "temp_includeString = $temp_includeString"
 # echo "space"
 # echo "space"

   includeString="{\"${compilerNameCxx}_HEADERS\"`${srcdir}/$ROSE_HOME/config/dirincludes "./include-staging/" "${compilerNameCxx}_HEADERS"`, $compilerHeaderDirs"

   if test "x$build_vendor" = "xapple"; then
     xcodeSDKPath=`xcrun --show-sdk-path`
     includeString="$includeString \"$xcodeSDKPath/usr/include\"}"
   else
     includeString="$includeString \"/usr/include\"}"
   fi

   AC_MSG_NOTICE([includeString = "$includeString"])
   AC_DEFINE_UNQUOTED([CXX_INCLUDE_STRING],$includeString,[Include path for backend C++ compiler.])

 # echo "Exiting in setup backend cxx compiler specific references"
 # exit 1;

])

AC_DEFUN([GENERATE_BACKEND_C_COMPILER_SPECIFIC_HEADERS],
[
   compilerName="`basename $BACKEND_C_COMPILER`"

   # echo "C compilerName = ${compilerName}"
   # echo "In GENERATE BACKEND C COMPILER SPECIFIC HEADERS: Using back-end C++ compiler = \"$BACKEND_CXX_COMPILER\" compiler vendor name = $ax_cv_cxx_compiler_vendor for processing of unparsed source files from ROSE preprocessors."

   chmod u+x "${srcdir}/config/create_system_headers"

   if test "$ROSE_C_HEADERS_DIR" = ""; then
      dnl AC_MSG_NOTICE([ROSE_C_HEADERS_DIR not set ...])
      ROSE_C_HEADERS_DIR="${compilerName}_HEADERS"
   else
      AC_MSG_NOTICE([ROSE_C_HEADERS_DIR = "$ROSE_C_HEADERS_DIR"])
   fi

   saveCurrentDirectory="`pwd`"
   cd "$srcdir"
   absolutePath_srcdir="`pwd`"
   cd "$saveCurrentDirectory"

 # DQ (9/1/2009): Output the absolute path
   AC_MSG_NOTICE([absolutePath_srcdir = "${absolutePath_srcdir}"])

 # This is a way to make this a global shaell variable, but it is better to pass it as a parameter to the function.
 # export language="c"
   language="c"
 # echo "In generate backend compiler specific headers: language = $language"

 # Use the full path name to generate the header from the correctly specified version of the backend compiler
   mkdir -p "./include-staging/${compilerName}_HEADERS"
 # "${srcdir}/config/create_system_headers" "${BACKEND_C_COMPILER}" "./include-staging/${compilerName}_HEADERS" "${absolutePath_srcdir}"
   "${srcdir}/config/create_system_headers" "${BACKEND_C_COMPILER}" "./include-staging/${compilerName}_HEADERS" "${absolutePath_srcdir}" "${language}" "$BACKEND_CXX_COMPILER_VENDOR"

   error_code=$?
   AC_MSG_NOTICE([error_code = "$error_code"])
   if test $error_code != 0; then
        AC_MSG_FAILURE([in generate backend C compiler specific headers: call to ${srcdir}/config/create_system_headers: nonzero exit code returned to caller error_code = "$error_code"])
   fi

 # DQ (2/2/2016): Debugging new support for detected the compiler vendor.
 # echo "Exiting after call to create_system_headers in GENERATE BACKEND C COMPILER SPECIFIC HEADERS"
 # exit 1

 # DQ (9/15/2010): Copy the upc.h header file from the config directory to our include-staging/${compilerName}_HEADERS directory.
 # It might be that these should be put into a UPC specific subdirectory (so that the C compiler can't accedentally find them), but this should be discussed.
   AC_MSG_NOTICE([copying UPC header files into ./include-staging/${compilerName}_HEADERS directory])
   cp ${srcdir}/config/upc.h ./include-staging/${compilerName}_HEADERS
   cp ${srcdir}/config/upc_io.h ./include-staging/${compilerName}_HEADERS
   cp ${srcdir}/config/upc_relaxed.h ./include-staging/${compilerName}_HEADERS
   cp ${srcdir}/config/upc_strict.h ./include-staging/${compilerName}_HEADERS
   cp ${srcdir}/config/upc_collective.h ./include-staging/${compilerName}_HEADERS
   cp ${srcdir}/config/bupc_extensions.h ./include-staging/${compilerName}_HEADERS

 # DQ (8/22/2011): Added support for SSE.
 # Copy alternative SSE and MMX headers to be seen by ROSE ahead of the originals.
   cp ${srcdir}/config/rose_specific_emmintrin.h ./include-staging/${compilerName}_HEADERS/emmintrin.h
   cp ${srcdir}/config/rose_specific_xmmintrin.h ./include-staging/${compilerName}_HEADERS/xmmintrin.h
   cp ${srcdir}/config/rose_specific_mmintrin.h  ./include-staging/${compilerName}_HEADERS/mmintrin.h

 # Phlin (6/18/2012): Added support for SSE4.2.
   cp ${srcdir}/config/rose_specific_ammintrin.h ./include-staging/${compilerName}_HEADERS/ammintrin.h
   cp ${srcdir}/config/rose_specific_nmmintrin.h ./include-staging/${compilerName}_HEADERS/nmmintrin.h
   cp ${srcdir}/config/rose_specific_pmmintrin.h ./include-staging/${compilerName}_HEADERS/pmmintrin.h
   cp ${srcdir}/config/rose_specific_smmintrin.h ./include-staging/${compilerName}_HEADERS/smmintrin.h
   cp ${srcdir}/config/rose_specific_tmmintrin.h ./include-staging/${compilerName}_HEADERS/tmmintrin.h

# Phlin (6/18/2012): Added support for AVX.
# Only GCC 4.6+ supports AVX instructions.
   if test x$BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER == x4; then
      if test "$BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER" -ge "6"; then
   cp ${srcdir}/config/rose_specific_avxintrin.h ./include-staging/${compilerName}_HEADERS/avxintrin.h
      fi
   fi

   error_code=$?
   AC_MSG_NOTICE([error_code = "$error_code"])
   if test $error_code != 0; then
        AC_MSG_FAILURE([in copying of upc.h header file: nonzero exit code returned to caller error_code = "$error_code"])
   fi
])


AC_DEFUN([SETUP_BACKEND_C_COMPILER_SPECIFIC_REFERENCES],
dnl DQ 12/17/2001 build from what Bobby put into configure.in directly 11/25/2001
dnl This builds the directories required for the back-end compiler specific header files.
dnl it depends upon the CHOOSE BACKEND COMPILER macro to have already been called.
[
 # Now setup the include path that we will prepend to any user -I<dir> options so that the 
 # required compiler-specific header files can be found (these are often relocated versions 
 # of the compiler specific header files that have been processed so that EDG can read them)
 # It is unfortunate, but many compiler-specific files include compiler-specific code which
 # will not compile with a standard C++ compiler or can not be processed using a standard
 # C preprocessor (cpp) (an ugly fact of common compilers).

   chmod u+x ${srcdir}/$ROSE_HOME/config/dirincludes

 #Mac OS X (and possibly other BSD-distros) does not support the echo -n option.
 #We need to detect this special case and use a "\c" in the end of the echo to not print a
 #newline.
   er=`echo -n ""`
   if test "X$er" = "X-n "
   then
     EC="\c"
     EO=""
   else
     EC=""
     EO="-n"
   fi

compilerNameC="`basename $BACKEND_C_COMPILER`"

 # DQ (11/1/2011): We need this same mechanism for C++'s use of EDG 4.x as we did for EDG 3.3 (but for C code this was not required; and was simpler).
 # Include the directory with the subdirectories of header files
 # if test "x$enable_new_edg_interface" = "xyes"; then
 #   includeString="{`${srcdir}/config/get_compiler_header_dirs ${BACKEND_C_COMPILER} | while read dir; do echo -n \\\"$dir\\\",\ ; done` \"/usr/include\"}"
 # else
 #   includeString="{\"${compilerNameC}_HEADERS\"`${srcdir}/$ROSE_HOME/config/dirincludes "./include-staging/" "${compilerNameC}_HEADERS"`, `${srcdir}/config/get_compiler_header_dirs ${BACKEND_C_COMPILER} | while read dir; do echo $EO \\\"$dir\\\",$EC\ ; done` \"/usr/include\"}"
 # fi
 #  includeString="{\"${compilerNameC}_HEADERS\"`${srcdir}/$ROSE_HOME/config/dirincludes "./include-staging/" "${compilerNameC}_HEADERS"`, `${srcdir}/config/get_compiler_header_dirs ${BACKEND_C_COMPILER} | while read dir; do echo $EO \\\"$dir\\\",$EC\ ; done` \"/usr/include\"}"

   compilerVendorName="$BACKEND_CXX_COMPILER_VENDOR"
   language="c"

 # if ! compilerHeaderDirs="$(${srcdir}/config/get_compiler_header_dirs ${BACKEND_C_COMPILER} | while read dir; do echo $EO \"$dir\",$EC\ ; done; exit ${PIPESTATUS[0]})"; then
 # [Matzke 2016-05-18]: Square brackets are quote characters in M4, so use "@<:@" and "@:>@" quadrigraphs instead. Ugly!
   if ! compilerHeaderDirs="$(env VERBOSE="$verbose" ${srcdir}/config/get_compiler_header_dirs ${BACKEND_C_COMPILER} ${language} ${compilerVendorName} | while read dir; do echo $EO \"$dir\",$EC\ ; done; exit ${PIPESTATUS@<:@0@:>@})"; then
      AC_MSG_FAILURE([$compilerHeaderDirs])
   fi

 # DQ (2/2/2016): Added error checking (though this already appears to be present).
 # [Matzke 2016-05-18]: Square brackets are quote characters in M4, so use "@<:@" and "@:>@" quadrigraphs instead. Ugly!
   if test "${PIPESTATUS@<:@0@:>@}" -ne 0; then
       AC_MSG_FAILURE([in SETUP BACKEND C COMPILER SPECIFIC REFERENCES: get_compiler_header_dirs failed])
   fi

 # temp_includeString="{\"${compilerNameC}_HEADERS\"`${srcdir}/$ROSE_HOME/config/dirincludes "./include-staging/" "${compilerNameC}_HEADERS"`, $compilerHeaderDirs"
 # echo "temp_includeString = $temp_includeString"
 # echo "\n\n"

   includeString="{\"${compilerNameC}_HEADERS\"`${srcdir}/$ROSE_HOME/config/dirincludes "./include-staging/" "${compilerNameC}_HEADERS"`, $compilerHeaderDirs"
   if test "x$build_vendor" = "xapple"; then
     xcodeSDKPath=`xcrun --show-sdk-path`
     includeString="$includeString \"$xcodeSDKPath/usr/include\"}"
   else
     includeString="$includeString \"/usr/include\"}"
   fi

   AC_MSG_NOTICE([includeString = "$includeString"])
   AC_DEFINE_UNQUOTED([C_INCLUDE_STRING],$includeString,[Include path for backend C compiler.])

# DQ (2/21/2017): Need to add required header file to support Intel compiler because we are using 
# the __INTEL_CLANG_COMPILER macro to use EDG with the Intel header files.
   if test "x$BACKEND_CXX_COMPILER_VENDOR" = "xintel"; then
#     echo "SETUP BACKEND C COMPILER: Copying config/rose_specific_tgmath_clang.h to ${compilerNameC}_HEADERS/tgmath_clang.h"
      cp ${srcdir}/config/rose_specific_tgmath_clang.h ./include-staging/${compilerNameC}_HEADERS/tgmath_clang.h
#     echo "DONE: SETUP BACKEND C COMPILER: Copying config/rose_specific_tgmath_clang.h to ${compilerNameC}_HEADERS/tgmath_clang.h"
   fi

# echo "Exiting as a test in SETUP BACKEND C COMPILER SPECIFIC REFERENCES: BACKEND_CXX_COMPILER_VENDOR = $BACKEND_CXX_COMPILER_VENDOR"
# exit 1
])

