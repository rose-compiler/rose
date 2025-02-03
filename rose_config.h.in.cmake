/* -*- c -*-
 *
 * DQ (10/16/2010): Comment added to clarify how this works and how to set macros.  This is not an automatically generated
 * file, it is checked into git as a regular file. Then it is used to generate the rose_config.h file in the cmake build tree.
 *
 * If new macros are defined they should be defined using either define or cmakedefine.  Define macros will always be defined
 * however they are set in this file, while cmakedefine will be set or left unset based on the cmake configurations step
 * (running cmake before running make within the build process).
 *
 * Since this file is contains name-space pollution (symbols not beginning with "ROSE_") it must be processed in order to
 * create a non-polluting file (rosePublicConfig.h) for inclusion into user code (either directly or via other ROSE public
 * header files). The script that does this (publicConfiguration.pl) reads the rose_config.h file and copies (while renaming)
 * certain "#define" lines. Therefore, don't put the "#define" inside conditional compilation--the conditional compilation
 * will not be copied (and cannot be since the conditions contain polluting symbols).
 * 
 * TSG (6/26/24) ConfigureChecks.cmake is used to assign the appropriate value for these variables.
 *               We don't wan't to hard code anything in here
 * TSG (2/3/25)  It's best to not put logic in this file 
 *               Almost always do this: #define MYMACRO "@MYMACRO@"
 *               #cmakedefine01 for bool vars 
 *               Almost never do this: #define MYMACRO ${DIFFERENT_CMAKE_VARIABLE} 
 *               This creates potential mismatches and confusion
 */

// VERSIONING 
/* Git SCM version information for ROSE: commit identifier. */
#cmakedefine ROSE_SCM_VERSION_ID "@ROSE_SCM_VERSION_ID@"

/* Git SCM version information for ROSE: commit date (Unix timestamp). */
#cmakedefine ROSE_VERSION @ROSE_VERSION@

/* Define user-friendly ROSE version */
#cmakedefine ROSE_PACKAGE_VERSION "@ROSE_PACKAGE_VERSION@"

// REQUIRED LIBS that should be found in CMakeLists.txt -- find_package() */ 
/* Define to 1 if you have the `dl' library (-ldl). */
#cmakedefine HAVE_LIBDL @HAVE_LIBDL@

// TPL, also located with find_package() *
/* Define if the Boost library is available, convert the cmake output Boost_FOUND to our custom HAVE_BOOST */
#cmakedefine HAVE_BOOST @Boost_FOUND@

/* Define if the Boost::Date_Time library is available */
#cmakedefine HAVE_BOOST_DATE_TIME @Boost_DATE_TIME_FOUND@

/* define if multithread support */ 
#cmakedefine _REENTRANT @_REENTRANT@ 

/* Define if the Boost::Filesystem library is available */
#cmakedefine HAVE_BOOST_FILESYSTEM @Boost_FILESYSTEM_FOUND@

/* Define if the Boost::Program_Options library is available */
#cmakedefine HAVE_BOOST_PROGRAM_OPTIONS @Boost_PROGRAM_OPTIONS_FOUND@

/* Define if the Boost::Regex library is available */
#cmakedefine HAVE_BOOST_REGEX @Boost_REGEX_FOUND@

/* Define if the Boost::System library is available */
#cmakedefine HAVE_BOOST_SYSTEM @Boost_SYSTEM_FOUND@

/* Define if the Boost::Thread library is available */
#cmakedefine HAVE_BOOST_THREAD @Boost_THREAD_FOUND@

/* Define if the Boost::Wave library is available */
#cmakedefine HAVE_BOOST_WAVE @Boost_WAVE_FOUND@

/* Define if the ROSE Boost Wave support is enabled */
#cmakedefine USE_ROSE_BOOST_WAVE_SUPPORT @USE_ROSE_BOOST_WAVE_SUPPORT@

/* Define if the boost::serialization library is available */
#cmakedefine HAVE_BOOST_SERIALIZATION_LIB @Boost_SERIALIZATION_FOUND@

/* Define if Z3 library is available */
#cmakedefine ROSE_HAVE_Z3 @ROSE_HAVE_Z3@

/* Location of Z3 executable */
#cmakedefine ROSE_Z3 "@ROSE_Z3@"

/* Define if z3_version.h is available. */
#cmakedefine ROSE_HAVE_Z3_VERSION_H @ROSE_HAVE_Z3_VERSION_H@

/* Define if libgcrypt is available. */
#cmakedefine ROSE_HAVE_LIBGCRYPT @ROSE_HAVE_LIBGCRYPT@

