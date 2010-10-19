/* ---  rose_config.h.cmake  ---
 * DQ (10/16/2010): Comment added to clarify how this works and how to set macros.
 * This is not an automatically generated file, it is checked into git as
 * a regular file. Then it is used to generate the rose_config.h file in the
 * cmake build tree.
 * If new macros are defined they should be defined using either 
 * define or cmakedefine.  define macros will always be defined
 * however they are set in this file, while cmakedefine will be
 * set or left unset based on the cmake configurations step
 * (running cmake before running make within the build process).
 */

/* Define to 1 if you have the `argz_append' function. */
#cmakedefine HAVE_ARGZ_APPEND 1
/* Define to 1 if you have the `argz_create_sep' function. */
#cmakedefine HAVE_ARGZ_CREATE_SEP 1
/* Define to 1 if you have the `argz_insert' function. */
#cmakedefine HAVE_ARGZ_INSERT 1
/* Define to 1 if you have the `argz_next' function. */
#cmakedefine HAVE_ARGZ_NEXT 1
/* Define to 1 if you have the `argz_stringify' function. */
#cmakedefine HAVE_ARGZ_STRINGIFY 1
/* Define to 1 if you have the <argz.h> header file. */
#cmakedefine HAVE_ARGZ_H 1

/* define if the Boost library is available , convert the cmake output Boost_FOUND to our custom HAVE_BOOST */
#cmakedefine Boost_FOUND 
#ifdef Boost_FOUND
  #define HAVE_BOOST
#endif

/* define if the Boost::Date_Time library is available */
#cmakedefine Boost_DATE_TIME_FOUND
#ifdef Boost_DATE_TIME_FOUND
  #define HAVE_BOOST_DATE_TIME
#endif

/* define if the Boost::Filesystem library is available */
#cmakedefine Boost_FILESSYSTEM_FOUND
#ifdef Boost_FILESSYSTEM_FOUND
  #define HAVE_BOOST_FILESYSTEM 
#endif
/* define if the Boost::PROGRAM_OPTIONS library is available */
#cmakedefine Boost_PROGRAM_OPTIONS_FOUND
#ifdef Boost_PROGRAM_OPTIONS_FOUND
  #define HAVE_BOOST_PROGRAM_OPTIONS
#endif

/* define if the Boost::Regex library is available */
#cmakedefine Boost_REGEX_FOUND
#ifdef Boost_REGEX_FOUND
  #define HAVE_BOOST_REGEX
#endif

/* define if the Boost::System library is available */
#cmakedefine Boost_SYSTEM_FOUND
#ifdef Boost_SYSTEM_FOUND
  #define HAVE_BOOST_SYSTEM
#endif

/* define if the Boost::Thread library is available */
#cmakedefine Boost_THREAD_FOUND
#ifdef Boost_THREAD_FOUND
  #define HAVE_BOOST_THREAD
#endif


/* define if the Boost::Wave library is available */
#cmakedefine Boost_WAVE_FOUND
#ifdef Boost_WAVE_FOUND
  #define HAVE_BOOST_WAVE
  #define USE_ROSE_BOOST_WAVE_SUPPORT
#endif

/* Define to 1 if you have the <byteswap.h> header file. */
#cmakedefine HAVE_BYTESWAP_H 1

/* Define to 1 if you have the `closedir' function. */
#cmakedefine HAVE_CLOSEDIR 1

/* Define to 1 if you have the <ctype.h> header file. */
#cmakedefine HAVE_CTYPE_H 1

/* Define to 1 if you have the <c_asm.h> header file. */
#cmakedefine HAVE_C_ASM_H 1

/* Define to 1 if you have the <dirent.h> header file, and it defines `DIR'. */
#cmakedefine HAVE_DIRENT_H 1

/* Whether <dlfcn.h> and -ldl contain dladdr() */
#cmakedefine HAVE_DLADDR 

/* Define if you have the GNU dld library. */
//AS Seems unnecessary
/* #undef HAVE_DLD */

/* Define to 1 if you have the <dld.h> header file. */
//AS Seems unnecessary
/* #undef HAVE_DLD_H */

