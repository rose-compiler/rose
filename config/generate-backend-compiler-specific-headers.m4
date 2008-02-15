AC_DEFUN([GENERATE_BACKEND_CXX_COMPILER_SPECIFIC_HEADERS],
dnl DQ 12/17/2001 build from what Bobby put into configure.in directly 11/25/2001
dnl This builds the directories required for the back-end compiler specific header files.
dnl it depends upon the CHOOSE BACKEND COMPILER macro to have already been called.
[
 # BP : 11/20/2001, create a directory to store header files which are compiler specific
   compilerName=`basename $BACKEND_CXX_COMPILER`
 # test -d ${BACKEND_CXX_COMPILER}_HEADERS && rm -rf ${BACKEND_CXX_COMPILER}_HEADERS
 # mkdir ${BACKEND_CXX_COMPILER}_HEADERS

 # DQ (9/17/2006): We no longer want to build this in the build tree (I think).
 # test -d ${compilerName}_HEADERS && rm -rf ${compilerName}_HEADERS
 # mkdir ${compilerName}_HEADERS

   chmod u+x ${srcdir}/config/create_system_headers

 # if ROSE_HEADERS_DIR is not setup yet then set it up here
 # the default is to set it up using the header files from the
 # back-end compiler.  This is the safest default though it
 # is problematic for unsupported compilers and versions of
 # GNU's compilers greater than 2.96.
 # ROSE_HEADERS_DIR="`pwd`/${BACKEND_CXX_COMPILER}_HEADERS"
   if test "$ROSE_CXX_HEADERS_DIR" = ""; then
      AC_MSG_NOTICE([ROSE_CXX_HEADERS_DIR not set ...])
    # ROSE_CXX_HEADERS_DIR="`pwd`/${BACKEND_CXX_COMPILER}_HEADERS"
    # ROSE_CXX_HEADERS_DIR="`pwd`/${compilerName}_HEADERS"
      ROSE_CXX_HEADERS_DIR="${prefix}/include/${compilerName}_HEADERS"
   else
      AC_MSG_NOTICE([ROSE_CXX_HEADERS_DIR set to: $ROSE_CXX_HEADERS_DIR])
   fi

 # echo "Before calling create system_headers ROSE_CXX_HEADERS_DIR = $ROSE_CXX_HEADERS_DIR"
 # echo "Before calling create system_headers pwd = `pwd`"
 # echo "Before calling create system_headers prefix = ${prefix}"
 # echo "Before calling create system_headers prefix = $prefix"

 # Make the directory where we will place the ROSE version of the header files
 # mkdir -p $prefix/include
 # echo "Built directory structure for $prefix/include"

   saveCurrentDirectory=`pwd`
   cd $srcdir
 # echo "After cd to srcdir: pwd = `pwd`"
   absolutePath_srcdir=`pwd`
   cd $saveCurrentDirectory

 # echo "current directory = `pwd`"
 # echo "absolutePath_srcdir = $absolutePath_srcdir"

 # Providing an absolute path permits the use of a relative path with the configure script
 # ${srcdir}/config/create_system_headers "${BACKEND_CXX_COMPILER}" "$ROSE_HEADERS_DIR" "${srcdir}"
 # ${srcdir}/config/create_system_headers "${BACKEND_CXX_COMPILER}" "$ROSE_HEADERS_DIR" "${absolutePath_srcdir}"
 # ${srcdir}/config/create_system_headers "${compilerName}" "$ROSE_HEADERS_DIR" "${absolutePath_srcdir}"

 # Use the full path name to generate the header from the correctly specified version of the backend compiler
   mkdir -p "./include-staging/${BACKEND_CXX_COMPILER}_HEADERS"
   ${srcdir}/config/create_system_headers "${BACKEND_CXX_COMPILER}" "./include-staging/${BACKEND_CXX_COMPILER}_HEADERS" "${absolutePath_srcdir}"
])


AC_DEFUN([SETUP_BACKEND_CXX_COMPILER_SPECIFIC_REFERENCES],
dnl DQ 12/17/2001 build from what Bobby put into configure.in directly 11/25/2001
dnl This builds the directories required for the back-end compiler specific header files.
dnl it depends upon the CHOOSE BACKEND COMPILER macro to have already been called.
[
 # BP : 11/20/2001, see if my macro works
 # these commands have to follow the create system headers because that's where the directories
 # are created

 # DQ (9/17/2006): These should be the same for both C and C++ (else we will need separate macros)
 # Setup the -D<xxx> defines required to allow EDG to take the same path through the compiler 
 # specific and system specific header files as for the backend compiler.  These depend
 # upon the selection of the back-end compiler.
 # GET_COMPILER_SPECIFIC_DEFINES

 # Now setup the include path that we will prepend to any user -I<dir> options so that the 
 # required compiler-specific header files can be found (these are often relocated versions 
 # of the compiler specific header files that have been processed so that EDG can read them)
 # It is unfortunate, but many compiler-specific files include compiler-specific code which
 # will not compile with a standard C++ compiler or can not be processed using a standard
 # C preprocessor (cpp) (an ugly fact of common compilers).

 # Experiment with Intel header files (this variable is now set in ROSE.m4)
 # ROSE_HEADERS_DIR="`pwd`/$ROSE_HOME/${BACKEND_CXX_COMPILER}_HEADERS"
 # ROSE_HEADERS_DIR="${srcdir}/INTEL_HEADERS"
 # ROSE_HEADERS_DIR="${srcdir}/GNU_HEADERS"

 # echo "Using ROSE_HEADERS_DIR = $ROSE_HEADERS_DIR to generate include direcives"
   chmod u+x ${srcdir}/$ROSE_HOME/config/dirincludes

 # Include the directory with the subdirectories of header files
 # includeString="`${srcdir}/$ROSE_HOME/config/dirincludes "$ROSE_CXX_HEADERS_DIR"`"
   includeString="{\"${BACKEND_CXX_COMPILER}_HEADERS\"`${srcdir}/$ROSE_HOME/config/dirincludes "./include-staging/" "${BACKEND_CXX_COMPILER}_HEADERS"`, \"/usr/include\"}"

   echo "includeString = $includeString"
   AC_DEFINE_UNQUOTED([CXX_INCLUDE_STRING],$includeString,[Include path for backend C++ compiler.])

 # echo "Exiting in generate-backend-compiler-specific-headers_m4"
 # exit 1
])