/* Define if YAML-CPP library is available. */
#cmakedefine ROSE_HAVE_YAMLCPP @ROSE_HAVE_YAMLCPP@

/* Define if Dlib is available. */
#cmakedefine ROSE_HAVE_DLIB @ROSE_HAVE_DLIB@

/* Define if Capstone is available. */
#cmakedefine ROSE_HAVE_CAPSTONE @ROSE_HAVE_CAPSTONE@

/* Define if Cereal is available. */
#cmakedefine ROSE_HAVE_CEREAL @ROSE_HAVE_CEREAL@

/* Define if libreadline is available. */
#cmakedefine ROSE_HAVE_LIBREADLINE @ROSE_HAVE_LIBREADLINE@
  
/* HEADERS_TO_CHECK 
 * These variables are defined to 1 if the respective header file is found
 * See headers_to_check in cmake/ConfigureChecks.cmake
 */

/* Define to 1 if you have the <argz.h> header file. */
#cmakedefine HAVE_ARGZ_H @HAVE_ARGZ_H@ 
/* These will be defined to 1 if we have the header above */ 
#cmakedefine HAVE_ARGZ_APPEND @HAVE_ARGZ_APPEND@
#cmakedefine HAVE_ARGZ_CREATE_SEP @HAVE_ARGZ_CREATE_SEP@
#cmakedefine HAVE_ARGZ_INSERT @HAVE_ARGZ_INSERT@
#cmakedefine HAVE_ARGZ_NEXT @HAVE_ARGZ_NEXT@
#cmakedefine HAVE_ARGZ_STRINGIFY @HAVE_ARGZ_STRINGIFY@
#cmakedefine HAVE_WORKING_ARGZ @HAVE_WORKING_ARGZ@

/* Define to 1 if you have the <c_asm.h> header file. */
#cmakedefine HAVE_C_ASM_H @HAVE_C_ASM@

/* HAVE_C_TYPE_H gets defined to 1 if <ctype.h> found  */
#cmakedefine HAVE_CTYPE_H @HAVE_CTYPE_H@

/* Define to 1 if you have the <dirent.h> header file, and it defines `DIR'. */
#cmakedefine HAVE_DIRENT_H @HAVE_DIRENT_H@

/* Define to 1 if you have the <dlfcn.h> header file. */
#cmakedefine HAVE_DLFCN_H @HAVE_DLFCN_H@

/* Define to 1 if you have the <dl.h> header file. */
#cmakedefine HAVE_DL_H @HAVE_DL_H@

/* Define to 1 if you have the <intrinsics.h> header file. */
#cmakedefine HAVE_INTRINSICS_H @HAVE_INTRINSICS_H@

/* Define to 1 if you have the <stdlib.h> header file. */
#cmakedefine HAVE_STDLIB_H @HAVE_STDLIB_H@ 

/* Define to 1 if you have the <sys/time.h> header file. */
#cmakedefine HAVE_SYS_TIME_H @HAVE_SYS_TIME_H@

/* Define to 1 if you have the <sys/utime.h> header file. */
#cmakedefine HAVE_SYS_UTIME_H @HAVE_SYS_UTIME_H@

/* Define to 1 if you have the `time_base_to_time' function. */
#cmakedefine HAVE_TIME_BASE_TO_TIME @HAVE_TIME_BASE_TO_TIME@

// The remaining headers below are not currently part of 
// cmake/ConfigureChecks.cmake. Some are processed in subdirectory cmake
// files, some may eventually be removed if they were intended for Autoconf  

/* Define to 1 if you have the <memory.h> header file. */
#cmakedefine HAVE_MEMORY_H @HAVE_MEMORY_H@
/* End of HEADERS_TO_CHECK */


/* FUNCTIONS_TO_CHECK   
 * These variables are defined to 1 if the respective function is found
 * See functions_to_check in cmake/ConfigureChecks.cmake
 */
/* Define to 1 if you have the `clock_gettime' function. */
#cmakedefine HAVE_CLOCK_GETTIME @HAVE_CLOCK_GETTIME@

/* Whether <dlfcn.h> and -ldl contain dladdr() */
#cmakedefine HAVE_DLADDR @HAVE_DLADDR@

/* Use explicit template instantiation for MSTL, ASTProcessing, and ROSETTA */
#cmakedefine HAVE_EXPLICIT_TEMPLATE_INSTANTIATION @HAVE_EXPLICIT_TEMPLATE_INSTANTIATION@

#cmakedefine HAVE_GETHRTIME 

