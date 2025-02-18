########################################################################################################################
# General warnings, instructions, and style guide.
########################################################################################################################

# 1. Please be careful about where to put your tests and variable settings. The order matters!!
#
# 2. CMake configuration files are neither completely case-sensitive nor completely case-insensitive. Therefore, to
#    avoid errors, assume everything is case sensitive. Prefer lower case for function names.
#
# 3. Do not use argument turds in "else()" and "endif()" functions. Those arguments are only comments and are entirely
#    redundant with the "if()" argument and easily become out of date and misleading since nobody actually reads them.
#
# 4. All messages should start with a capitalized word except in special circumstances where capitalization would be
#    incorrect (such as the name of a command).
#
# 5. Indentation is two spaces. Do not use ASCII horizontal tab characters for indentation or alignment.
#
# 6. As with standard mathematical notation, there should be no white space on either side of a function's
#    parentheses. This includes functions such as "if", "else", and "endif".
#
# 7. Prefer FALSE and TRUE as Boolean values since these are the names used in Mathematics and most other computer
#    languages. Avoid OFF, ON, NO, YES, 0, 1, and especially avoid mixing them.
#
# 8. If a CMakefile file needs to be conditionally enabled, do it in that CMake file rather than around the
#    add_subdirectory in the level above. This keeps all the logic for a directory in a single file rather than
#    split across two files. It is a bit unfortunate that CMake can't find the lower-level CMakeList files
#    on its own, so some of the logic is still necessarily in the level above.  There are exceptions to this rule,
#    and they're pretty obvious when they occur--as when a single if() protects a whole bunch of add_subdirectory.

########################################################################################################################
# Platform-independent settings
########################################################################################################################
cmake_minimum_required(VERSION 3.15)

#-----------------------------------------------------------------------------------------------------------------------
# CCACHE
# Enable ccache if not already enabled by symlink masquerading and if no other compiler launchers are already defined
#-----------------------------------------------------------------------------------------------------------------------
find_program(CCACHE_EXECUTABLE ccache)
mark_as_advanced(CCACHE_EXECUTABLE)
if(CCACHE_EXECUTABLE)
  foreach(LANG C CXX)
    if(NOT DEFINED CMAKE_${LANG}_COMPILER_LAUNCHER AND NOT CMAKE_${LANG}_COMPILER MATCHES ".*/ccache")
      message(STATUS "Enabling ccache for ${LANG}")
      set(CMAKE_${LANG}_COMPILER_LAUNCHER ${CCACHE_EXECUTABLE} CACHE STRING "")
    endif()
  endforeach()
endif()

# Option for custom compilers 
option(SKIP-ABI "Option to use custom CC/CXX" OFF) 
if(SKIP-ABI) # turn off checks and abi 
  set(CMAKE_C_ABI_COMPILED FALSE)
  set(CMAKE_C_COMPILER_WORKS ON CACHE INTERNAL "")
  set(CMAKE_C_STANDARD_LIBRARIES_INIT "")
  set(CMAKE_CXX_ABI_COMPILED FALSE) 
  set(CMAKE_CXX_COMPILER_WORKS ON CACHE INTERNAL "")
  set(CMAKE_CXX_STANDARD_LIBRARIES_INIT "")
endif() 

project(ROSE C CXX)

#-----------------------------------------------------------------------------------------------------------------------
# BLT 
#-----------------------------------------------------------------------------------------------------------------------
 # Use internal BLT if no BLT_SOURCE_DIR is given
if(NOT DEFINED BLT_SOURCE_DIR)
  set(BLT_SOURCE_DIR "${PROJECT_SOURCE_DIR}/cmake/blt" CACHE PATH "Path to BLT")
endif()

# Support having a shared BLT outside of the repository if given a BLT_SOURCE_DIR
if(NOT EXISTS ${BLT_SOURCE_DIR}/SetupBLT.cmake)
  message(FATAL_ERROR "Given BLT_SOURCE_DIR does not contain SetupBLT.cmake or"
                      "the BLT git submodule is not present. "
                      "Either run the following two commands in your git repository: \n"
                      "    git submodule init\n"
                      "    git submodule update\n"
                      "Or add -DBLT_SOURCE_DIR=/path/to/blt to your CMake command." )
endif()

# If the user sets CMAKE_CXX_STANDARD, use that for BLT_CXX_STD as well
if(CMAKE_CXX_STANDARD)
  set(BLT_CXX_STD "c++${CMAKE_CXX_STANDARD}" CACHE STRING "")
elseif(NOT BLT_CXX_STD)
  # Default to C++14 if not set so GTest/GMock can build
  set(BLT_CXX_STD "c++14" CACHE STRING "")
  set(CMAKE_CXX_STANDARD 14)
endif()
  set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${BLT_SOURCE_DIR}/SetupBLT.cmake)

#-----------------------------------------------------------------------------------------------------------------------
# CMAKE POLICIES  
#-----------------------------------------------------------------------------------------------------------------------
# CMake 2.8.12 and newer has support for using @rpath in a target install name. This was enabled by setting the target
# property MACOSX_RPATH. The @rpath in an install name is a more flexible and powerful mechanism than @executable_path
# or @loader_path for locating shared libraries.
#
# CMake 3.0 and later prefer CMP0042 to be ON by default. Projects wanting @rpath in a target's install name may remove
# any setting of the INSTALL_NAME_DIR and CMAKE_INSTALL_NAME_DIR variables.
#
# CMP0042 was introduced in CMake version 3.0. CMake version 3.0.2 warns when the policy is not set and uses OLD
# behavior.
if(POLICY_CMP0042)
  cmake_policy(SET CMP0042 NEW)
endif()

# CMP0054 causes CMake to only interpret "if()" arguments as variables or keywords when unquoted. CMake 3.1 and above no
# longer implicitly dereference variables or interpret keywords in an if() command argument when it is a Quoted Argument
# or a Bracket Argument. The OLD behavior for this policy is to dereference variables and interpret keywords even if
# they are quoted or bracketed. The NEW behavior is to not dereference variables or interpret keywords that have been
# quoted or bracketed.
if(POLICY CMP0054)
  cmake_policy(SET CMP0054 NEW)
endif()

# In CMake 3.12 and above the find_package(<PackageName>) command now searches prefixes specified by the
# <PackageName>_ROOT CMake variable and the <PackageName>_ROOT environment variable. Package roots are maintained as a
# stack so nested calls to all find_* commands inside find modules also search the roots as prefixes. This policy
# provides compatibility with projects that have not been updated to avoid using <PackageName>_ROOT variables for other
# purposes.  The OLD behavior for this policy is to ignore <PackageName>_ROOT variables. The NEW behavior for this
# policy is to use <PackageName>_ROOT variables.
#
# Behavior is set to "NEW" because ROSE matrix testing, Livermore's LC RZ/CZ resources, Spack, and RMC/Spock seldom
# install ROSE software dependencies in standard locations because they need to support the ability to install multiple
# versions and configurations of the dependencies.
if(POLICY_CMP0074)
  cmake_policy(SET CMP0074 NEW)
endif()

# This controls whether the "cmake" command is verbose. It has nothing to do with the verbosity the resulting Makefiles.
option(VERBOSE "CMake should be verbose" FALSE)
if(VERBOSE)
  set(QUIET FALSE)
  set(CMAKE_VERBOSE_MAKEFILE TRUE)
else()
  set(QUIET TRUE)
  set(CMAKE_VERBOSE_MAKEFILE FALSE)
endif()

option(BUILD_SHARED_LIBS "Build all libraries shared" FALSE)

option(use-lib64-paths "Should ROSE be installed in lib64 paths? (Some Linux distributes expect this)" OFF)
if(use-lib64-paths)
  set(ROSE_LIB_DIR_NAME "lib64")
else()
  set(ROSE_LIB_DIR_NAME "lib")
endif()

if(WIN32)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  add_definitions(-DBOOST_ALL_NO_LIB=1)
endif()

# FIXME: Why do we have to have a copy of some standard built-in modules inside rose?
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" "${CMAKE_SOURCE_DIR}/cmake/modules"  ${CMAKE_MODULE_PATH})

