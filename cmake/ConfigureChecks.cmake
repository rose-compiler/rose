# This file is part of LyX, the document processor.
# Licence details can be found in the file COPYING.
#
# Copyright (c) 2006, Peter Kümmel, <syntheticpp@gmx.net>
#
# TSG 6/27/24 Refactored to remove many autoconf vars
#             Keep "time" vars for astDiagnostics 

include(CheckIncludeFile)
include(CheckFunctionExists)
include(CheckLibraryExists) # TODO: Use find_package instead 
include(CheckTypeExists)
include(CheckTypeSize)
include(MacroBoolTo01)
include(TestBigEndian)

test_big_endian(WORDS_BIGENDIAN)

if(CMAKE_REQUIRED_QUIET)
  message(STATUS "Checking for common configuration values by inspecting headers...")
endif()

# Check headers in a group 
set(headers_to_check
  argz.h c_asm.h ctype.h dirent.h dlfcn.h dl.h intrinsics.h mach/mach_time.h
  machine/endian.h pthread.h stdlib.h sys/time.h sys/utime.h utime.h 

)

# Define each header that was found for rose_config.h
foreach(header ${headers_to_check})
  string(TOUPPER ${header} HEADER_UPPER)
  string(REPLACE "." "_" HEADER_VAR ${HEADER_UPPER}) 
  # Handle sys/headers format 
  if(header MATCHES "^sys/" OR header MATCHES "mach")
    string(REPLACE "/" "_" HEADER_VAR ${HEADER_VAR})
    check_include_file(${header} HAVE_${HEADER_VAR})
  else()
    check_include_file(${header} HAVE_${HEADER_VAR})
  endif()
endforeach()

# argz functions are part of GNU C
if(HAVE_ARGZ_H) 
  # all of these are defined in argz.h 
  set(HAVE_ARGZ_APPEND 1)
  set(HAVE_ARGZ_CREATE_SEP 1)
  set(HAVE_ARGZ_INSERT 1)
  set(HAVE_ARGZ_NEXT 1) 
  set(HAVE_ARGZ_STRINGIFY 1)
  set(HAVE_WORKING_ARGZ 1)
endif() 

# Check functions in group
set(functions_to_check
   clock_gettime dladdr fcntl gethrtime mach_absolute_time
   read_real_time time_base_to_time
)

# Define each function for rose_config.h
foreach(func ${functions_to_check})
  string(TOUPPER ${func} UPPERCASE_FUNCTION)
  check_function_exists(${func} HAVE_${UPPERCASE_FUNCTION})
endforeach()

# Check types in a group 
CHECK_TYPE_SIZE(char          SIZEOF_CHAR)
check_type_size(double        SIZEOF_DOUBLE)
check_type_size(float         SIZEOF_FLOAT)
CHECK_TYPE_SIZE("long double" SIZEOF_LONG_DOUBLE)
check_type_size("long long"   SIZEOF_LONG_LONG)
check_type_size("void*"       SIZEOF_VOID_P)
CHECK_TYPE_SIZE(short         SIZEOF_SHORT)
CHECK_TYPE_SIZE(int           SIZEOF_INT)
CHECK_TYPE_SIZE(long          SIZEOF_LONG)
check_type_size(intmax_t      HAVE_INTMAX_T)
check_type_size("long double" HAVE_LONG_DOUBLE)
check_type_size("long long"   HAVE_LONG_LONG)
check_type_size(wchar_t       HAVE_WCHAR_T)
check_type_size(wint_t        HAVE_WINT_T)

if(NOT CMAKE_REQUIRED_QUIET)
  message(STATUS "Checking whether system has ANSI C header files")
endif()

# More manual explicit checks
if(HAVE_SYS_TIME_H)  
  check_include_file("time.h" TIME_WITH_SYS_TIME)
  check_type_exists("struct tm" sys/time.h TM_IN_SYS_TIME)
  check_type_exists("hrtime_t" sys/time.h HAVE_HRTIME_T)
endif()

check_library_exists(dl dlopen "/lib64;/usr/lib;/lib;/usr/local/lib;/usr/pkg/lib" HAVE_LIBDL)