/* Define to 1 if you have the `mach_absolute_time' function. */
#cmakedefine HAVE_MACH_ABSOLUTE_TIME @HAVE_MACH_ABSOLUTE_TIME@

/* End of FUNCTIONS_TO_CHECK */

// BACKEND SECTION
/* Define if building with Jovial support */ 
#cmakedefine ATERM_BIN_PATH "@ATERM_BIN_PATH@"
#cmakedefine ATERM_INSTALL_PATH "@ATERM_INSTALL_PATH@"
#cmakedefine ATERM_LIBRARY_PATH "@ATERM_LIBRARY_PATH@"

// defines for jovial_support.C 
#cmakedefine STRATEGO_BIN_PATH "@STRATEGO_BIN_PATH@"

// defines for cmdline.C
#cmakedefine BACKEND_JOVIAL_COMPILER_NAME_WITH_PATH "@BACKEND_JOVIAL_COMPILER_NAME_WITH_PATH@"

// defines for sage_support.C, SageTreeBuilder.C, and utility_functions.C 
#cmakedefine ROSE_EXPERIMENTAL_JOVIAL_ROSE_CONNECTION 

/* Define if building with Ada support */ 
#cmakedefine BACKEND_ADA_COMPILER_NAME_WITH_PATH "@BACKEND_ADA_COMPILER_NAME_WITH_PATH@"
#cmakedefine ROSE_EXPERIMENTAL_ADA_ROSE_CONNECTION 

/* Define this to "no" typically */ 
#cmakedefine ROSE_EXPERIMENTAL_ADA_ROSE_CONNECTION_GNAT_HOME "@ROSE_EXPERIMENTAL_ADA_ROSE_CONNECTION_GNAT_HOME@" 
#cmakedefine BACKEND_FORTRAN_IS_GNU_COMPILER @BACKEND_FORTRAN_IS_GNU_COMPILER@

/* Backend C and C++ (CXX) Macros */
/* BACKEND CXX -- No quotes around major, minor version numbers */
#cmakedefine BACKEND_CXX_IS_GNU_COMPILER @BACKEND_CXX_IS_GNU_COMPILER@ 
#define BACKEND_CXX_COMPILER_NAME_WITH_PATH "@BACKEND_CXX_COMPILER_NAME_WITH_PATH@"
#define BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH "@BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH@"
#define BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER @BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER@
#define BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER @BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER@
#define BACKEND_CXX_COMPILER_PATCH_VERSION_NUMBER @BACKEND_C_COMPILER_PATCH_VERSION_NUMBER@ 
/* BACKEND C */ 
#define BACKEND_C_COMPILER_NAME_WITH_PATH "@BACKEND_C_COMPILER_NAME_WITH_PATH@" 
#define BACKEND_C_COMPILER_NAME_WITHOUT_PATH "@BACKEND_C_COMPILER_NAME_WITHOUT_PATH@"
#define BACKEND_C_COMPILER_MAJOR_VERSION_NUMBER @BACKEND_C_COMPILER_MAJOR_VERSION_NUMBER@
#define BACKEND_C_COMPILER_MINOR_VERSION_NUMBER @BACKEND_C_COMPILER_MINOR_VERSION_NUMBER@ 
#define BACKEND_C_COMPILER_PATCH_VERSION_NUMBER @BACKEND_C_COMPILER_PATCH_VERSION_NUMBER@ 

/* (May or may not explicit include path; used to call backend). */
#define BACKEND_FORTRAN_COMPILER_NAME_WITH_PATH "${CMAKE_Fortran_COMPILER}"

#define BACKEND_FORTRAN_COMPILER_NAME_WITHOUT_PATH "${BACKEND_FORTRAN_COMPILER_NAME_WITHOUT_PATH}"

/* Major version number of backend Fortran compiler. */
#define BACKEND_FORTRAN_COMPILER_MAJOR_VERSION_NUMBER ${BACKEND_FORTRAN_COMPILER_MAJOR_VERSION_NUMBER}

/* Minor version number of backend Fortran compiler. */
#define BACKEND_FORTRAN_COMPILER_MINOR_VERSION_NUMBER ${BACKEND_FORTRAN_COMPILER_MINOR_VERSION_NUMBER}

/* Name of backend Java compiler including path */
#define BACKEND_JAVA_COMPILER_NAME_WITH_PATH "${BACKEND_JAVA_COMPILER}"

/* Name of backend Python interpreter including path */
#define BACKEND_PYTHON_INTERPRETER_NAME_WITH_PATH "${BACKEND_PYTHON_INTERPRETER_NAME_WITH_PATH}"
/* END OF BACKEND SECTION */