# ROSE source and build (binary) directory hierarchies
set(ROSE_TOP_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(ROSE_TOP_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

########################################################################################################################
# ROSE version information
########################################################################################################################
# All distributions of ROSE also have a ROSE_VERSION file with a dotted version that's updated during the ROSE release process.
set(ROSE_SCM_VERSION_FILE "${PROJECT_SOURCE_DIR}/ROSE_VERSION")
if(NOT EXISTS "${ROSE_SCM_VERSION_FILE}")
  message(FATAL_ERROR "ROSE version file not found: ${ROSE_SCM_VERSION_FILE}")
endif()

# All distributions of ROSE also have a config/SCM_DATE file with a numerical version that's updated during the ROSE release process.
set(ROSE_SCM_DATE_FILE "${PROJECT_SOURCE_DIR}/config/SCM_DATE")
if(NOT EXISTS "${ROSE_SCM_DATE_FILE}")
  message(FATAL_ERROR "ROSE SCM Date file not found: ${ROSE_SCM_DATE_FILE}")
endif()

file(STRINGS ${ROSE_SCM_VERSION_FILE} ROSE_PACKAGE_VERSION LIMIT_COUNT 1)
file(STRINGS ${ROSE_SCM_VERSION_FILE} ROSE_SCM_VERSION_ID LIMIT_COUNT 1)
file(READ ${ROSE_SCM_DATE_FILE} ROSE_VERSION)

# Fix trailing whitespace 
string(STRIP "${ROSE_VERSION}" ROSE_VERSION) 

# Print Results
message(STATUS "The ROSE version integer is ${ROSE_VERSION}")
message(STATUS "The ROSE version string is ${ROSE_PACKAGE_VERSION}")

########################################################################################################################
# Operating system information
########################################################################################################################

# Test whether the host OS (the OS compiling ROSE) is Red Hat Enterprise Linux, and if so, set ROSE_HOST_OS_IS_RHEL to
#the RHEL major version number.
set(ROSE_HOST_OS_IS_RHEL "")
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
  execute_process(
    COMMAND bash -c ". /etc/os-release && echo -n $ID"
    OUTPUT_VARIABLE ROSE_HOST_OS_ID)
  if ("${ROSE_HOST_OS_ID}" MATCHES "rhel")
    execute_process(
      COMMAND bash -c ". /etc/os-release && echo -n \${VERSION_ID%%.*}"
      OUTPUT_VARIABLE ROSE_HOST_OS_VERSION_MAJOR)
    set(ROSE_HOST_OS_IS_RHEL "${ROSE_HOST_OS_VERSION_MAJOR}")
  endif()
endif()

########################################################################################################################
# Initialize the leading part of the Rose::initialize token
########################################################################################################################
# Configuration synopsis used by Rose::initialize.  The ROSE_CONFIG_TOKEN is #define'd as a string in rose_config.h and
# ultimately rosePublicConfig.h and the string is a synopsis of some important configuration details. This string is
# passed by user code to the Rose::initialize function which compares it against the same macro compiled into the ROSE
# library. If their contents differ it means that the ROSE header files being used by the user are not the same as the
# ROSE header files that were used to compile librose being linked by the user and bad things will probably happen.
#
# The CMake ROSE_CONFIG_TOKEN variable is the string that becomes the eventual value of the ROSE_CONFIG_TOKEN C
# preprocessor macro.  We initialize it here, but other parts of the CMake file might augment its value with additional
# information (e.g., adding the boost version number).
set(ROSE_CONFIG_TOKEN "rose-${ROSE_SCM_VERSION_ID}")

########################################################################################################################
# Boost libraries
########################################################################################################################
if(CMAKE_BUILD_TYPE STREQUAL "Debug") 
  message(STATUS
    "If you get a whole bunch of warnings saying 'New Boost version may have incorrect or missing dependencies and "
    "imported targets' it is probably because you're using a Boost version that was released after your CMake version. "
    "See [https://github.com/Kitware/CMake/commits/master/Modules/FindBoost.cmake] to see latest supported version and "
    "[https://github.com/Microsoft/vcpkg/issues/2459] for a description of the problem. Note that CMake versions "
    "3.11.0 through 3.13.2 and possibly later) cannot be compiled (syntax errors) with a GNU C++ compiler that's "
    "configured to use a non-default language standard (e.g., C++11 with GCC-5.4 whose default is GNU++03).")
endif() 

set(Boost_USE_STATIC_LIBS FALSE)
set(Boost_DEBUG ${VERBOSE})
option(BOOST_USE_MULTITHREADED "Should Boost multithreaded libraries be used?" OFF) # puts -pthread on link and compile

# Honor BOOST_HOME environment variable
if(DEFINED ENV{BOOST_HOME})
  set(BOOST_ROOT "$ENV{BOOST_HOME}")
endif()

# Boost 1.47 is no longer supported by ROSE, but is what's installed on Jenkins' CMake test machine. This should
#        be changed to the actual minimum supported version once Pei-Hung upgrades the machine. [Matzke 2019-01-21]
#
# First look for the Boost libraries that are required in order to compile ROSE, then additionally look for any optional
# libraries that are useful to ROSE but not required. From a user: "The find_package() change is required for boost
# 1.70 or above on any system when using a recent enough version of cmake.  Without it, serialization support will not
# be used.  Before boost 1.70, the findBoost() in cmake is used directly, and it will look for boost components that are
# not in the find_package clause.  Since 1.70, cmake will load the findBoost() provided by the boost package itself.
# Unfortunately, that findBoost() will only check for components in the find_package() clause.  This means that
# serialization is considered not to exist, even if it is there.  My change calls find_package again without the
# REQUIRED clause after the first one has succeeded.  That way the first clause checks for requires components, and the
# second can check again including optional components.  If the version of cmake is high enough you can use an
# OPTIONAL_COMPONENTS clause instead, but that wasn't introduced until 3.11."
find_package(Boost REQUIRED COMPONENTS chrono date_time filesystem iostreams program_options random regex system wave thread serialization)
if(WIN32) 
  set(BOOST_LIBRARYDIR ${Boost_LIBRARY_DIRS})
  set(BOOST_INCLUDEDIR ${Boost_INCLUDE_DIRS}/)
endif() 

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

# Defines for rose_config.h  
set(ROSE_CONFIG_TOKEN "${ROSE_CONFIG_TOKEN} boost-${Boost_VERSION}")

# Boost version-specific macros
if(Boost_VERSION VERSION_GREATER_EQUAL "1.78.0") 
  # Resolves compiler warning about Boost placeholders (_1, _2, etc.) in the global namespace, which is deprecate
  # Boost now recommends including the <boost/bind/bind.hpp> header and using the boost::placeholders namespace instead
  # To resolve this warning, we can use the line of CMake below or Update the code to use the recommended namespace
  # To update the code,  Modify Sawyer codes to include <boost/bind/bind.hpp> and use boost::placeholders
  add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS) 
endif()

# Boost compiler-specific flags
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang") 
  # Do not supress boost warnings in debug mode  
  if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug") 
    # Compiler specific commands, not to be used with MSVC 
    add_compile_options(-Wno-switch-default) # supress warnings coming from the boost library code itself 
    add_compile_options(-Wno-deprecated)     # supress warnings coming from the boost library code itself 
  endif() 
endif() 

# If the boost is compiled with multi-thread support then we must add "-pthread" to all compile and link commands, or
# whatever is appropriate for the system.  With GCC it is not sufficient to just add "-lpthread" to the link
# commands--the "-pthread" switch MUST be added to the compile commands as well.
# FIXME: Do we want to use our option(BOOST_USE_MULTITHREADED) or always do this? 
if(Boost_THREAD_FOUND)
  set(Threads_FIND_QUIETLY ${QUIET})
  find_package(Threads)
  # FIXME: This breaks things
  # if(Threads_FOUND)
  #   if(BOOST_USE_MULTITHREADED) # Something inside this conditional gets OpenCL   
  #     message(WARNING "Setting _REENTRANT") 
  #     set(_REENTRANT 1) 
  #  endif() 
  add_definitions(-pthread) # it seems we always want this  
  set(ROSE_CONFIG_TOKEN "${ROSE_CONFIG_TOKEN} pthread")  
endif()

# Boost misconfiguration is often the cause of many issues
message(STATUS "Boost_VERSION: ${Boost_VERSION}" )           # Report Boost version in all modes  
message(STATUS "Boost_INCLUDE_DIRS:  ${Boost_INCLUDE_DIRS}") # Report Boost -I in all modes  
message(STATUS "Boost_LIBRARY_DIRS:  ${Boost_LIBRARY_DIRS}") # Report Boost -L in all modes  

# Paths to install header, executable, and libraries
# TODO: Prioritize using built-in CMake Variables instead if possible 
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT) # built-in variable in version 3.7.1
  message(WARNING "Using default installation path: ${CMAKE_INSTALL_PREFIX} \
                   \nRun the command: cmake .. -DCMAKE_INSTALL_PREFIX=</custom/path> to control where ROSE is installed")
endif() 

# Directories where Rose executable, rose library, and rose headers will be installed 
set(INCLUDE_INSTALL_DIR "include/rose")
set(BIN_INSTALL_DIR     "bin")
set(LIB_INSTALL_DIR     "${ROSE_LIB_DIR_NAME}")

set(INSTALL_TARGETS_DEFAULT_ARGS
    RUNTIME DESTINATION "${BIN_INSTALL_DIR}"
    LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
    ARCHIVE DESTINATION "${LIB_INSTALL_DIR}"
    COMPONENT Devel)

# A new definition to tweak code for cmake
set(USE_CMAKE ON CACHE BOOL "Define if using CMake")

# ROSE configuration variables for Boost
set(HAVE_BOOST ${Boost_FOUND})
set(HAVE_BOOST_SERIALIZATION_LIB ${Boost_SERIALIZATION_FOUND})
set(HAVE_BOOST_DATE_TIME ${Boost_DATE_TIME_FOUND})
set(HAVE_BOOST_FILESYSTEM ${Boost_FILESYSTEM_FOUND})
set(HAVE_BOOST_PROGRAM_OPTIONS ${Boost_PROGRAM_OPTIONS_FOUND})
set(HAVE_BOOST_REGEX ${Boost_REGEX_FOUND})
set(HAVE_BOOST_SYSTEM ${Boost_SYSTEM_FOUND})
set(HAVE_BOOST_THREAD ${Boost_THREAD_FOUND})
set(HAVE_BOOST_WAVE ${Boost_WAVE_FOUND})
set(USE_ROSE_BOOST_WAVE_SUPPORT ${Boost_WAVE_FOUND})

########################################################################################################################
# Compiler toolchain features
########################################################################################################################
# Define and initialize cache variables for optimization flags
set(WITH_C_OPTIMIZE "-O2" CACHE STRING "Custom C optimization flags")
set(WITH_CXX_OPTIMIZE "-O2" CACHE STRING "Custom C++ optimization flags")
set(WITH_OPTIMIZE "-O2" CACHE STRING "Global optimization flags")