/* Define to 1 if you have the `dlerror' function. */
#cmakedefine HAVE_DLERROR 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#cmakedefine HAVE_DLFCN_H 1

/* Define to 1 if you have the <dl.h> header file. */
#cmakedefine HAVE_DL_H 1

/* Define to 1 if you don't have `vprintf' but do have `_doprnt.' */
#cmakedefine HAVE_DOPRNT 1

/* Define if you have the _dyld_func_lookup function. */
#cmakedefine HAVE_DYLD 

/* Define to 1 if you have the <errno.h> header file. */
#cmakedefine HAVE_ERRNO_H 1

/* Define to 1 if the system has the type `error_t'. */
//AS seems unnecessary. ROSE does not use it
//#define HAVE_ERROR_T 1

/* Use explicit template instantiation. */
//AS FIXME: Need to have a check for this
#define HAVE_EXPLICIT_TEMPLATE_INSTANTIATION 

/* Define to 1 if you have the <ffi.h> header file. */
#cmakedefine HAVE_FFI_H 1 

/* Define to 1 if you have the `getcwd' function. */
#cmakedefine HAVE_GETCWD 1

/* Define to 1 if you have the `gethrtime' function. */
#cmakedefine HAVE_GETHRTIME 1

/* Define to 1 if you have the `getpagesize' function. */
#cmakedefine HAVE_GETPAGESIZE 1

/* Define to 1 if you have the `getwd' function. */
#cmakedefine HAVE_GETWD 1

/* Define to 1 if hrtime_t is defined in <sys/time.h> */
#cmakedefine HAVE_HRTIME_T 1

/* Define to 1 if you have the `index' function. */
#cmakedefine HAVE_INDEX 1

/* Define to 1 if you have the <intrinsics.h> header file. */
#cmakedefine HAVE_INTRINSICS_H 

/* Define to 1 if you have the <inttypes.h> header file. */
#cmakedefine HAVE_INTTYPES_H 1


#cmakedefine HAVE_IO_H 1

/* Define to 1 if you have the <limits.h> header file. */
#cmakedefine HAVE_LIMITS_H 1
#cmakedefine HAVE_LOCALE_H 1
#cmakedefine HAVE_PROCESS_H 1
#cmakedefine HAVE_STDLIB_H 1
#cmakedefine HAVE_SYS_STAT_H 1
#cmakedefine HAVE_SYS_TIME_H 1
#cmakedefine HAVE_SYS_TYPES_H 1
#cmakedefine HAVE_SYS_UTIME_H 1
#cmakedefine HAVE_SYS_SOCKET_H 1
#cmakedefine HAVE_UNISTD_H 1
#cmakedefine HAVE_INTTYPES_H 1
#cmakedefine HAVE_UTIME_H 1
#cmakedefine HAVE_STRING_H 1
#cmakedefine HAVE_STRINGS_H 1
#cmakedefine HAVE_ISTREAM 1
#cmakedefine HAVE_OSTREAM 1
#cmakedefine HAVE_IOS 1
#cmakedefine HAVE_LOCALE 1
#cmakedefine HAVE_OPEN 1
#cmakedefine HAVE_CHMOD 1
#cmakedefine HAVE_CLOSE 1
#cmakedefine HAVE_DCGETTEXT 1
#cmakedefine HAVE_POPEN 1
#cmakedefine HAVE_PCLOSE 1
#cmakedefine HAVE__OPEN 1
#cmakedefine HAVE__CLOSE 1
#cmakedefine HAVE__POPEN 1
#cmakedefine HAVE__PCLOSE 1
#cmakedefine HAVE_GETPID 1
#cmakedefine HAVE__GETPID 1
#cmakedefine HAVE_GETTEXT 1
#cmakedefine HAVE_MKDIR 1
#cmakedefine HAVE__MKDIR 1
#cmakedefine HAVE_PUTENV 1
#cmakedefine HAVE_MKTEMP 1
#cmakedefine HAVE_MKSTEMP 1
#cmakedefine HAVE_STRERROR 1
#cmakedefine HAVE_STD_COUNT 1
#cmakedefine HAVE_ASPRINTF 1
#cmakedefine HAVE_WPRINTF 1
#cmakedefine HAVE_SNPRINTF 1
#cmakedefine HAVE_POSIX_PRINTF 1
#cmakedefine HAVE_FCNTL 1
#cmakedefine HAVE_INTMAX_T 1
#cmakedefine HAVE_INTTYPES_H_WITH_UINTMAX 1
#cmakedefine HAVE_DECL_ISTREAMBUF_ITERATOR 1
#cmakedefine CXX_GLOBAL_CSTD 1
#cmakedefine HAVE_GETCWD 1
#cmakedefine HAVE_STPCPY 1
#cmakedefine HAVE_STRCASECMP 1
#cmakedefine HAVE_STRDUP 1
#cmakedefine HAVE_STRTOUL 1
#cmakedefine HAVE___FSETLOCKING 1
#cmakedefine HAVE_MEMPCPY 1
#cmakedefine HAVE___ARGZ_COUNT 1
#cmakedefine HAVE___ARGZ_NEXT 1
#cmakedefine HAVE___ARGZ_STRINGIFY 1
#cmakedefine HAVE_SETLOCALE 1
#cmakedefine HAVE_TSEARCH 1
#cmakedefine HAVE_GETEGID 1
#cmakedefine HAVE_GETGID 1
#cmakedefine HAVE_GETUID 1
#cmakedefine HAVE_WCSLEN 1
#cmakedefine HAVE_MKFIFO 1
#cmakedefine HAVE_WPRINTF 1
/* Define to 1 if long double works and has more range or precision than
   double. */