/* Include path for backend C++ compiler. */
#cmakedefine CXX_INCLUDE_STRING @CXX_INCLUDE_STRING@

/* Include path for backend C compiler. */
#cmakedefine C_INCLUDE_STRING @C_INCLUDE_STRING@

/* Define to 1 if MySQL libraries are available */
#cmakedefine MYSQL_FOUND
#ifdef MYSQL_FOUND
  #define HAVE_MYSQL
#endif

/* Have the SQLITE3 library */
#cmakedefine HAVE_SQLITE3

/* Define to 1 if you have the POSIX.1003 header file, <pthread.h> */
#cmakedefine HAVE_PTHREAD_H 

/* Prefix path for use of IDA. */
#define IDA_PRO_PATH "${with_ida}"

/* Path to JVM executable */
#define JAVA_JVM_PATH "${JAVA_RUNTIME}"

/* Location (unquoted) of Boost specified on configure line. */
#define ROSE_BOOST_PATH "${BOOST_ROOT}"

/* Location of ROSE Compile Tree. */
#define ROSE_COMPILE_TREE_PATH "@ROSE_COMPILE_TREE_PATH@"

/* Location of ROSE Source Tree. */
#define ROSE_SOURCE_TREE_PATH "@ROSE_SOURCE_TREE_PATH@"

/* PATH on Windows, LD_LIBRARY_PATH everywhere else. */
#define ROSE_SHLIBPATH_VAR "${ROSE_SHLIBPATH_VAR}"

/* Use Valgrind calls in ROSE */
//AS Don't know what to do with this
#cmakedefine ROSE_USE_VALGRIND

// Using define always defines as opposed to cmakedefine (conditional)
/* The size of `char', as computed by sizeof. */
#define SIZEOF_CHAR ${SIZEOF_CHAR}

/* The size of `double', as computed by sizeof. */
#define SIZEOF_DOUBLE ${SIZEOF_DOUBLE}

/* The size of `float', as computed by sizeof. */
#define SIZEOF_FLOAT ${SIZEOF_FLOAT}

/* The size of `int', as computed by sizeof. */
#define SIZEOF_INT ${SIZEOF_INT}

/* The size of `long', as computed by sizeof. */
#define SIZEOF_LONG ${SIZEOF_LONG}

/* The size of `long double', as computed by sizeof. */
#define SIZEOF_LONG_DOUBLE ${SIZEOF_LONG_DOUBLE}

/* The size of `long long', as computed by sizeof. */
#define SIZEOF_LONG_LONG ${SIZEOF_LONG_LONG}

/* The size of `short', as computed by sizeof. */
#define SIZEOF_SHORT ${SIZEOF_SHORT}

/* The size of `void *', as computed by sizeof. */
#define SIZEOF_VOID_P ${SIZEOF_VOID_P}

/* Define to 1 if you have the ANSI C header files. */
#cmakedefine STDC_HEADERS @STDC_HEADERS@ 

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#cmakedefine TIME_WITH_SYS_TIME @TIME_WITH_SYS_TIME@ 

/* Define to 1 if your <sys/time.h> declares `struct tm'. */
#cmakedefine TM_IN_SYS_TIME @TM_IN_SYS_TIME@

/* Mark that GFORTRAN is available */
#cmakedefine USE_GFORTRAN_IN_ROSE @USE_GFORTRAN_IN_ROSE@ 

/* Support for Purify */
#cmakedefine USE_PURIFY

/* Support for Purify API (Compiling with the Purify library). */
#cmakedefine USE_PURIFY_API

/* Link-time support for Purify. */
#cmakedefine USE_PURIFY_LINKER

/* Use of GCC OpenMP in ROSE. */
//AS Don't know what to do with this
#cmakedefine USE_ROSE_GCC_OMP

/* Controls use of IDA support for disassembling. */
//AS Don't know what to do with this
#cmakedefine USE_ROSE_IDA_SUPPORT

/* Controls use of ROSE support for Java. */
#cmakedefine JAVA_RUNTIME
#ifdef JAVA_RUNTIME
  #define USE_ROSE_JAVA_SUPPORT
#endif

/* Version number of package */
//AS Don't know what to do with this
#define VERSION "0.9.10"

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
#  undef WORDS_BIGENDIAN
# endif
#endif

/* DQ (10/16/2010): Added OFP required macros as part of move to exclude
 * rose_config.h from the ROSE header files.
 */