# If CMAKE_BUILD_TYPE is empty, default to -O0 for WITH_OPTIMIZE
if(NOT DEFINED CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
  set(WITH_OPTIMIZE "-O0" CACHE STRING "Global optimization flags" FORCE)
endif()

# If CMAKE_BUILD_TYPE is Release, default to -O3 for WITH_OPTIMIZE 
if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(WITH_OPTIMIZE "-O3" CACHE STRING "Global optimization flags" FORCE)
endif()

if(WIN32)
  set(CMAKE_BUILD_TYPE Release)
  # Compiler flags
  # /TP to indicate files are C++
  # / CLR common intermediate language
  # /GL whole program optimization
  # /O1 optimization for small files
  # /Ob0 disable inline expansion
  # /MP multiple processors compilation
  # /0s small files
  # /wd4716 to turn no return to a warning and not an error
  # /w -- suppresses all compiler warnings
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /TP /MP /O1 /Os /GR /EHsc /wd4541 /wd4716 /bigobj /w")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}   /wd4541 ")
  # Linker flags
  # /IGNORE:4217 - suppress "symbol 'A' defined in 'B' is imported by 'C' in function 'D'"
  # /IGNORE:4286 - suppress "symbol 'A' defined in 'B' is imported by 'C'
  set(WINDOWS_EXTRA_LINKER_FLAGS "/INCREMENTAL:NO /IGNORE:4217,4286")
  set(CMAKE_SHARED_LINKER_FLAGS_DEBUG " ${CMAKE_SHARED_LINKER_FLAGS_DEBUG} ${WINDOWS_EXTRA_LINKER_FLAGS}")
  set(CMAKE_SHARED_LINKER_FLAGS_RELEASE " ${CMAKE_SHARED_LINKER_FLAGS_RELEASE} ${WINDOWS_EXTRA_LINKER_FLAGS}")
  set(CMAKE_MODULE_LINKER_FLAGS_DEBUG " ${CMAKE_MODULE_LINKER_FLAGS_DEBUG} ${WINDOWS_EXTRA_LINKER_FLAGS}")
  set(CMAKE_MODULE_LINKER_FLAGS_RELEASE " ${CMAKE_MODULE_LINKER_FLAGS_RELEASE} ${WINDOWS_EXTRA_LINKER_FLAGS}")
  set(CMAKE_LINKER_FLAGS " ${CMAKE_LINKER_FLAGS} ${WINDOWS_EXTRA_LINKER_FLAGS}")
  set(CMAKE_SHARED_LINKER_FLAGS " ${CMAKE_SHARED_LINKER_FLAGS} ${WINDOWS_EXTRA_LINKER_FLAGS}")
  set(CMAKE_MODULE_LINKER_FLAGS " ${CMAKE_MODULE_LINKER_FLAGS} ${WINDOWS_EXTRA_LINKER_FLAGS}")
  set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} ${WINDOWS_EXTRA_LINKER_FLAGS}")
  set(CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} ${WINDOWS_EXTRA_LINKER_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${WINDOWS_EXTRA_LINKER_FLAGS}")
endif()
if(NOT WIN32)
  #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -DHAVE_CONFIG_H -fPIC ")
  #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fexceptions -DHAVE_CONFIG_H -fPIC ")
  
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-w) 
  endif() 

  # Extract and set C optimization flags
  if(CMAKE_C_COMPILER_ID STREQUAL "KCC")
    set(C_OPTIMIZE " -g +K0 --no_exceptions --no_rtti --keep_gen_c ")
  else()
    set(C_OPTIMIZE "${WITH_C_OPTIMIZE}")
  endif()

  # Extract and set CXX optimization flags
  if(CMAKE_CXX_COMPILER_ID STREQUAL "KCC")
    set(CXX_OPTIMIZE " -g +K0 --no_exceptions --no_rtti --keep_gen_c ")
  else()
    set(CXX_OPTIMIZE "${WITH_CXX_OPTIMIZE}")
  endif()

  # Set global optimization flags, if specified
  if(WITH_OPTIMIZE)
    set(C_OPTIMIZE "${WITH_OPTIMIZE}")
    set(CXX_OPTIMIZE "${WITH_OPTIMIZE}")
  endif()

  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_OPTIMIZE} ")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_OPTIMIZE} ")

  # Ensure C++11 ABI compatibility
  if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=1")
  endif()

  if(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=1")
  endif()
endif()

# Set ROSE_SHLIBPATH_VAR.  For Visual Studio, this is PATH.  Otherwise, it is just LD_LIBRARY_PATH.
if(MSVC)
  set(ROSE_SHLIBPATH_VAR "PATH")
  add_definitions("-D__STDC_LIMIT_MACROS")
else()
  set(ROSE_SHLIBPATH_VAR "LD_LIBRARY_PATH")
endif()

########################################################################################################################
# Analyzable languages supported by ROSE
########################################################################################################################
# Binary analysis
option(ENABLE-BINARY-ANALYSIS "Enable binary analysis" OFF)
if(ENABLE-BINARY-ANALYSIS)
  set(ROSE_BUILD_BINARY_ANALYSIS_SUPPORT 1)
endif()

# C/C++ (default)
option(ENABLE-C "Enable C/C++ analysis" ON)
if(ENABLE-C)
  set(ROSE_BUILD_CXX_LANGUAGE_SUPPORT 1)
  set(ROSE_BUILD_C_LANGUAGE_SUPPORT 1)
endif()

# CUDA
option(ENABLE-CUDA "Enable CUDA analysis" OFF) # OFF because lack of CUDA is not handled properly
if(ENABLE-CUDA)
  if(APPLE)
    message(FATAL_ERROR "CUDA analysis (ENABLE-CUDA) is not supported on macOS")
  endif()
  find_package(CUDA REQUIRED)
  set(ROSE_BUILD_CUDA_LANGUAGE_SUPPORT 1)
endif()

# Java
option(ENABLE-JAVA "Enable Java analysis" OFF)
if(ENABLE-JAVA)
  message(STATUS "Looking for JAVA ...")
  if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR VERBOSE)
    message(STATUS "JAVA_ROOT is: ${JAVA_ROOT}") 
  endif() 
  include(FindJava)
  find_program(GCJ gcj)
  find_program(GCJH gcjh)
  find_package(JNI)
  set(ROSE_BUILD_JAVA_LANGUAGE_SUPPORT 1)
  set(USE_ROSE_INTERNAL_JAVA_SUPPORT 1)
  get_filename_component(BACKEND_JAVA_COMPILER ${Java_JAVAC_EXECUTABLE} NAME)
  install(FILES ${JAVA_JVM_LIBRARY} DESTINATION ${ROSE_LIB_DIR_NAME})
endif()

# OpenCL
option(ENABLE-OPENCL "Enable OpenCL analysis" OFF)
if(ENABLE-OPENCL)
  find_package(OpenCL)
  set(ROSE_BUILD_OPENCL_LANGUAGE_SUPPORT 1)
  find_path(with-opencl-inc NAMES cl.h DOC "For OpenCL runtime library")
  find_library(with-opencl-lib OpenCL DOC "OpenCL library for runtime examples")
endif()