#cmakedefine HAVE_LONG_DOUBLE 1
#cmakedefine HAVE_LONG_LONG 1
#cmakedefine HAVE_WCHAR_T 1
#cmakedefine HAVE_WINT_T 1
#cmakedefine HAVE_STDINT_H_WITH_UINTMAX 1
#cmakedefine HAVE_LC_MESSAGES 1    
#cmakedefine HAVE_SSTREAM 1
#cmakedefine HAVE_ARGZ_H 1
#cmakedefine SIZEOF_WCHAR_T_IS_2 1
#cmakedefine SIZEOF_WCHAR_T_IS_4 1

/* Name of backend C++ compiler including path (may or may not explicit
   include path; used to call backend). */
#define BACKEND_CXX_COMPILER_NAME_WITH_PATH "${BACKEND_CXX_COMPILER}"

/* Name of backend C++ compiler excluding path (used to select code generation
   options). */
#define BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH "${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH}"

/* Major version number of backend C++ compiler. */
#define BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER ${BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER}

/* Minor version number of backend C++ compiler. */
#define BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER ${BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER}

/* Name of backend C compiler including path (may or may not explicit include
   path; used to call backend). */
#define BACKEND_C_COMPILER_NAME_WITH_PATH "${BACKEND_C_COMPILER}"

#define BACKEND_C_COMPILER_NAME_WITHOUT_PATH "${BACKEND_C_COMPILER_NAME_WITHOUT_PATH}"
#define BACKEND_C_COMPILER_MAJOR_VERSION_NUMBER ${BACKEND_C_COMPILER_MAJOR_VERSION_NUMBER}
#define BACKEND_C_COMPILER_MINOR_VERSION_NUMBER ${BACKEND_C_COMPILER_MINOR_VERSION_NUMBER}

/* Name of backend Fortran compiler including path (may or may not explicit
   include path; used to call backend). */

#define BACKEND_FORTRAN_COMPILER_NAME_WITH_PATH "${BACKEND_FORTRAN_COMPILER}"
#define BACKEND_FORTRAN_COMPILER_NAME_WITHOUT_PATH "${BACKEND_FORTRAN_COMPILER_NAME_WITHOUT_PATH}"
/* Major version number of backend Fortran compiler. */
#define BACKEND_FORTRAN_COMPILER_MAJOR_VERSION_NUMBER ${BACKEND_FORTRAN_COMPILER_MAJOR_VERSION_NUMBER}