AC_DEFUN([GENERATE_BACKEND_C_COMPILER_SPECIFIC_HEADERS],
[
   compilerName=`basename $BACKEND_C_COMPILER`

   echo "C compilerName = ${compilerName}"

   chmod u+x ${srcdir}/config/create_system_headers

   if test "$ROSE_C_HEADERS_DIR" = ""; then
      AC_MSG_NOTICE([ROSE_C_HEADERS_DIR not set ...])
      ROSE_C_HEADERS_DIR="${compilerName}_HEADERS"
   else
      AC_MSG_NOTICE([ROSE_C_HEADERS_DIR set to: $ROSE_C_HEADERS_DIR])
   fi

 # echo "Before calling create system_headers ROSE_C_HEADERS_DIR = $ROSE_C_HEADERS_DIR"
 # echo "Before calling create system_headers BACKEND_C_COMPILER  = $BACKEND_C_COMPILER"
 # echo "Before calling create system_headers absolutePath_srcdir = ${absolutePath_srcdir}"

 # Make the directory where we will place the ROSE version of the header files
 # mkdir -p $prefix/include
 # echo "Built directory structure for $prefix/include"

   saveCurrentDirectory=`pwd`
   cd $srcdir
   absolutePath_srcdir=`pwd`
   cd $saveCurrentDirectory

 # Use the full path name to generate the header from the correctly specified version of the backend compiler
 # ${srcdir}/config/create_system_headers "${BACKEND_C_COMPILER}" "$ROSE_C_HEADERS_DIR" "${absolutePath_srcdir}"
 # ${srcdir}/config/create_system_headers "${BACKEND_C_COMPILER}" "$ROSE_C_HEADERS_DIR" "${absolutePath_srcdir}" || : exit 1
   mkdir -p "./include-staging/${BACKEND_C_COMPILER}_HEADERS"
   ${srcdir}/config/create_system_headers "${BACKEND_C_COMPILER}" "./include-staging/${BACKEND_C_COMPILER}_HEADERS" "${absolutePath_srcdir}"

   error_code=$?
   echo "error_code = $error_code"
   if test $error_code != 0; then
        echo "Error in ${srcdir}/config/create_system_headers: nonzero exit code returned to caller error_code = $error_code"
        exit 1
   fi

 # echo "Exiting after call to create system headers ... status = $status ..."
 # exit 1
])


AC_DEFUN([SETUP_BACKEND_C_COMPILER_SPECIFIC_REFERENCES],
dnl DQ 12/17/2001 build from what Bobby put into configure.in directly 11/25/2001
dnl This builds the directories required for the back-end compiler specific header files.
dnl it depends upon the CHOOSE BACKEND COMPILER macro to have already been called.
[
 # BP : 11/20/2001, see if my macro works
 # these commands have to follow the create system headers because that's where the directories
 # are created

 # DQ (9/17/2006): These should be the same for both C and C++ (else we will need separate macros)
 # Setup the -D<xxx> defines required to allow EDG to take the same path through the compiler 
 # specific and system specific header files as for the backend compiler.  These depend
 # upon the selection of the back-end compiler.
 # GET_COMPILER_SPECIFIC_DEFINES

 # Now setup the include path that we will prepend to any user -I<dir> options so that the 
 # required compiler-specific header files can be found (these are often relocated versions 
 # of the compiler specific header files that have been processed so that EDG can read them)
 # It is unfortunate, but many compiler-specific files include compiler-specific code which
 # will not compile with a standard C++ compiler or can not be processed using a standard
 # C preprocessor (cpp) (an ugly fact of common compilers).

 # Experiment with Intel header files (this variable is now set in ROSE.m4)
 # ROSE_HEADERS_DIR="`pwd`/$ROSE_HOME/${BACKEND_CXX_COMPILER}_HEADERS"
 # ROSE_HEADERS_DIR="${srcdir}/INTEL_HEADERS"
 # ROSE_HEADERS_DIR="${srcdir}/GNU_HEADERS"

 # echo "Using ROSE_HEADERS_DIR = $ROSE_HEADERS_DIR to generate include direcives"
   chmod u+x ${srcdir}/$ROSE_HOME/config/dirincludes

 # Include the directory with the subdirectories of header files
 # includeString="`${srcdir}/$ROSE_HOME/config/dirincludes "$ROSE_C_HEADERS_DIR"`"
   includeString="{\"${BACKEND_C_COMPILER}_HEADERS\"`${srcdir}/$ROSE_HOME/config/dirincludes "./include-staging/" "${BACKEND_C_COMPILER}_HEADERS"`, \"/usr/include\"}"

   echo "includeString = $includeString"
   AC_DEFINE_UNQUOTED([C_INCLUDE_STRING],$includeString,[Include path for backend C compiler.])

 # echo "Exiting in generate-backend-compiler-specific-headers_m4"
 # exit 1
])