# Fortran
option(ENABLE-FORTRAN "Enable Fortran analysis." OFF) # OFF because lack of OFP is not handled properly
if(ENABLE-FORTRAN)
  if(NOT ENABLE-JAVA)
    message(FATAL_ERROR "Fortran analysis also requires Java analysis.  Either
    turn on ENABLE-JAVA, or turn off ENABLE-FORTRAN")
  endif()
  set(ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT 1)
  enable_language(Fortran)

  # check if gfortran (gnu) was found
  if(CMAKE_COMPILER_IS_GNUG77 OR CMAKE_Fortran_COMPILER MATCHES "gfortran")
    set(USE_GFORTRAN_IN_ROSE 1)
    set(BACKEND_FORTRAN_IS_GNU_COMPILER 1)
  endif() 
  
  # query fortran compiler version
  execute_process(COMMAND ${CMAKE_Fortran_COMPILER} --version
                  OUTPUT_VARIABLE Fortran_COMPILER_VERSION_OUTPUT
                  OUTPUT_STRIP_TRAILING_WHITESPACE) 
  string(REGEX MATCH "([0-9]+\\.[0-9]+\\.[0-9]+)"
         Fortran_COMPILER_VERSION "${Fortran_COMPILER_VERSION_OUTPUT}")
  if(Fortran_COMPILER_VERSION)
    message(STATUS "Fortran compiler version detected:  ${Fortran_COMPILER_VERSION}")
  else() 
    message(WARNING "Failed to detect Fortran compiler version")
  endif()
  
  # This is a cache string var assuming we sometimes may expect user to set  
  set(enable-ofp-version "20200819-JDK8" CACHE STRING "version number for OFP")
  
  # Set rose_config.h macros
  set(ROSE_OFP_VERSION_NUMBER "${enable-ofp-version}")
 
  # This is trying to set an environment variable -- is this what we want to do?  
  set($ENV{CLASSPATH} ${CMAKE_SOURCE_DIR}/src/3rdPartyLibraries/antlr-jars/antlr-3.5.2-complete.jar::${CMAKE_SOURCE_DIR}/rc/3rdPartyLibraries/fortran-parser/OpenFortranParser-${enable-ofp-version}.jar)
endif()

# PHP
option(ENABLE-PHP "Enable PHP analysis" OFF)
if(ENABLE-PHP)
  set(ROSE_BUILD_PHP_LANGUAGE_SUPPORT 1)
  find_path(with-php php DOC "Specify the prefix where PHP (and phc) is installed")
endif()

# Python
option(ENABLE-PYTHON "Enable Python analysis" OFF)
if(ENABLE-PYTHON)
  set(ROSE_BUILD_PYTHON_LANGUAGE_SUPPORT 1)
  find_package(PythonLibs REQUIRED)
  option(WITH-PYTHON "Build code that requires a Python interpreter" ON)
  if(WITH-PYTHON)
    find_package(PythonInterp)
  endif()
endif()

# Ada
option(ENABLE-ADA "Enable Ada analysis" OFF)
if(ENABLE-ADA)
  message(STATUS "Checking the environment variables GNAT_HOME and BOOST_HOME have been set.") 
  
  # Force user to define GNAT_HOME 
  if(NOT DEFINED ENV{GNAT_HOME}) 
    message(FATAL_ERROR "Ada support requires the environment variable GNAT_HOME to be set to an installed GNAT") 
  endif() 

  # Force user to define GNAT_HOME as a directory that exists on the system or within the container 
  if(NOT IS_DIRECTORY "$ENV{GNAT_HOME}") 
    message(WARNING "GNAT_HOME: $ENV{GNAT_HOME}")
    message(FATAL_ERROR "GNAT_HOME does not exist on this system") 
  endif() 

  # Force user to define BOOST_HOME
  if(NOT DEFINED ENV{BOOST_HOME}) 
    message(FATAL_ERROR "Ada support requires the environment variable BOOST_HOME to be set to an installed boost")
  endif() 

  # Force user to define BOOST_HOME as a directory that exists on the system or within the container 
  if(NOT IS_DIRECTORY "$ENV{BOOST_HOME}") 
    message(WARNING "BOOST_HOME: $ENV{BOOST_HOME}")
    message(FATAL_ERROR "BOOST_HOME does not exist on this system") 
  endif()    

  # Check PATH and LD_LIBRARY_PATH
  get_filename_component(USER_PATH $ENV{PATH} ABSOLUTE)               # retrieve USER_PATH  
  get_filename_component(USER_LD_PATH $ENV{LD_LIBRARY_PATH} ABSOLUTE) # retrieve USER_LD_PATH
  
  # Ensure user has GNAT_HOME/bin in their path 
  string(FIND "${USER_PATH}" "$ENV{GNAT_HOME}/bin" found_gnat_in_path) 
  if(found_gnat_in_path EQUAL -1) 
    message(WARNING "GNAT_HOME/bin was not found in users path: ${USER_PATH}") 
  else() 
    message(STATUS "Found $ENV{GNAT_HOME}/bin")
  endif()

  # Ensure user has GNAT_HOME/lib in their LD path 
  string(FIND "${USER_LD_PATH}" "$ENV{GNAT_HOME}/lib" found_gnat_in_ld_path) 
     
  if(found_gnat_in_ld_path EQUAL -1) 
    message(WARNING "{GNAT_HOME}/lib was not found in users LD_LIBRARY_PATH: ${USER_LD_PATH}") 
  else() 
    message(STATUS "Found $ENV{GNAT_HOME}/lib")
  endif()

  # Ensure user has GNAT_HOME/lib in their LD path 
  string(FIND "${USER_LD_PATH}" "${BOOST_HOME}/lib" found_boost_in_ld_path) 
  if(found_boost_in_ld_path EQUAL -1) 
    message(WARNING "BOOST_HOME not found in users LD_LIBRARY_PATH: ${USER_LD_PATH}") 
  else() 
    message(STATUS "Found $ENV{BOOST_HOME}/lib")
  endif()

  # Define macros for source code 
  set(ROSE_BUILD_ADA_LANGUAGE_SUPPORT 1)
  # Autoconf sets this macro below as "no" when enable-ada-frontend=yes 
  set(ROSE_EXPERIMENTAL_ADA_ROSE_CONNECTION_GNAT_HOME "$ENV{GNAT_HOME}") # if this causes errors, set to Autoconf default
  message(WARNING "Ada Analysis may work and pass compile tests in this setup but one macro is still needed to be defined"
                  "ROSE_EXPERIMENTAL_ADA_ROSE_CONNECTION currently causes compilation errors")
  # TODO: Define this 
  # set(ROSE_EXPERIMENTAL_ADA_ROSE_CONNECTION 1) 
endif()

# Jovial
option(ENABLE-JOVIAL "Enable Jovial analysis" OFF)
if(ENABLE-JOVIAL)
  set(ROSE_BUILD_JOVIAL_LANGUAGE_SUPPORT 1)
endif()

# The C preprocessor is enabled automatically if C, C++, or Fortran is enabled
if(ENABLE-C OR ENABLE-FORTRAN)
  set(ENABLE-CPP ON)
  set(ROSE_BUILD_CPP_LANGUAGE_SUPPORT 1)
endif()

########################################################################################################################
# EDG (Edison Design Group) frontend C/C++ compiler
########################################################################################################################
# Default EDG version
set(EDG_VERSION "6.5" CACHE STRING "major.minor version number for EDG (e.g. 5.0).")

# Check whether we have the EDG source code. Even if we have it, we might pretend (for testing) that we don't.
option(EDG_COMPILE "Compile EDG source code if available" TRUE)
if(EXISTS "${PROJECT_SOURCE_DIR}/src/frontend/CxxFrontend/EDG/CMakeLists.txt")
  if(EDG_COMPILE)
    set(have_EDG_source TRUE)
  else()
    set(have_EDG_source FALSE)
  endif()
else()
  set(have_EDG_source FALSE)
  set(EDG_COMPILE FALSE)
endif()

# Check if we should download the EDG binary tarball. We only need to download it if we're not compiling the
# EDG source code and ROSE is configured to analyze C (and C++).
if(NOT have_EDG_source)
  set(BINARY_EDG 0)
  if(ENABLE-C)
    set(BINARY_EDG 1)
    if(NOT WIN32)
      message(STATUS "EDG - downloading EDG-${EDG_VERSION} binary tar file")
      if(EXISTS "${PROJECT_BINARY_DIR}/src/frontend/CxxFrontend/EDG.tar.gz")
        execute_process(COMMAND "tar" "-zxvf" "-C" "${PROJECT_BINARY_DIR}/src/frontend/CxxFrontend/EDG")
      else()
      # no need to display this message after the file has been downloaded
      if(NOT EXISTS "${PROJECT_BINARY_DIR}/src/frontend/CxxFrontend/EDG/.libs")
        message(WARNING "At build time, CMake will attempt to download a required library tarball.\n"
                        "Please note that EDG binary tarballs are available for only certain configurations.")
      endif()
      include(${PROJECT_SOURCE_DIR}/cmake/DownloadEDG.cmake)
      endif()
    endif()
  endif()
endif()

set(edg_lib EDG) # the compiled library, the downloaded library, or the dummy library
if(have_EDG_source)
  message(STATUS "EDG - will compile EDG-${EDG_VERSION} source code")
elseif(BINARY_EDG)
  message(STATUS "EDG - will use EDG-${EDG_VERSION} binary release (download)")
else()
  message(STATUS "EDG - not needed; using a nearly empty dummy library")
endif()

########################################################################################################################
# System features
########################################################################################################################
# A set of common features including endian, stdio.h, printf, size of long int, etc.
set(CMAKE_REQUIRED_QUIET ${QUIET})
include(ConfigureChecks)
set(CMAKE_REQUIRED_QUIET FALSE)

# A collection of macros which extend the built-in cmake commands
include(MacroLibrary)

# Database
include(FindMySQL)
find_package(OpenSSL)

find_package(Perl REQUIRED)

########################################################################################################################
# Miscellaneous user-selectable features
########################################################################################################################
option(ENABLE-BINARY-ANALYSIS-TESTS "Enable tests of ROSE binary analysis code" OFF)
option(ENABLE-EXAMPLE-TRANSLATORS-DIRECTORY "Enable compilation and testing of exampleTranslators directory" OFF)
# 1/8/25 we will most likely remove this enable-tests-directory option 
option(ENABLE-TESTS-DIRECTORY "Enable compilation and testing of the ROSE/tests directory" OFF)
# 1/15/24 Turn on ctests until new CMake-Build-RHEL7.sh is merged  
if(NOT ENABLE-TESTS-DIRECTORY AND EXISTS "${PROJECT_SOURCE_DIR}/tests/README.md")
  set(ENABLE-TESTS-DIRECTORY ON CACHE BOOL "Enable compilation and testing of the ROSE/tests directory" FORCE)
endif()
option(ENABLE-TUTORIAL-DIRECTORY "Enable compilation and testing of the ROSE/tutorial directory" OFF)
option(ENABLE-ADVANCED-WARNINGS "Support for an advanced uniform warning level for ROSE development" OFF)

if(ENABLE-ADVANCED-WARNINGS)
  message(WARNING "Using an advanced uniform warning level for ROSE development")
  set(ROSE_USE_UNIFORM_ADVANCED_WARNINGS_SUPPORT 1)
endif()

option(ENABLE-ASSEMBLY-SEMANTICS "Enable semantics-based analysis of assembly code" OFF)
option(ENABLE-CANDL "Support for ScopLib" OFF)

if(ENABLE-CANDL)
  find_path(with-candl "include/candl.h"
    DOC "Path to a valid Candl installation")
endif()

option(ENABLE-CLOOG "Support for Cloog" OFF)
if(ENABLE-CLOOG)
  find_path(with-cloog "include/cloog.h"
    DOC "Path to a valid Cloog installation")
endif()

option(ENABLE-COMPASS2 "build the Compass2 static analysis tool" OFF)
option(ENABLE-EDG-CUDA "Build EDG 4.0 with CUDA support" OFF)
option(ENABLE-EDG-OPENCL "Build EDG 4.0 with OpenCL support" OFF)

option(ENABLE-EDG_UNION_STRUCT_DEBUGGING "Should EDG Union/Struct debugging support be used?" OFF)
if(ENABLE-EDG_UNION_STRUCT_DEBUGGING)
  set(USE_ROSE_EDG_DEBUGGING_SUPPORT 1)
endif()

option(ENABLE-FLTK "Enable FLTK")
if(ENABLE-FLTK)
  find_package(FLTK REQUIRED)
endif()

option(ENABLE-GNU-EXTENSIONS "Enable internal support in ROSE for GNU language extensions" OFF)
if(ENABLE-GNU-EXTENSIONS)
  set(ROSE_SUPPORT_GNU_EXTENSIONS TRUE)
endif()

option(ENABLE-INTERNALFRONTENDDEVELOPMENT "Enable development mode to reduce files required to support work on
  language frontends" OFF)
if(ENABLE-INTERNALFRONTENDDEVELOPMENT)
  set(ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT 1)
endif()

option(ENABLE-MICROSOFT-EXTENSIONS "Enable internal support in ROSE for GNU language extensions" OFF)
if(ENABLE-MICROSOFT-EXTENSIONS)
  set(ROSE_SUPPORT_MICROSOFT_EXTENSIONS TRUE)
endif()

if(ENABLE-JOVIAL) # Find dependencies Aterm and Stratego 
  if(NOT DEFINED ATERM_ROOT) # Require location of aterm to be specified by the user at config time  
    message(FATAL_ERROR "Jovial Support Requires Aterm. Please define -DATERM_ROOT with the path of an Aterm installation on your system")
  endif() 
  if(NOT EXISTS "${ATERM_ROOT}" OR NOT IS_DIRECTORY "${ATERM_ROOT}")
    message(WARNING "${ATERM_ROOT} does not exist or is not a directory.") 
    message(FATAL_ERROR "Jovial Support Requires Aterm. Please set ATERM_ROOT to indicate the path of Aterm.")
  endif() 
  include(FindAterm) # Find Aterm based on ATERM_ROOT  
  find_aterm()
  # Error if we cannot find aterm
  if(NOT USE_ROSE_ATERM_SUPPORT)
    message(FATAL_ERROR "Support for experimental_jovial_frontend requires Aterm library support.\nPlease define ATERM_ROOT with location of Aterm install.")
  else() 
    message(STATUS "Found aterm: ${ATERM_ROOT}") 
  endif()
  if(NOT DEFINED STRATEGO_ROOT) # Require location of sglri executable
    message(FATAL_ERROR "Jovial support requires the Stratego sglri executable. Please define -DSTRATEGO_ROOT with the path of a Stratego installation on your system")
  endif()
  include(FindSGLRI) # Find Stratego based on STRATEGO_ROOT
  find_sglri()
endif()

option(ENABLE-PPL "Support for Parma Polyhedral Library" OFF)
if(ENABLE-PPL)
  find_path(with-ppl "include/ppl.h" DOC "Path to Parma Polyhedral Library installation")
  find_library(libppl ppl)
endif()

option(ENABLE-PURIFY-API "Enable purify API in code" OFF)
if(ENABLE-PURIFY-API)
  set(USE_PURIFY_API 1)
endif()

option(ENABLE-PURIFY-LINKER "Augment the linker with purify" OFF)
if(ENABLE-PURIFY-LINKER)
  set(USE_PURIFY 1)
  set(USE_PURIFY_LINKER 1)
endif()

option(ENABLE-ROSEHPCT "enable build of the ROSE-HPCT module" OFF)
if(ENABLE-ROSEHPCT)
  set(ROSE_BUILD_ROSEHPCT 1)
endif()

option(ENABLE-ROSE-OPENGL "enable openGL" OFF)
if(ENABLE-ROSE-OPENGL)
  find_package(OpenGL)
  find_package(GLUT)
endif()

option(ENABLE-SCOPLIB "Support for ScopLib" OFF)
if(ENABLE-SCOPLIB)
  find_path(with-scoplib "include/scolib.h" DOC "Path to a valid ScopLib installation")
endif()

option(ENABLE-POET "Enable POET support" OFF)

# We are removing BISON 
include(FindBison)
find_bison()

# To use Z3, set Z3_ROOT to the Z3 installation prefix. To avoid Z3, use Z3_ROOT=no.
include(FindZ3)
find_z3()

# To use doxygen, set ENABLE_DOXYGEN=ON and DOXYGEN_ROOT=<doxygen/installation/path> 
if(ENABLE_DOXYGEN) 
  if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND "${DOXYGEN_ROOT}" STREQUAL "")
    message(STATUS "Set DOXYGEN_ROOT to the directory doxygen is installed if you want to explicitly control which Doxygen to use") 
  endif() 
  include(FindDoxygen)
  find_doxygen()
endif()

# FIXME: Deprecated. Remove this and the FindYamlcpp file once YAML-CPP has been eliminated from the ROSE source code.
# To use YAML-CPP, set YAMLCPP_ROOT to the YAML-CPP installation prefix. To avoid YAML-CPP set YAMLCPP_ROOT=no
option(ENABLE-YAML-DEPRECATED "Option to Enable Yaml, which is now deprecated" OFF)
if(ENABLE_YAML) 
  include(FindYamlcpp)
  find_yamlcpp()
endif() 

# To use Dlib, set DLIB_ROOT to the Dlib installation prefix. To avoid Dlib set DLIB_ROOT=no
include(FindDlib)
find_dlib()

# To use Capstone, set CAPSTONE_ROOT to the Capstone installation prefix. To avoid Capstone set CAPSTONE_ROOT=no
include(FindCapstone)
find_capstone()

# To use Cereal, set CEREAL_ROOT to the Cereal installation prefix. To avoid Cereal set CEREAL_ROOT=no
include(FindCereal)
find_cereal()

# To use GCrypt, set cmake .. -DENABLE_GCRYPT=ON -DGCRYPT_ROOT=/path/to/GCrypt/installation
option(ENABLE_GCRYPT "Use the GNU cryptography library, libgcrypt, available from https://www.gnupg.org/related_software/libgcrypt/" OFF)
if(ENABLE_CRYPT)
  include(FindGcrypt)
  find_gcrypt()
  if(GCRYPT_FOUND)
    # To use GPG-Error, set GPGERROR_ROOT to the GPG-Error installation prefix. To avoid GPG-Error set GPGERROR=no
    include(FindGpgError)
    find_gpgerror()
  endif()
endif()

# To use libmagic, set MAGIC_ROOT to the libmagic installation prefix. To avoid libmagic set MAGIC_ROOT=no
include(FindMagic)
find_magic()

# To use libpqxx, set PQXX_ROOT to the libpqxx installation prefix. To avoid libpqxx set PQXX_ROOT=no
include(FindPqxx)
find_pqxx()

# To use Qt, set QT_ROOT to the Qt installation prefix. To avoid Qt set QT_ROOT=no
if(WITH-ROSEQT)
  include(FindQt)
  find_qt()
endif()

# To use libreadline, set READLINE_ROOT to the readline installation prefix. To avoid readline set READLINE_ROOT=no
option(ENABLE-READLINE "Option to enable Readline" OFF)
if(ENABLE-READLINE)
  include(FindReadline)
  find_readline()
endif() 

# To use libspot, set SPOT_ROOT to the spot installation prefix. To avoid spot set SPOT_ROOT=no
if(ENABLE-C)
  include(FindSpot)
  find_spot()
endif()

# To use Yices, set YICES_ROOT to the Yices installation prefix. To avoid Yices set YICES_ROOT=no
include(FindYices)
find_yices()

# Zlib is required by Boost. To find it in a special place, set ZLIB_ROOT to its installation prefix.
include(FindZlib)
find_zlib()

# DWARF library
include(FindDwarf)
find_dwarf()

set(ASSERTION_BEHAVIOR "exit" CACHE STRING "Default behavior of failed assertions. Can be 'abort', 'exit', or 'throw'.")
if(ASSERTION_BEHAVIOR STREQUAL "abort")
  add_definitions("-DROSE_ASSERTION_BEHAVIOR=ROSE_ASSERTION_ABORT")
elseif(ASSERTION_BEHAVIOR STREQUAL "exit")
  add_definitions("-DROSE_ASSERTION_BEHAVIOR=ROSE_ASSERTION_EXIT")
elseif(ASSERTION_BEHAVIOR STREQUAL "throw")
  add_definitions("-DROSE_ASSERTION_BEHAVIOR=ROSE_ASSERTION_THROW")
else()
  message(FATAL_ERROR "ASSERTION_BEHAVIOR should be 'abort', 'exit', or 'throw'")
endif()

find_path(with-backstroke-ross ROSS DOC "Specify the path where ROSS is installed")
find_path(with-backstroke-speedes SPEEDES DOC "Specify the path where SPEEDES is installed")
find_library(with-gomp_omp_runtime_library gomp_omp DOC "Specify the prefix where GOMP Runtime System is installed")
find_path(with-GraphViz_include graphviz.h DOC "Specify the prefix where GraphViz include files are installed")
find_path(with-GraphViz_libs GraphViz_libs DOC "Specify the prefix where GraphViz libraries are installed")
find_path(with-haskell runghc DOC "Path to bin directory containing ghc and runghc.")
find_path(with-ida "ida2sql.py" DOC "Specify the prefix where IDA Pro is installed")
find_path(with-insure insure++ DOC "Specify the prefix where insure++ is installed")
find_path(with-IntelPin include/IntelPinSupport.h DOC "Specify the prefix where Intel Pin Package is installed")
find_path(with-ltdl-include ltdl.h DOC "use the ltdl headers installed in DIR")
find_library(with-ltdl-lib ltdl DOC "Path to ltdl library")
find_path(with-llvm "llvm" DOC "Specify the prefix where LLVM (and opt) is installed")
find_path(with-maple "include/maple.h" DOC "Specify the prefix where Maple is installed")
find_path(with-omni_omp_runtime_support include/omni_omp.h DOC "Specify the prefix where Omni OpenMP Runtime System is installed")
find_path(with-purify bin/purify DOC "Specify the prefix where purify is installed")
if(with-purify)
  set(USE_PURIFY 1)
endif()

find_path(with-QRose QRose DOC "prefix of QRose installation")

if(WITH-ROSEQT)
  find_package(Qt4 REQUIRED)
  include(${QT_USE_FILE})
endif()

find_path(with-rted rted DOC "Configure option to have RTED enabled.")
find_path(with-smt-solver smt-solver DOC "Specify the path to an SMT-LIB compatible SMT solver. Used only for testing")

find_path(with-wine "bin/wine" DOC "Specify the prefix where Wine is installed")
if(with-wine)
  set(ROSE_WINE_INCLUDES -I${with-wine}/include)
endif()

if((NOT WIN32) AND (ENABLE-BINARY-ANALYSIS))
  include(FindOpenSSL)
  include(FindSqlite3)
  find_sqlite3()
endif()

find_package(FLEX)
if((NOT FLEX_FOUND) AND (ENABLE-C))
  message(FATAL_ERROR "Could not find flex command")
endif()

# Find libm.so, libquadmath.so for C Support on Linux and find librt.so for Linux systems
if(NOT WIN32 AND NOT APPLE)
  if(ENABLE-C) 
    # libc.so 
    find_library(C_LIB c REQUIRED) 
    # libm.so 
    find_library(M_LIB m REQUIRED)
    # Find libquadmath.so, prioritize ${GCC_LIB_DIRS}
    file(GLOB GCC_LIB_DIRS "/usr/lib/gcc/x86_64-redhat-linux/*") 
    find_library(QM_LIB 
                 NAMES quadmath libquadmath.so.0 libquadmath.so.0.0.0 libquadmath.so
                 HINTS ${GCC_LIB_DIRS})
  endif()
  # libdl.so 
  find_library(DL_LIB dl REQUIRED) 
  # librt.so, Linux/POSIX Timers and clocks
  find_library(RT_LIB rt QUIET)
endif()

# Only use quadmath if C/C++ is enable and it is available (also exclude windows)
if(ENABLE-C AND NOT ROSE_SUPPORT_MICROSOFT_EXTENSIONS)
  set(COMPILE_TEST_SOURCE "
  #include <quadmath.h>
  int main() { return 0; }")
  
  # Perform the compile test
  check_cxx_source_compiles("${COMPILE_TEST_SOURCE}" HAVE_QUADMATH_COMPILE)
  
  # Determine whether quadmath is usable
  if(HAVE_QUADMATH_COMPILE)
    set(ROSE_USE_EDG_QUAD_FLOAT 1)
    set(SUPPORT_FLOAT128 ON CACHE BOOL "Variable that will persist to EDG submodule telling it to include quadmath")
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
      message(STATUS "quadmath is available and usable, ROSE_USE_EDG_QUAD_FLOAT set to 1")
    endif() 
  else()
    # Comment this line out for issues with clang 
    set(ROSE_USE_EDG_QUAD_FLOAT 0)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
      message(STATUS "quadmath is not available or not usable, ROSE_USE_EDG_QUAD_FLOAT set to 0")
    endif()
  endif() 
endif()

# This is essential to find the right include path from either build or installation tree for a translator
if(HAVE_DLFCN_H)
  if(HAVE_DLADDR)
    set(use_rose_in_build_tree_var TRUE)
    # this following line won't work since it only set the environment variable for cmake's session not for ctest
    # session. Still no good way to set it within cmake fortunately,
    set($ENV{ROSE_IN_BUILD_TREE} ${ROSE_TOP_BINARY_DIR})
  endif()
else()
  set(use_rose_in_build_tree_var, FALSE)
endif()

########################################################################################################################
# Alt Back-end compilers -- set alt rose_config.h variables
########################################################################################################################
set(WITH-ALT-BACKEND-CXX-COMPILER "" CACHE STRING "Specify an alternative C++ back-end compiler")
if(WITH-ALT-BACKEND-CXX-COMPILER)
  if(EXISTS ${WITH-ALT-BACKEND-CXX-COMPILER})
    set(BACKEND_CXX_COMPILER ${WITH-ALT-BACKEND-CXX-COMPILER})
  else()
    message(WARNING "${WITH-ALT-BACKEND-CXX-COMPILER} does not exist; pecify an absolute path to the compiler")
  endif()
endif()

set(WITH-ALT-BACKEND-C-COMPILER "" CACHE STRING "Specify an alternative C back-end compiler")
if(WITH-ALT-BACKEND-C-COMPILER)
  if(EXISTS ${WITH-ALT-BACKEND-C-COMPILER})
    set(BACKEND_C_COMPILER ${WITH-ALT-BACKEND-C-COMPILER})
    message(STATUS "just set backend C compiler to ${BACKEND_C_COMPILER}")
  else()
    message(WARNING "${WITH-ALT-BACKEND-C-COMPILER} does not exist; specify an absolute path to the compiler")
  endif()
endif()

set(WITH-ALT-BACKEND-FORTRAN-COMPILER "" CACHE STRING "Specify an alternative fortran back-end compiler")
if(WITH-ALT-BACKEND-FORTRAN-COMPILER)
  if(EXISTS ${WITH-ALT-BACKEND-FORTRAN-COMPILER})
    set(BACKEND_FORTRAN_COMPILER ${WITH-ALT-BACKEND-FORTRAN-COMPILER})
  else()
    message(WARNING "${WITH-ALT-BACKEND-FORTRAN-COMPILER} does not exist; specify an absolute path to the compiler")
  endif()
endif()

set(WITH-ALT-BACKEND-JAVA-COMPILER "" CACHE STRING "Specify an alternative java back-end compiler")
if(WITH-ALT-BACKEND-JAVA-COMPILER)
  if(EXISTS ${WITH-ALT-BACKEND-JAVA-COMPILER})
    set(BACKEND_JAVA_COMPILER ${WITH-ALT-BACKEND-JAVA-COMPILER})
  else()
    message(WARNING "${WITH-ALT-BACKEND-JAVA-COMPILER} does not exist; specify an absolute path to the compiler")
  endif()
endif()

########################################################################################################################
# Determine libraries that rose-compiler and librose.so will link to 
########################################################################################################################
# Initialize an empty list to collect all common linked third-party libraries for rose
set(link_with_libraries "") 
# All OS platforms require boost 
list(APPEND link_with_libraries ${Boost_LIBRARIES}) 

# Linux dependencies
if(NOT WIN32 AND NOT APPLE)
  # Conditionally append libc.so to list of libraries rose will link to 
  if(C_LIB AND NOT C_LIB STREQUAL "C_LIB-NOTFOUND")
    list(APPEND link_with_libraries ${C_LIB})
  endif()
  # Conditionally append libm.so to list of libraries rose will link to 
  if(M_LIB AND NOT M_LIB STREQUAL "M_LIB-NOTFOUND")
    list(APPEND link_with_libraries ${M_LIB})
  endif()
  # Conditionally append libdl.so to list of libraries rose will link to 
  if(M_LIB AND NOT DL_LIB STREQUAL "DL_LIB-NOTFOUND")
    list(APPEND link_with_libraries ${DL_LIB})
  endif()
  # Conditionally append libquadmath.so to list of libraries rose will link to 
  if(QM_LIB AND NOT QM_LIB STREQUAL "QM_LIB-NOTFOUND")
    list(APPEND link_with_libraries ${QM_LIB})    
  endif()
  # Conditionally append librt.so to list of libraries rose will link to  
  if(RT_LIB AND NOT RT_LIB STREQUAL "RT_LIB-NOTFOUND")
    list(APPEND link_with_libraries ${RT_LIB})
  endif()    
  # Conditionally append Pthreads to list of libraries rose will link to  
  list(APPEND link_with_libraries ${CMAKE_THREAD_LIBS_INIT})
endif() 

# Mac OS dependencies 
if(APPLE)
  # Conditionally append MLIB
  if(M_LIB AND NOT M_LIB STREQUAL "M_LIB-NOTFOUND")
    list(APPEND link_with_libraries ${M_LIB})
  endif()
  list(APPEND link_with_libraries ${CMAKE_THREAD_LIBS_INIT})
endif()

# Windows dependencies 
if(WIN32)
  list(APPEND link_with_libraries shlwapi.lib psapi.lib)
endif()

# Special Case for rose tool 
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND ENABLE-CPP)
  if(NOT ENABLE-C) 
    list(APPEND link_with_libraries stdc++)
  endif() 
endif() 
if(HAVE_LIBDL)
  list(APPEND link_with_libraries dl)
endif()

# Check compilers and version numbers. The module is located in src/cmake/modules.
if(ENABLE-C)
  include(roseChooseBackendCompiler)
  include(roseGenerateBackendCompilerSpecificHeaders)
  install(DIRECTORY ${CMAKE_BINARY_DIR}/include-staging/ DESTINATION include/edg)
else()
  # These are required to be defined even when C/C++ analysis is not enabled, but their values don't matter
  set(BACKEND_C_COMPILER_NAME_WITHOUT_PATH "no_compiler")
  set(BACKEND_C_COMPILER_MAJOR_VERSION_NUMBER "0")
  set(BACKEND_C_COMPILER_MINOR_VERSION_NUMBER "0")
  set(BACKEND_C_COMPILER_PATCH_LEVEL_NUMBER "0")
  set(BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH "no_compiler")
  set(BACKEND_CXX_COMPILER_MAJOR_VERSION_NUMBER "0")
  set(BACKEND_CXX_COMPILER_MINOR_VERSION_NUMBER "0")
  set(BACKEND_CXX_COMPILER_PATCH_LEVEL_NUMBER "0")
endif()

########################################################################################################################
# Summary of results
########################################################################################################################
if(VERBOSE)
  # if you are building in-source, this is the same as CMAKE_SOURCE_DIR, otherwise
  # this is the top level directory of your build tree
  message(STATUS "CMAKE_BINARY_DIR:                   ${CMAKE_BINARY_DIR}")

  # if you are building in-source, this is the same as CMAKE_CURRENT_SOURCE_DIR, otherwise this
  # is the directory where the compiled or generated files from the current CMakeLists.txt will go to
  message(STATUS "CMAKE_CURRENT_BINARY_DIR:           ${CMAKE_CURRENT_BINARY_DIR}")

  # this is the directory, from which cmake was started, i.e. the top level source directory
  message(STATUS "CMAKE_SOURCE_DIR:                   ${CMAKE_SOURCE_DIR}")

  # this is the directory where the currently processed CMakeLists.txt is located in
  message(STATUS "CMAKE_CURRENT_SOURCE_DIR:           ${CMAKE_CURRENT_SOURCE_DIR}")

  # contains the full path to the top level directory of your build tree
  message(STATUS "PROJECT_BINARY_DIR:                 ${PROJECT_BINARY_DIR}")

  # contains the full path to the root of your project source directory,
  # i.e. to the nearest directory where CMakeLists.txt contains the PROJECT() command
  message(STATUS "PROJECT_SOURCE_DIR:                 ${PROJECT_SOURCE_DIR}")

  # set this variable to specify a common place where CMake should put all executable files
  # (instead of CMAKE_CURRENT_BINARY_DIR)
  message(STATUS "EXECUTABLE_OUTPUT_PATH:             ${EXECUTABLE_OUTPUT_PATH}")

  # set this variable to specify a common place where CMake should put all libraries
  # (instead of CMAKE_CURRENT_BINARY_DIR)
  message(STATUS "LIBRARY_OUTPUT_PATH:                ${LIBRARY_OUTPUT_PATH}")

  # tell CMake to search first in directories listed in CMAKE_MODULE_PATH
  # when you use FIND_PACKAGE() or INCLUDE()
  message(STATUS "CMAKE_MODULE_PATH:                  ${CMAKE_MODULE_PATH}")

  # this is the complete path of the cmake which runs currently (e.g. /usr/local/bin/cmake)
  message(STATUS "CMAKE_COMMAND:                      ${CMAKE_COMMAND}")

  # this is the CMake installation directory
  message(STATUS "CMAKE_ROOT:                         ${CMAKE_ROOT}")

  # this is the filename including the complete path of the file where this variable is used.
  message(STATUS "CMAKE_CURRENT_LIST_FILE:            ${CMAKE_CURRENT_LIST_FILE}")

  # this is used when searching for include files e.g. using the FIND_PATH() command.
  message(STATUS "CMAKE_INCLUDE_PATH:                 ${CMAKE_INCLUDE_PATH}")

  # this is used when searching for libraries e.g. using the FIND_LIBRARY() command.
  message(STATUS "CMAKE_LIBRARY_PATH:                 ${CMAKE_LIBRARY_PATH}")

  # the complete system name, e.g. "Linux-2.4.22", "FreeBSD-5.4-RELEASE" or "Windows 5.1"
  message(STATUS "CMAKE_SYSTEM:                       ${CMAKE_SYSTEM}")

  # the short system name, e.g. "Linux", "FreeBSD" or "Windows"
  message(STATUS "CMAKE_SYSTEM_NAME:                  ${CMAKE_SYSTEM_NAME}")

  # only the version part of CMAKE_SYSTEM
  message(STATUS "CMAKE_SYSTEM_VERSION:               ${CMAKE_SYSTEM_VERSION}")

  # the processor name (e.g. "Intel(R) Pentium(R) M processor 2.00GHz")
  message(STATUS "CMAKE_SYSTEM_PROCESSOR:             ${CMAKE_SYSTEM_PROCESSOR}")

  # is TRUE on all UNIX-like OS's, including Apple OS X and CygWin
  message(STATUS "UNIX:                               ${UNIX}")

  # is TRUE on Windows, including CygWin
  message(STATUS "WIN32:                              ${WIN32}")

  # is TRUE on Apple OS X
  message(STATUS "APPLE:                              ${APPLE}")

  # is TRUE when using the MinGW compiler in Windows
  message(STATUS "MINGW:                              ${MINGW}")

  # is TRUE on Windows when using the CygWin version of cmake
  message(STATUS "CYGWIN:                             ${CYGWIN}")

  # is TRUE on Windows when using a Borland compiler
  message(STATUS "BORLAND:                            ${BORLAND}")

  # is RHEL
  message(STATUS "Red Hat version:                    ${ROSE_HOST_OS_IS_RHEL}")

  # Microsoft compiler
  message(STATUS "MSVC:                               ${MSVC}")
  message(STATUS "MSVC_IDE:                           ${MSVC_IDE}")
  message(STATUS "MSVC60:                             ${MSVC60}")
  message(STATUS "MSVC70:                             ${MSVC70}")
  message(STATUS "MSVC71:                             ${MSVC71}")
  message(STATUS "MSVC80:                             ${MSVC80}")
  message(STATUS "CMAKE_COMPILER_2005:                ${CMAKE_COMPILER_2005}")

  # set this to true if you don't want to rebuild the object files if the rules have changed,
  # but not the actual source files or headers (e.g. if you changed the some compiler switches)
  message(STATUS "CMAKE_SKIP_RULE_DEPENDENCY:         ${CMAKE_SKIP_RULE_DEPENDENCY}")

  # since CMake 2.1 the install rule depends on all, i.e. everything will be built before installing.
  # If you don't like this, set this one to true.
  message(STATUS "CMAKE_SKIP_INSTALL_ALL_DEPENDENCY:  ${CMAKE_SKIP_INSTALL_ALL_DEPENDENCY}")

  # If set, runtime paths are not added when using shared libraries. Default it is set to OFF
  message(STATUS "CMAKE_SKIP_RPATH:                   ${CMAKE_SKIP_RPATH}")

  # set this to true if you are using makefiles and want to see the full compile and link
  # commands instead of only the shortened ones
  message(STATUS "CMAKE_VERBOSE_MAKEFILE:             ${CMAKE_VERBOSE_MAKEFILE}")

  # this will cause CMake to not put in the rules that re-run CMake. This might be useful if
  # you want to use the generated build files on another machine.
  message(STATUS "CMAKE_SUPPRESS_REGENERATION:        ${CMAKE_SUPPRESS_REGENERATION}")

  # A simple way to get switches to the compiler is to use ADD_DEFINITIONS().
  # But there are also two variables exactly for this purpose:
  #   CMAKE_C_FLAGS:   the compiler flags for compiling C sources, and
  #   CMAKE_CXX_FLAGS: the compiler flags for compiling C++ sources
  message(STATUS "CMAKE_C_FLAGS:                      ${CMAKE_C_FLAGS}")
  message(STATUS "CMAKE_CXX_FLAGS:                    ${CMAKE_CXX_FLAGS}")

  # Choose the type of build.  Example: SET(CMAKE_BUILD_TYPE Debug)
  message(STATUS "CMAKE_BUILD_TYPE:                   ${CMAKE_BUILD_TYPE}")

  # if this is set to ON, then all libraries are built as shared libraries by default.
  message(STATUS "BUILD_SHARED_LIBS:                  ${BUILD_SHARED_LIBS}")

  # the compiler used for C files
  message(STATUS "CMAKE_C_COMPILER:                   ${CMAKE_C_COMPILER}")

  # the compiler used for C++ files
  message(STATUS "CMAKE_CXX_COMPILER:                 ${CMAKE_CXX_COMPILER}")

  # if the compiler is a variant of gcc, this should be set to 1
  message(STATUS "CMAKE_COMPILER_IS_GNUCC:            ${CMAKE_COMPILER_IS_GNUCC}")

  # if the compiler is a variant of g++, this should be set to 1
  message(STATUS "CMAKE_COMPILER_IS_GNUCXX:           ${CMAKE_COMPILER_IS_GNUCXX}")

  # the tools for creating libraries
  message(STATUS "CMAKE_AR:                           ${CMAKE_AR}")
  message(STATUS "CMAKE_RANLIB:                       ${CMAKE_RANLIB}")

endif()


########################################################################################################################
# ROSE headers are scattered throughout the source tree.
########################################################################################################################
set(ROSE_INCLUDES
  ${ROSE_TOP_BINARY_DIR}
  ${ROSE_TOP_BINARY_DIR}/src/frontend/SageIII/
  ${ROSE_TOP_BINARY_DIR}/src/frontend/SageIII/astFileIO
  ${ROSE_TOP_SRC_DIR}
  ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII
  ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/sage_support
  ${ROSE_TOP_SRC_DIR}/src/ROSETTA/src/
  ${ROSE_TOP_SRC_DIR}/src
  ${ROSE_TOP_SRC_DIR}/src/generated
  ${ROSE_TOP_SRC_DIR}/src/frontend/CxxFrontend/EDG/EDG_SAGE_Connection/
  ${ROSE_TOP_SRC_DIR}/src/frontend/CxxFrontend/EDG/EDG_3.3/src
  ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII
  ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/astFixup
  ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/astPostProcessing
  ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/astMerge
  ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/astVisualization
  ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/includeDirectivesProcessing
  ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/astFileIO
  ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/sageInterface
  ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/virtualCFG
  ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/astTokenStream
  ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/astHiddenTypeAndDeclarationLists
  ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/astFileIO
  ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII/astFromString
  ${ROSE_TOP_SRC_DIR}/src/frontend/OpenFortranParser_SAGE_Connection
  ${ROSE_TOP_SRC_DIR}/src/frontend/PHPFrontend
  ${ROSE_TOP_SRC_DIR}/src/frontend/PythonFrontend
  ${ROSE_TOP_SRC_DIR}/src/frontend/BinaryFormats
  ${ROSE_TOP_SRC_DIR}/src/frontend/BinaryLoader
  ${ROSE_TOP_SRC_DIR}/src/frontend/Disassemblers
  ${ROSE_TOP_SRC_DIR}/src/frontend/ExecFormats
  ${ROSE_TOP_SRC_DIR}/src/frontend
  ${ROSE_TOP_SRC_DIR}/src/backend/unparser
  ${ROSE_TOP_SRC_DIR}/src/backend/unparser/formatSupport
  ${ROSE_TOP_SRC_DIR}/src/backend/unparser/languageIndependenceSupport
  ${ROSE_TOP_SRC_DIR}/src/backend/unparser/CxxCodeGeneration
  ${ROSE_TOP_SRC_DIR}/src/backend/unparser/FortranCodeGeneration
  ${ROSE_TOP_SRC_DIR}/src/backend/unparser/JavaCodeGeneration
  ${ROSE_TOP_SRC_DIR}/src/backend/unparser/PHPCodeGeneration
  ${ROSE_TOP_SRC_DIR}/src/backend/unparser/PythonCodeGeneration
  ${ROSE_TOP_SRC_DIR}/src/backend/unparser/AdaCodeGeneration
  ${ROSE_TOP_SRC_DIR}/src/backend/unparser/JovialCodeGeneration
  ${ROSE_TOP_SRC_DIR}/src/backend/asmUnparser
  ${ROSE_TOP_SRC_DIR}/src/util
  ${ROSE_TOP_SRC_DIR}/src/util/support
  ${ROSE_TOP_SRC_DIR}/src/util/graphs
  ${ROSE_TOP_SRC_DIR}/src/util/stringSupport
  ${ROSE_TOP_SRC_DIR}/src/util/commandlineProcessing
  ${ROSE_TOP_SRC_DIR}/src/midend
  ${ROSE_TOP_SRC_DIR}/src/midend/abstractHandle
  ${ROSE_TOP_SRC_DIR}/src/midend/abstractLayer
  ${ROSE_TOP_SRC_DIR}/src/midend/abstractMemoryObject
  ${ROSE_TOP_SRC_DIR}/src/midend/astDiagnostics
  ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/astInlining
  ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/astOutlining
  ${ROSE_TOP_SRC_DIR}/src/midend/astProcessing
  ${ROSE_TOP_SRC_DIR}/src/midend/astMatching
  ${ROSE_TOP_SRC_DIR}/src/midend/astQuery
  ${ROSE_TOP_SRC_DIR}/src/midend/astRewriteMechanism
  ${ROSE_TOP_SRC_DIR}/src/midend/astUtil
  ${ROSE_TOP_SRC_DIR}/src/midend/astUtil/annotation
  ${ROSE_TOP_SRC_DIR}/src/midend/astUtil/astInterface
  ${ROSE_TOP_SRC_DIR}/src/midend/astUtil/astSupport
  ${ROSE_TOP_SRC_DIR}/src/midend/astUtil/symbolicVal
  ${ROSE_TOP_SRC_DIR}/src/midend/astUtil/dependenceTracking
  ${ROSE_TOP_SRC_DIR}/src/midend/BinaryAnalysis
  ${ROSE_TOP_SRC_DIR}/src/midend/BinaryAnalysis/dataflowanalyses
  ${ROSE_TOP_SRC_DIR}/src/midend/BinaryAnalysis/graph
  ${ROSE_TOP_SRC_DIR}/src/midend/BinaryAnalysis/instructionSemantics
  ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/loopProcessing/computation
  ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/loopProcessing/depGraph
  ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/loopProcessing/depInfo
  ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/loopProcessing/driver
  ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/loopProcessing/outsideInterface
  ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/loopProcessing/prepostTransformation
  ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/loopProcessing/slicing
  ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/ompLowering
  ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis
  ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis/CFG
  ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis/CallGraphAnalysis
  ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis/OAWrap
  ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis/bitvectorDataflow
  ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis/VirtualFunctionAnalysis
  ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis/defUseAnalysis
  ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis/distributedMemoryAnalysis
  ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis/dominanceAnalysis
  ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis/pointerAnal
  ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis/staticSingleAssignment
  ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis/staticInterproceduralSlicing
  ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis/valuePropagation
  ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/partialRedundancyElimination
  ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/finiteDifferencing
  ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/functionCallNormalization
  ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/constantFolding
  ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/implicitCodeGeneration
  ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/runtimeTransformation
  ${ROSE_TOP_SRC_DIR}/src/roseSupport
  ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/MSTL
  ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/qrose/Framework
  ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/qrose/Widgets
  ${ROSE_TOP_SRC_DIR}/src/util/graphs
  ${ROSE_TOP_SRC_DIR}/src/midend/astUtil/astInterface
  ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/json)

if(SQLITE3_FOUND_LIB)
  list(APPEND ROSE_INCLUDES ${ROSE_TOP_SRC_DIR}/src/roseExtensions/sqlite3x)
endif()

if(ENABLE-POET)
  list(APPEND ROSE_INCLUDES ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/POET)
endif()

if(WIN32)
  list(APPEND ROSE_INCLUDES ${ROSE_TOP_SRC_DIR}/winspecific)
endif()

include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES})