/* Minor version number of backend Fortran compiler. */
#define BACKEND_FORTRAN_COMPILER_MINOR_VERSION_NUMBER ${BACKEND_FORTRAN_COMPILER_MINOR_VERSION_NUMBER}



/* Internal EDG specific consistency tests. */
//AS FIXME
#define CHECKING 1


/* Define if building universal (internal helper macro) */
#undef AC_APPLE_UNIVERSAL_BUILD

/* Internal EDG specific consistency tests. */
//AS FIXME

#undef CHECKING

/* Include path for backend C++ compiler. */
/* TODO We cheat here since in autoconf/automake the list is generated by a script which find all subdirectories under xxx_HEADERS i
 roseGenerateBackendCompilerSpecificHeaders.cmake  should be enhanced to handle this
*/
#define CXX_INCLUDE_STRING {"${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH}_HEADERS", "${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH}_HEADERS/hdrs", "${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH}_HEADERS/hdrs1", "${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH}_HEADERS/hdrs2", "${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH}_HEADERS/hdrs3", "${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH}_HEADERS/hdrs4", "${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH}_HEADERS/hdrs5", "${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH}_HEADERS/hdrs6", "${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH}_HEADERS/hdrs7", "/usr/include"}

/* -D options to hand to EDG C++ front-end. */
#define CXX_SPEC_DEF {"-D__GNUG__=${BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER}", "-D__GNUC__=${BACKEND_C_COMPILER_MAJOR_VERSION_NUMBER}", "-D__GNUC_MINOR__=${BACKEND_C_COMPILER_MINOR_VERSION_NUMBER}", "-D__GNUC_PATCHLEVEL__=${BACKEND_C_COMPILER_PATCH_LEVEL_NUMBER}", "--preinclude", "rose_edg_required_macros_and_functions.h"}

/* Include path for backend C compiler. */
#define C_INCLUDE_STRING {"${BACKEND_C_COMPILER_NAME_WITHOUT_PATH}_HEADERS", "${BACKEND_C_COMPILER_NAME_WITHOUT_PATH}_HEADERS/hdrs", "${BACKEND_C_COMPILER_NAME_WITHOUT_PATH}_HEADERS/hdrs1", "${BACKEND_C_COMPILER_NAME_WITHOUT_PATH}_HEADERS/hdrs2","${BACKEND_C_COMPILER_NAME_WITHOUT_PATH}_HEADERS/hdrs3","${BACKEND_C_COMPILER_NAME_WITHOUT_PATH}_HEADERS/hdrs4", "${BACKEND_C_COMPILER_NAME_WITHOUT_PATH}_HEADERS/hdrs5", "${BACKEND_C_COMPILER_NAME_WITHOUT_PATH}_HEADERS/hdrs6", "${BACKEND_C_COMPILER_NAME_WITHOUT_PATH}_HEADERS/hdrs7","/usr/include"}


/* Controls use of ROSE support for Binary SQL. */
#undef HASH_ROSE_USE_BINARYSQL


/* Define to 1 if you have the `clock_gettime' function. */
#cmakedefine HAVE_CLOCK_GETTIME 1


/* Define to 1 if you have the `dl' library (-ldl). */
#cmakedefine HAVE_LIBDL 1

/* Define if libdlloader will be built on this platform */
//AS FIXME
#undef HAVE_LIBDLLOADER


/* Define to 1 if you have the <machine/endian.h> header file. */
#cmakedefine HAVE_MACHINE_ENDIAN_H 1

/* Define to 1 if you have the `mach_absolute_time' function. */
#cmakedefine HAVE_MACH_ABSOLUTE_TIME 1

/* Define to 1 if you have the <mach/mach_time.h> header file. */
#cmakedefine HAVE_MACH_MACH_TIME_H 1

/* Define to 1 if you have the <mach-o/dyld.h> header file. */
#cmakedefine HAVE_MACH_O_DYLD_H 1