#define ROSE_OFP_MAJOR_VERSION_NUMBER 0
#define ROSE_OFP_MINOR_VERSION_NUMBER 8
#define ROSE_OFP_PATCH_VERSION_NUMBER 3

#define ROSE_OFP_VERSION_NUMBER "@ROSE_OFP_VERSION_NUMBER@"

/* EDG major version number */
#define ROSE_EDG_MAJOR_VERSION_NUMBER @EDG_MAJOR_VERSION@

/* EDG minor version number */
#define ROSE_EDG_MINOR_VERSION_NUMBER @EDG_MINOR_VERSION@

/* Controls if EDG Union/Struct debugging support is to be used. */
#cmakedefine01 USE_ROSE_EDG_DEBUGGING_SUPPORT

/* Defined if we're using a prebuilt EDG library */
#cmakedefine BINARY_EDG

/* Internal EDG specific consistency tests. */
//AS FIXME
#define CHECKING 1

/* -D options to hand to EDG C++ front-end. */
#define CXX_SPEC_DEF {"-D__GNUG__=${BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER}", "-D__GNUC__=${BACKEND_C_COMPILER_MAJOR_VERSION_NUMBER}", "-D__GNUC_MINOR__=${BACKEND_C_COMPILER_MINOR_VERSION_NUMBER}", "-D__GNUC_PATCHLEVEL__=${BACKEND_C_COMPILER_PATCH_LEVEL_NUMBER}", "--preinclude", "rose_edg_required_macros_and_functions.h"}

#cmakedefine01 ROSE_USE_EDG_QUAD_FLOAT 

/* Defined if we're building using CMake */
#cmakedefine USE_CMAKE @USE_CMAKE@

/* Build ROSE to support the Binary Analysis */
#cmakedefine ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

/* Build ROSE to support the Cuda langauge */
#cmakedefine ROSE_BUILD_CUDA_LANGUAGE_SUPPORT

/* Build ROSE to support the C++ langauge */
#cmakedefine ROSE_BUILD_CXX_LANGUAGE_SUPPORT

/* Build ROSE to support the C langauge */
#cmakedefine ROSE_BUILD_C_LANGUAGE_SUPPORT

/* Build ROSE to support the C preprocessor. */
#cmakedefine ROSE_BUILD_CPP_LANGUAGE_SUPPORT

/* Build ROSE to support the Fortran langauge */
#cmakedefine ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT

/* Build ROSE to support the Java langauge */
#cmakedefine ROSE_BUILD_JAVA_LANGUAGE_SUPPORT
#cmakedefine USE_ROSE_INTERNAL_JAVA_SUPPORT

/* Build ROSE to support the OpenCL langauge */
#cmakedefine ROSE_BUILD_OPENCL_LANGUAGE_SUPPORT

/* Build ROSE to support the PHP langauge */
#cmakedefine ROSE_BUILD_PHP_LANGUAGE_SUPPORT

/* Build ROSE to support the Python langauge */
#cmakedefine ROSE_BUILD_PYTHON_LANGUAGE_SUPPORT

/* Support for an advanced uniform warning level for ROSE development */
#cmakedefine ROSE_USE_UNIFORM_ADVANCED_WARNINGS_SUPPORT

#cmakedefine ROSE_SUPPORT_GNU_EXTENSIONS
#cmakedefine ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
#cmakedefine ROSE_SUPPORT_MICROSOFT_EXTENSIONS

/* Detect whether our compilers are GNU or not */
#cmakedefine CMAKE_COMPILER_IS_GNUCC
#cmakedefine CMAKE_COMPILER_IS_GNUCXX
#cmakedefine CMAKE_COMPILER_IS_GNUG77

/* Create the configuration synopsis token for use by Rose::initialize */
#define ROSE_CONFIG_TOKEN "@ROSE_CONFIG_TOKEN@"

/* Simple preprocessor as default in ROSE -- dont delete me */
//AS Need configure option to allow wave to be default
#define ROSE_WAVE_DEFAULT false 

#define SAGE_TRANSFORM 1

/* Define to 1 if you have the `read_real_time' function. */
#cmakedefine HAVE_READ_REAL_TIME @HAVE_READ_REAL_TIME@ 
 
/* Define to 1 if you have the <machine/endian.h> header file. */
#cmakedefine HAVE_MACHINE_ENDIAN_H @HAVE_MACHINE_ENDIAN_H@ 

/* Define to 1 if you have the <mach/mach_time.h> header file. */
#cmakedefine HAVE_MACH_MACH_TIME_H @HAVE_MACH_MACH_TIME_H@ 