if(WITH-ROSEQT)
  list(APPEND ROSE_INCLUDES
    ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/qrose/Components/Common
    ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/qrose/Components/QueryBox
    ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/qrose/Components/SourceBox
    ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/qrose/Components/TreeBox)
  set(qtWidgets_INCLUDE_DIRS
    ${ROSE_SOURCE_DIR}/src/roseExtensions/qtWidgets/AsmInstructionBar
    ${ROSE_SOURCE_DIR}/src/roseExtensions/qtWidgets/AsmView
    ${ROSE_SOURCE_DIR}/src/roseExtensions/qtWidgets/AstBrowserWidget
    ${ROSE_SOURCE_DIR}/src/roseExtensions/qtWidgets/AstGraphWidget
    ${ROSE_SOURCE_DIR}/src/roseExtensions/qtWidgets/AstProcessing
    ${ROSE_SOURCE_DIR}/src/roseExtensions/qtWidgets/BeautifiedAst
    ${ROSE_SOURCE_DIR}/src/roseExtensions/qtWidgets/FlopCounter
    ${ROSE_SOURCE_DIR}/src/roseExtensions/qtWidgets/InstructionCountAnnotator
    ${ROSE_SOURCE_DIR}/src/roseExtensions/qtWidgets/KiviatView
    ${ROSE_SOURCE_DIR}/src/roseExtensions/qtWidgets/MetricsConfig
    ${ROSE_SOURCE_DIR}/src/roseExtensions/qtWidgets/MetricFilter
    ${ROSE_SOURCE_DIR}/src/roseExtensions/qtWidgets/MetricsKiviat
    ${ROSE_SOURCE_DIR}/src/roseExtensions/qtWidgets/NodeInfoWidget
    ${ROSE_SOURCE_DIR}/src/roseExtensions/qtWidgets/ProjectManager
    ${ROSE_SOURCE_DIR}/src/roseExtensions/qtWidgets/PropertyTreeWidget
    ${ROSE_SOURCE_DIR}/src/roseExtensions/qtWidgets/QCodeEditWidget
    ${ROSE_SOURCE_DIR}/src/roseExtensions/qtWidgets/QCodeEditWidget/QCodeEdit
    ${ROSE_SOURCE_DIR}/src/roseExtensions/qtWidgets/QCodeEditWidget/QCodeEdit/widgets
    ${ROSE_SOURCE_DIR}/src/roseExtensions/qtWidgets/QtGradientEditor
    ${ROSE_SOURCE_DIR}/src/roseExtensions/qtWidgets/RoseCodeEdit
    ${ROSE_SOURCE_DIR}/src/roseExtensions/qtWidgets/RoseFileSelector
    ${ROSE_SOURCE_DIR}/src/roseExtensions/qtWidgets/SrcBinView
    ${ROSE_SOURCE_DIR}/src/roseExtensions/qtWidgets/TaskSystem
    ${ROSE_SOURCE_DIR}/src/roseExtensions/qtWidgets/TreeModel
    ${ROSE_SOURCE_DIR}/src/roseExtensions/qtWidgets/util
    ${ROSE_SOURCE_DIR}/src/roseExtensions/qtWidgets/WidgetCreator)