/* Define to 1 if you have the <memory.h> header file. */
#cmakedefine HAVE_MEMORY_H 1 

/* Define to 1 if you have the `mktime' function. */
#cmakedefine HAVE_MKTIME 1

/* Define to 1 if you have a working `mmap' system call. */
#cmakedefine HAVE_MMAP 1

/* Define to 1 if MySQL libraries are available */
#cmakedefine MYSQL_FOUND
#ifdef MYSQL_FOUND
  #define HAVE_MYSQL
#endif


/* Define to 1 if you have the <ndir.h> header file, and it defines `DIR'. */
#cmakedefine HAVE_NDIR_H 1

/* Define to 1 if you have the `opendir' function. */
#cmakedefine HAVE_OPENDIR 1

/* Define if libtool can extract symbol lists from object files. */
//AS irrelevant as we are not using libtool
//#cmakedefine HAVE_PRELOADED_SYMBOLS

/* Define to 1 if you have the `readdir' function. */
#cmakedefine HAVE_READDIR 1

/* Define to 1 if you have the `read_real_time' function. */
#cmakedefine HAVE_READ_REAL_TIME 1

/* Define if you have the shl_load function. */
#cmakedefine HAVE_SHL_LOAD 1

/* Have the SQLITE3 library */
#cmakedefine SQLITE3_FOUND
#ifdef HAVE_SQLITE3
  #define HAVE_SQLITE3
#endif

/* Define to 1 if you have the <stdint.h> header file. */
#cmakedefine HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#cmakedefine HAVE_STDLIB_H 1

/* Define to 1 if you have the `strcoll' function and it is properly defined.
   */
#cmakedefine HAVE_STRCOLL 1

/* Define to 1 if you have the `strcspn' function. */
#cmakedefine HAVE_STRCSPN 1

/* Define to 1 if you have the `strerror' function. */
#cmakedefine HAVE_STRERROR 1

/* Define to 1 if you have the `strftime' function. */
#cmakedefine HAVE_STRFTIME 1

/* Define to 1 if you have the <strings.h> header file. */
#cmakedefine HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#cmakedefine HAVE_STRING_H 1 

/* Define to 1 if you have the `strlcat' function. */
#cmakedefine HAVE_STRLCAT 1 

/* Define to 1 if you have the `strlcpy' function. */
#cmakedefine HAVE_STRLCPY 1 

/* Define to 1 if you have the `strspn' function. */
#cmakedefine HAVE_STRSPN 1

/* Define to 1 if you have the `strstr' function. */
#cmakedefine HAVE_STRSTR 1

/* Define to 1 if you have the `strtod' function. */
#cmakedefine HAVE_STRTOD

/* Define to 1 if you have the `strtol' function. */
#cmakedefine HAVE_STRTOL 1

/* Define to 1 if you have the `strtoul' function. */
#cmakedefine HAVE_STRTOUL 1

/* Define to 1 if you have the <sys/dir.h> header file, and it defines `DIR'.
   */
#cmakedefine HAVE_SYS_DIR_H 1

/* Define to 1 if you have the <sys/dl.h> header file. */
#cmakedefine HAVE_SYS_DL_H 1

/* Define to 1 if you have the <sys/ndir.h> header file, and it defines `DIR'.
   */
#cmakedefine HAVE_SYS_NDIR_H

/* Define to 1 if you have the <sys/stat.h> header file. */
#cmakedefine HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#cmakedefine HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#cmakedefine HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the `time_base_to_time' function. */
#cmakedefine HAVE_TIME_BASE_TO_TIME 1

/* Define to 1 if you have the <unistd.h> header file. */
#cmakedefine HAVE_UNISTD_H 1

/* Define to 1 if you have the `vprintf' function. */
#cmakedefine HAVE_VPRINTF 1

/* This value is set to 1 to indicate that the system argz facility works */
//AS Not quite sure what to do with this
#undef HAVE_WORKING_ARGZ

/* Define if you have the UNICOS _rtc() intrinsic. */
//AS Not quite sure what to do with this
#undef HAVE__RTC