endif()
set(QT_INCLUDES ${ROSE_INCLUDES} ${Boost_INCLUDE_DIRS})

########################################################################################################################
# Set output directories
########################################################################################################################
# Set default output paths for libraries and directories.  This causes all the libraries to be built into a single lib
# directory, and all the executables to be built into a single bin directory.  If these variables are not set, then by
# default the targets will be littered throughout the binary directory, matching the structure of the source directory.
#
# Organizing the output of the build directory this way is important when building on Windows.  By default, Windows
# .exes do not know where to find their required .dlls.  The easiest way to overcome this problem is by placing the .exe
# and all its dependent .dlls in the same directory.  We accomplish this with CMake by setting
# CMAKE_RUNTIME_OUTPUT_DIRECTORY.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Set up RPATH so that our executables will be able to find their dependent
# libraries using relative paths.
if(APPLE)
  set(origin "@executable_path")
  set(CMAKE_INSTALL_RPATH "${origin}/../lib;${origin}/") # May rm this 
endif()

if(NOT DEFINED CMAKE_INSTALL_RPATH)
  message(STATUS "Setting CMAKE_INSTALL_RPATH") 
  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib;${CMAKE_INSTALL_PREFIX}/bin;${Boost_LIBRARY_DIRS}")
  if(CMAKE_BUILD_TYPE STREQUAL "Debug") 
    message(STATUS "CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_RPATH}")
  endif() 
endif()

if(NOT DEFINED CMAKE_INSTALL_RPATH_USE_LINK_PATH) 
  message(STATUS "Setting CMAKE_INSTALL_RPATH_USE_LINK_PATH")
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE) 
  if(CMAKE_BUILD_TYPE STREQUAL "Debug") 
    message(STATUS "CMAKE_INSTALL_RPATH_USE_LINK_PATH ${CMAKE_INSTALL_RPATH_USE_LINK_PATH}")
  endif() 
endif() 

########################################################################################################################
# Recursion
########################################################################################################################
# Recursion in CMake is not automatic. We have to tell it where the other CMakeList.txt files are located rather than
# letting it find them automatically.
add_subdirectory(LicenseInformation)
add_subdirectory(config)
add_subdirectory(src)
add_subdirectory(exampleTranslators)
add_subdirectory(scripts)

# Copy CTestCustom.cmake into the binary directory.  This file tells CTest to ignore some output that it otherwise
# considers compilation errors.
configure_file(cmake/CTestCustom.cmake.in ${CMAKE_BINARY_DIR}/CTestCustom.cmake @ONLY)

#TODO: better support for other compilers & operating systems
if(ENABLE-C)
  set(C_INCLUDE_STRING "{\"${BACKEND_C_COMPILER_NAME_WITHOUT_PATH}_HEADERS\"")
  set(CXX_INCLUDE_STRING "{\"${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH}_HEADERS\"")
  find_program(shell sh)
  if(shell AND CMAKE_COMPILER_IS_GNUCXX)
    execute_process(
      COMMAND ${shell} ${CMAKE_SOURCE_DIR}/config/dirincludes "./include-staging/" "${BACKEND_C_COMPILER_NAME_WITHOUT_PATH}_HEADERS"
      OUTPUT_VARIABLE C_includeString)
    string(STRIP "${C_includeString}" C_includeString)

    execute_process(
      COMMAND ${shell} ${CMAKE_SOURCE_DIR}/config/dirincludes "./include-staging/" "${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH}_HEADERS"
      OUTPUT_VARIABLE CXX_includeString)
    string(STRIP "${CXX_includeString}" CXX_includeString)

    set(CXX_INCLUDE_STRING "${CXX_INCLUDE_STRING} ${CXX_includeString}")
    execute_process(
      COMMAND ${shell} ${CMAKE_SOURCE_DIR}/config/get_compiler_header_dirs gcc c gnu
      OUTPUT_VARIABLE C_backend_includes)

    execute_process(
      COMMAND ${shell} ${CMAKE_SOURCE_DIR}/config/get_compiler_header_dirs g++ c++ gnu
      OUTPUT_VARIABLE CXX_backend_includes)
    string(REPLACE "\n" ";" C_backend_includes "${C_backend_includes}")
    string(REPLACE "\n" ";" CXX_backend_includes "${CXX_backend_includes}")

    foreach(C_include_dir ${C_backend_includes})
      string(STRIP "${C_include_dir}" C_include_dir)
      set(C_INCLUDE_STRING "${C_INCLUDE_STRING}, \"${C_include_dir}\"")
    endforeach()
    set(C_INCLUDE_STRING "${C_INCLUDE_STRING} ${C_includeString}, \"/usr/include\"")

    foreach(CXX_include_dir ${CXX_backend_includes})
      string(STRIP "${CXX_include_dir}" CXX_include_dir)
      set(CXX_INCLUDE_STRING "${CXX_INCLUDE_STRING}, \"${CXX_include_dir}\"")
    endforeach()
    set(CXX_INCLUDE_STRING "${CXX_INCLUDE_STRING}, \"/usr/include\"")

    # In the Autools build system, ROSE stores compiler specific headers in two distinct locations:
    #    In the build tree, the directory is named $ROSE_BUILD/include-staging
    #    In the installation tree, the directory is named $ROSE_INSTALL/include
    # During make-install, the content of $ROSE_BUILD/include-staging is simply copied over into $ROSE_INSTALL/include.
    #
    # If a ROSE translator is detected as being run from the build tree, then the associated header file include paths
    # are added to the translator's command line, e.g. <translator> -I$ROSE_BUILD/include-staging/<path>. Similarly, if
    # the ROSE translator is detected as being run from the install tree, the command line will contain the associated
    # paths in the installation location, e.g. -I$ROSE_INSTALL/include/<path>
    #
    # The setup of CMake for ROSE, on the other hand, does not make this distinction. Instead, the CMake system utilizes
    # a single directory $ROSE_CMAKE_BIN/include for the same purpose.
    execute_process(
      COMMAND ${CMAKE_COMMAND} -E create_symlink
      ${CMAKE_BINARY_DIR}/include-staging ${CMAKE_BINARY_DIR}/include)
  elseif(MSVC)
    set(INCLUDE_LIST $ENV{INCLUDE})
    string(REPLACE "\\" "\\\\" INCLUDE_LIST "${INCLUDE_LIST}")
    foreach(C_include_dir ${INCLUDE_LIST})
      string(STRIP "${C_include_dir}" C_include_dir)
      set(C_INCLUDE_STRING "${C_INCLUDE_STRING}, \"${C_include_dir}\"")
    endforeach()
    set(CXX_INCLUDE_STRING "${C_INCLUDE_STRING}")
  endif()
  set(C_INCLUDE_STRING "${C_INCLUDE_STRING}}")
  set(CXX_INCLUDE_STRING "${CXX_INCLUDE_STRING}}")