/* Prefix path for use of IDA. */
//AS Not quite sure what to do with this
#undef IDA_PRO_PATH

/* Path to JVM executable */
#define JAVA_JVM_PATH ${JAVA_RUNTIME}

/* Define if the OS needs help to load dependent libraries for dlopen(). */
//AS Don't know what to do with this
#undef LTDL_DLOPEN_DEPLIBS

/* Define to the system default library search path. */
//AS Don't know what to do with this
#undef LT_DLSEARCH_PATH

/* The archive extension */
//AS Don't know what to do with this
#undef LT_LIBEXT

/* Define to the extension used for runtime loadable modules, say, ".so". */
//AS Don't know what to do with this
#undef LT_MODULE_EXT

/* Define to the name of the environment variable that determines the run-time
   module search path. */
//AS Don't know what to do with this
#undef LT_MODULE_PATH_VAR

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
//AS Don't know what to do with this
#undef LT_OBJDIR

/* Define if dlsym() requires a leading underscore in symbol names. */
//AS Don't know what to do with this
#undef NEED_USCORE

/* Name of package */
//AS Don't know what to do with this
#undef PACKAGE

/* Define to the address where bug reports for this package should be sent. */
//AS Don't know what to do with this
#undef PACKAGE_BUGREPORT

/* Define to the full name of this package. */
//AS Don't know what to do with this
#undef PACKAGE_NAME

/* Define to the full name and version of this package. */
//AS Don't know what to do with this
#undef PACKAGE_STRING

/* Define to the one symbol short name of this package. */
//AS Don't know what to do with this
#undef PACKAGE_TARNAME

/* Define to the version of this package. */
//AS Don't know what to do with this
#undef PACKAGE_VERSION

/* Define as the return type of signal handlers (`int' or `void'). */
//AS Don't know what to do with this
#undef RETSIGTYPE

/* Location (unquoted) of Boost specified on configure line. */
#define ROSE_BOOST_PATH ${BOOST_ROOT}

/* Location of ROSE Compile Tree. */
#define ROSE_COMPILE_TREE_PATH ${ROSE_TOP_BINARY_DIR}

/* Variable like LD_LIBRARY_PATH */
//AS Don't know what to do with this
#undef ROSE_SHLIBPATH_VAR

/* Whether to use the new EDG version 4.x */
//AS Don't know what to do with this
#undef ROSE_USE_EDG_VERSION_4

/* Whether to use the new interface to EDG */
//AS Don't know what to do with this
#undef ROSE_USE_NEW_EDG_INTERFACE

/* Use Valgrind calls in ROSE */
//AS Don't know what to do with this
#undef ROSE_USE_VALGRIND

/* Whether to use the new graph IR nodes compatability option with older API
   */
//#undef ROSE_USING_GRAPH_IR_NODES_FOR_BACKWARD_COMPATABILITY

/* Simple preprocessor as default in ROSE */
//AS Need configure option to allow wave to be default
#define ROSE_WAVE_DEFAULT false

/* Location (unquoted) of Wave specified on configure line. */
//AS Not needed
//#undef ROSE_WAVE_PATH

/* Control use of SAGE within EDG. */
//AS Need configure flag to control this variable
#define SAGE_TRANSFORM 1


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
#cmakedefine STDC_HEADERS 1

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#cmakedefine TIME_WITH_SYS_TIME 1

/* Define to 1 if your <sys/time.h> declares `struct tm'. */
#cmakedefine TM_IN_SYS_TIME 1

/* Mark that GFORTRAN is available */
//AS Don't know what to do with this
#undef USE_GFORTRAN_IN_ROSE

/* Link-time support for Insure. */
//AS Don't know what to do with this
#undef USE_INSURE

/* Support for Purify */
//AS Don't know what to do with this
#undef USE_PURIFY

/* Support for Purify API (Compiling with the Purify library). */
//AS Don't know what to do with this
#undef USE_PURIFY_API

/* Link-time support for Purify. */
//AS Don't know what to do with this
#undef USE_PURIFY_LINKER