else()
  set(C_INCLUDE_STRING {\"none\"})
  set(CXX_INCLUDE_STRING {\"none\"})
endif()

########################################################################################################################
# Private and public build-time configuration files
########################################################################################################################
# Private configuration file.
#
# The rose_config.h file contains CPP macros describing what features were detected during the configuration phase.
# This file should not be installed and should not be #include'd into user-level code because it pollutes the global
# name space.  This needs to be here at the end in order to catch all the variables defined above such as compiler
# names, versions, etc.
configure_file(${ROSE_TOP_SRC_DIR}/rose_config.h.in.cmake ${ROSE_TOP_BINARY_DIR}/rose_config.h)

# Public configuration file.
#
# The rosePublicConfig.h header is generated from rose_config.h by copying certain CPP macros from rose_config.h to
# rosePublicConfig.h and changing their names so as not to pollute the global name space.  See
# scripts/publicConfiguration.pl for details.

########################################################################################################################
# Ancillary ROSE components. These are things that are not part of the ROSE library.
########################################################################################################################
add_custom_target(tools)                 # ie "make tools"

if(NOT CMAKE_GENERATOR STREQUAL "Ninja")
  add_subdirectory(tools EXCLUDE_FROM_ALL) # making tools is separate from making rose
  add_custom_target(install-tools          # ie "make install-tools"
    COMMAND ${CMAKE_COMMAND} --build tools/ --target install
  )
else()
  # Ninja does not create a separate build.ninja file in tools/ like Make does
  add_subdirectory(tools)                  # making tools is not separate from making rose
  add_custom_target(install-tools          # ie "make install-tools" is called from top-level build.ninja
    COMMAND ${CMAKE_COMMAND} --build . --target install
  )
endif()

# 1/15/24 Turn on ctests until new CMake-Build-RHEL7.sh is merged  
if(ENABLE-TESTS-DIRECTORY)
  enable_testing()
  add_subdirectory(tests)
  add_subdirectory(tutorial)
endif()

########################################################################################################################
# Miscellaneous final actions
########################################################################################################################
# These actions don't work on Linux Debian-based systems during "make install" and appear to not be necessary anyway.
if(NOT UNIX)
  # This has to remain the last subdirectory so that all the targets are installed before bundle.cmake runs.
  add_subdirectory(cmake)
  # This include file defines how CPack should create our installers.
  include(cmake/ROSECPack.cmake)
endif()