/* Controls use of ROSE support for bddbddb (Binary decision diagrams (BDD)
   BDD-Based Deductive DataBase. */
//AS Don't know what to do with this
#undef USE_ROSE_BDDBDDB_SUPPORT

/* Controls use of ROSE support for Dwarf (libdwarf) library. */
//AS Don't know what to do with this
//#undef USE_ROSE_DWARF_SUPPORT

/* Controls if EDG Union/Struct debugging support is to be used. */
//AS Fixme: introduce configure control for this
#define USE_ROSE_EDG_DEBUGGING_SUPPORT 1

/* Use of GCC OpenMP in ROSE. */
//AS Don't know what to do with this
#undef USE_ROSE_GCC_OMP

/* Controls use of IDA support for disassembling. */
//AS Don't know what to do with this
#undef USE_ROSE_IDA_SUPPORT

/* Controls use of ROSE support for Intel Pin Dynamic Instrumentation Package.
   */
//AS Don't know what to do with this
#undef USE_ROSE_INTEL_PIN_SUPPORT

/* Controls use of ROSE support for Java. */
#cmakedefine JAVA_RUNTIME
#ifdef JAVA_RUNTIME
  #define USE_ROSE_JAVA_SUPPORT
#endif

/* Controls use of ROSE support for Maple Symbolic Algebra Package. */
//AS Don't know what to do with this
#undef USE_ROSE_MAPLE_SUPPORT

/* Controls use of ROSE support for OpenMP Translator targeting Omni RTL. */
//AS Don't know what to do with this
#undef USE_ROSE_OMNI_OPENMP_SUPPORT

/* Always enable Fortran support whenever Java and gfortran are present */
//AS don't know what to do with this
#undef USE_ROSE_OPEN_FORTRAN_PARSER_SUPPORT

/* Controls use of ROSE support for PHP. */
//AS Don't know what to do with this
#undef USE_ROSE_PHP_SUPPORT

/* Controls use of ROSE support for RTED library. */
//AS Don't know what to do with this
#undef USE_ROSE_RTED_SUPPORT

/* Controls use of ROSE support for SSL (libssl -- MD5) library (incompatable
   with Java, so disables Fortran support). */
//AS Don't know what to do with this
#undef USE_ROSE_SSL_SUPPORT

/* Wine provides access to Windows header files for analysis of windows
   applications under Linux. */
//AS Don't know what to do with this
#undef USE_ROSE_WINDOWS_ANALYSIS_SUPPORT

/* Controls use of ROSE support for Yices Satisfiability Modulo Theories (SMT)
   Solver Package. */
//AS Don't know what to do with this

#undef USE_ROSE_YICES_SUPPORT

/* Support for PURIFY debugging software. */
//AS Don't know what to do with this

#undef USING_PURIFY

/* Version number of package */
//AS Don't know what to do with this

#define VERSION "0.9.4"

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

/* Define to 1 if the X Window System is missing or not being used. */
#undef X_DISPLAY_MISSING

/* Define to 1 if `lex' declares `yytext' as a `char *' by default, not a
   `char[]'. */
#undef YYTEXT_POINTER

/* Define to 1 if type `char' is unsigned and you are not using gcc.  */
#ifndef __CHAR_UNSIGNED__
# undef __CHAR_UNSIGNED__
#endif

/* Define so that glibc/gnulib argp.h does not typedef error_t. */
#undef __error_t_defined

/* Define to empty if `const' does not conform to ANSI C. */
#undef const

/* Define to a type to use for `error_t' if it is not otherwise available. */
#undef error_t

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
#undef inline
#endif

/* Define to `long int' if <sys/types.h> does not define. */
#undef off_t

/* Define to `unsigned int' if <sys/types.h> does not define. */
#undef size_t

/* DQ (10/16/2010): Added OFP required macros as part of move to exclude 
 * rose_config.h from the ROSE header files.
 */
#define ROSE_OFP_MAJOR_VERSION_NUMBER 0
#define ROSE_OFP_MINOR_VERSION_NUMBER 8
#define ROSE_OFP_PATCH_VERSION_NUMBER 2
