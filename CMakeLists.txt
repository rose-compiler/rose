# Please be careful about where to put your tests and variable settings  -*- cmake -*-
# The order matters!!

#----- put platform independent settings in this section-----------------
cmake_minimum_required(VERSION 2.8)
project (ROSE CXX C)

# Why do we have to have a copy of some standard built-in modules inside rose?
set( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" "${CMAKE_SOURCE_DIR}/cmake/modules"  ${CMAKE_MODULE_PATH})
#message ("CMAKE_MODULE_PATH = ${CMAKE_MODULE_PATH}")

#-------------------------------------------------------------------------------
# ROSE SCM Version Information
#
#   1. Grab the SCM information from Git, otherwise
#   2. Grab the SCM information from the $ROSE/VERSION file
#-------------------------------------------------------------------------------
set(ROSE_SCM_VERSION_ID_REGEX               "^[0-9a-zA-Z]+$") # {40}
set(ROSE_SCM_VERSION_ID_LENGTH_EXACT        40)
set(ROSE_SCM_VERSION_UNIX_DATE_REGEX        "^[0-9]+$") # {10,}
set(ROSE_SCM_VERSION_UNIX_DATE_LENGTH_MIN   10)
set(ROSE_SCM_VERSION_FILE                   "${PROJECT_SOURCE_DIR}/VERSION")
set(ROSE_SCM_VERSION_FILE_FIELD_COUNT       2)

message(STATUS "Check for ROSE source tree type")
if(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/.git)
  message(STATUS "Check for ROSE source tree type - Git")
  find_package(Git)
  if(NOT GIT_FOUND)
      string(REPLACE ":" "\n\t" ENV_PATH "$ENV{PATH}")
      message(
          FATAL_ERROR
          "git executable not found. You might need to add /path/to/git/bin "
          "to your \$PATH environment variable.\n"
          "PATH=${ENV_PATH}"
      )
  else()
      message(
          STATUS
          "Detecting ROSE SCM version information (from Git)"
      )

      # HEAD
      execute_process(
          COMMAND             "${GIT_EXECUTABLE}" rev-parse HEAD
          WORKING_DIRECTORY   ${PROJECT_SOURCE_DIR}
          ERROR_VARIABLE      stderr
          RESULT_VARIABLE     error_code
          OUTPUT_VARIABLE     ROSE_SCM_VERSION_ID
          OUTPUT_STRIP_TRAILING_WHITESPACE
      )
      if (NOT error_code EQUAL 0)
          message(
              FATAL_ERROR
              "(status=${error_code}) "
              "Failed to determine the ROSE SCM version id: "
              "${ROSE_SCM_VERSION_ID}${stderr}"
          )
      endif()

      execute_process(
        # Author time (%at) of the HEAD commit
          COMMAND             "${GIT_EXECUTABLE}" log -1 --format=%at HEAD
          WORKING_DIRECTORY   ${PROJECT_SOURCE_DIR}
          ERROR_VARIABLE      stderr
          RESULT_VARIABLE     error_code
          OUTPUT_VARIABLE     ROSE_SCM_VERSION_UNIX_DATE
          OUTPUT_STRIP_TRAILING_WHITESPACE
      )

      if(NOT error_code EQUAL 0)
          message(
              FATAL_ERROR
              "(status=${error_code} "
              "Failed to determine the ROSE SCM version unix timestamp: "
              "${ROSE_SCM_VERSION_UNIX_DATE}${stderr}"
          )
      endif()
  endif()
else()
  # Distribution versions of ROSE will have a VERSION file.
  message(STATUS "Check for ROSE source tree type - Distribution")
  message(STATUS "Check for VERSION file - ${ROSE_SCM_VERSION_FILE}")
  if(NOT EXISTS "${ROSE_SCM_VERSION_FILE}")
      message(
          FATAL_ERROR
          "File not found: ${ROSE_SCM_VERSION_FILE}"
      )
  else()
      message(
          STATUS
          "Check for VERSION file - found "
          "${ROSE_SCM_VERSION_FILE}"
      )

      message(
          STATUS
          "Detecting ROSE SCM version information "
          "(from ${ROSE_SCM_VERSION_FILE})"
      )

      execute_process(
          COMMAND             "cat" ${ROSE_SCM_VERSION_FILE}
          WORKING_DIRECTORY   ${PROJECT_SOURCE_DIR}
          ERROR_VARIABLE      stderr
          RESULT_VARIABLE     error_code
          OUTPUT_VARIABLE     ROSE_SCM_VERSION_INFO
          OUTPUT_STRIP_TRAILING_WHITESPACE
      )
      if (NOT error_code EQUAL 0)
          message(
              FATAL_ERROR
              "(status=${error_code}) "
              "Failed to extract the ROSE SCM version information from "
              "${ROSE_SCM_VERSION_FILE}\n"
              "${ROSE_SCM_VERSION_INFO}${stderr}"
          )
      endif()
      # Convert string information to CMake LIST
      # (space-delimited to semicolon-delimited)
      string(
          REPLACE " " ";"             # <match> <replace>
          ROSE_SCM_VERSION_INFO_LIST  # output
          ${ROSE_SCM_VERSION_INFO}    # input
      )
      list(
          LENGTH
          ROSE_SCM_VERSION_INFO_LIST
          ROSE_SCM_VERSION_INFO_LIST_LENGTH
      )
      if(NOT ROSE_SCM_VERSION_INFO_LIST_LENGTH EQUAL ${ROSE_SCM_VERSION_FILE_FIELD_COUNT})
          message(
              FATAL_ERROR
              "Check for VERSION file - failed\n"
              "Invalid VERSION file ${ROSE_SCM_VERSION_FILE} has "
              "(${ROSE_SCM_VERSION_INFO_LIST_LENGTH}) fields, expected "
              "only (${ROSE_SCM_VERSION_FILE_FIELD_COUNT}) fields!"
          )
      else()
          list(GET ROSE_SCM_VERSION_INFO_LIST 0 ROSE_SCM_VERSION_ID)
          list(GET ROSE_SCM_VERSION_INFO_LIST 1 ROSE_SCM_VERSION_UNIX_DATE)
      endif()
  endif()
endif(IS_DIRECTORY ${PROJECT_SOURCE_DIR}/.git)

#---------------------------------------
# HEAD commit - id
#---------------------------------------
if (NOT "${ROSE_SCM_VERSION_ID}" MATCHES
        "${ROSE_SCM_VERSION_ID_REGEX}")
    message(
        FATAL_ERROR
        "Invalid Git hash \"${ROSE_SCM_VERSION_ID}\" does not "
        "match regex \"${ROSE_SCM_VERSION_ID_REGEX}\""
    )
endif()

string(LENGTH ${ROSE_SCM_VERSION_ID} ROSE_SCM_VERSION_ID_LENGTH)
if (NOT "${ROSE_SCM_VERSION_ID_LENGTH}" EQUAL
        "${ROSE_SCM_VERSION_ID_LENGTH_EXACT}")
    message(
        FATAL_ERROR
        "Invalid Git hash \"${ROSE_SCM_VERSION_ID}\" has length="
        "${ROSE_SCM_VERSION_ID_LENGTH}; expected length="
        "${ROSE_SCM_VERSION_ID_LENGTH_EXACT}"
    )
endif()

#---------------------------------------
# HEAD commit - date (Unix timestamp)
#---------------------------------------
if(NOT "${ROSE_SCM_VERSION_UNIX_DATE}" MATCHES
            "${ROSE_SCM_VERSION_UNIX_DATE_REGEX}")
    message(
        FATAL_ERROR
        "Invalid Git author date \"${ROSE_SCM_VERSION_UNIX_DATE}\" does not "
        "match regex \"${ROSE_SCM_VERSION_UNIX_DATE_REGEX}\"")
endif()

string(LENGTH ${ROSE_SCM_VERSION_UNIX_DATE} ROSE_SCM_VERSION_UNIX_DATE_LENGTH)
if ("${ROSE_SCM_VERSION_UNIX_DATE_LENGTH}" LESS
    "${ROSE_SCM_VERSION_UNIX_DATE_LENGTH_MIN}")
    message(
        FATAL_ERROR
        "Invalid Git Unix timestamp author date "
        "\"${ROSE_SCM_VERSION_UNIX_DATE}\" has length="
        "${ROSE_SCM_VERSION_UNIX_DATE_LENGTH}; "
        "expected length>=${ROSE_SCM_VERSION_UNIX_DATE_LENGTH_MIN}"
    )
endif()

message(STATUS "The ROSE SCM version identifier is ${ROSE_SCM_VERSION_ID}")
message(STATUS "The ROSE SCM version Unix timestamp is ${ROSE_SCM_VERSION_UNIX_DATE}")

#-------------------------------------------------------------------------------
# Boost C++ Libraries
#-------------------------------------------------------------------------------
set(Boost_USE_STATIC_LIBS TRUE)        
set(Boost_DEBUG TRUE)

# Honor BOOST_HOME environment variable
if (DEFINED ENV{BOOST_HOME})
  set (BOOST_ROOT "$ENV{BOOST_HOME}")
endif ()

option(Boost_USE_MULTITHREADED
  "Should Boost multithreaded libraries be used?" OFF)

if (WIN32)
FIND_PACKAGE(Boost REQUIRED) 
#set (Boost_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}/../../ )

#set (Boost_LIBRARY_DIRS ${Boost_INCLUDE_DIRS}/lib/ )
#set (Boost_LIBRARIES ${Boost_INCLUDE_DIRS}/lib/ )
set (BOOST_LIBRARYDIR ${Boost_LIBRARY_DIRS} )
set (BOOST_INCLUDEDIR ${Boost_INCLUDE_DIRS}/ )
#set (BOOST_ROOT ${Boost_INCLUDE_DIRS}/ )

MESSAGE("Boost information:") 
MESSAGE("  BOOST_ROOT: ${BOOST_ROOT}") 
MESSAGE("  Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}") 
MESSAGE("  Boost_LIBRARIES: ${Boost_LIBRARIES}") 
MESSAGE("  Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}") 
MESSAGE("  BOOST_LIBRARYDIR : ${BOOST_LIBRARYDIR}") 
MESSAGE("  BOOST_INCLUDEDIR : ${BOOST_INCLUDEDIR}") 


INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
endif (WIN32)

#Ensure that Boost is found
# environment variable BOOST_ROOT can help to find a user-specified path to Boost
SET(Boost_ADDITIONAL_VERSIONS "1.36.0" "1.37" "1.37.0" "1.38.0" "1.39.0" "1.40.0" )
find_package( Boost 1.36.0 COMPONENTS date_time filesystem program_options regex system wave thread REQUIRED)



if(NOT Boost_FOUND)
    message(FATAL_ERROR "Could not find Boost version 1.35.0 or newer command")
endif(NOT Boost_FOUND)
include_directories(${Boost_INCLUDE_DIRS})
add_definitions(-DBOOST_FILESYSTEM_VERSION=2)



# default paths to install header, executable, and libraries
set(INCLUDE_INSTALL_DIR "include")
set(BIN_INSTALL_DIR      "bin"   )   # The install dir for executables
set(LIB_INSTALL_DIR      "lib" )

set(INSTALL_TARGETS_DEFAULT_ARGS  RUNTIME DESTINATION "${BIN_INSTALL_DIR}"
                                      LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
                                                                        ARCHIVE
                                                                        DESTINATION
                                                                        "${LIB_INSTALL_DIR}"
                                                                        COMPONENT
                                                                        Devel
                                                                        )
# a new definition to tweak code for cmake 
add_definitions (-DUSE_CMAKE)


if (NOT WIN32)
enable_language(Fortran)
endif (NOT WIN32)

# tps (1/4/2010) Do not know how to add flag to CMakeSetup yet
option ( enable-smaller-generated-files "Build with smaller generated files for IR nodes" )
#set( enable-smaller-generated-files 1 )
if ( enable-smaller-generated-files )
add_definitions ("-DsmallerGeneratedFiles" "-DROSE_USE_SMALLER_GENERATED_FILES")
MESSAGE ( STATUS ">>>>>>>>>>>>>>>>>>>>>>>>>> Smaller generated files enabled  "  )
else ( enable-smaller-generated-files )
MESSAGE ( STATUS ">>>>>>>>>>>>>>>>>>>>>>>>> Smaller generated files disabled" )
endif ( enable-smaller-generated-files )


#/CLR  /MDd /TP 
if (WIN32)
# /TP to indicate files are C++
# / CLR common intermediate language
# /GL whole program optimization
# /O1 optimization for small files
# /Ob0 disable inline expansion
# /MP multiple processors compilation
# /0s small files
set( CMAKE_BUILD_TYPE Release )
set( CMAKE_CXX_FLAGS " /TP /MP /O1 /Os /GR /EHsc /wd4541 " )
set( CMAKE_C_FLAGS "   /wd4541 " )
set( CMAKE_SHARED_LINKER_FLAGS_DEBUG " ${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /INCREMENTAL:NO" )
set( CMAKE_SHARED_LINKER_FLAGS_RELEASE " ${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /INCREMENTAL:NO" )
set( CMAKE_MODULE_LINKER_FLAGS_DEBUG " ${CMAKE_MODULE_LINKER_FLAGS_DEBUG} /INCREMENTAL:NO" )
set( CMAKE_MODULE_LINKER_FLAGS_RELEASE " ${CMAKE_MODULE_LINKER_FLAGS_RELEASE} /INCREMENTAL:NO" )
set( CMAKE_LINKER_FLAGS " ${CMAKE_LINKER_FLAGS} /INCREMENTAL:NO" )
set( CMAKE_SHARED_LINKER_FLAGS " ${CMAKE_SHARED_LINKER_FLAGS} /INCREMENTAL:NO" )
set( CMAKE_MODULE_LINKER_FLAGS " ${CMAKE_MODULE_LINKER_FLAGS} /INCREMENTAL:NO" )
set( CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} /INCREMENTAL:NO")
set( CMAKE_C_LINK_FLAGS "${CMAKE_C_LINK_FLAGS} /INCREMENTAL:NO")
else (WIN32)
set( CMAKE_CXX_FLAGS " -fexceptions -DHAVE_CONFIG_H -fPIC" )
set( CMAKE_C_FLAGS " -fexceptions -DHAVE_CONFIG_H -fPIC" )
#set( CMAKE_CXX_FLAGS " -fexceptions -DHAVE_CONFIG_H -g -fPIC -fvisibility=hidden" )
#set( CMAKE_C_FLAGS " -fexceptions -DHAVE_CONFIG_H -g -fPIC -fvisibility=hidden" )
#set( CMAKE_CXX_FLAGS " -fexceptions -DHAVE_CONFIG_H -g -fPIC " )
#set( CMAKE_C_FLAGS " -fexceptions -DHAVE_CONFIG_H -g -fPIC " )
endif (WIN32)


#set(  CMAKE_VERBOSE_MAKEFILE  true )

set(ROSE_TOP_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(ROSE_TOP_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

#Generate our configure file

# --------- please do all system, platform, software tests here --------------

# Setup EDG version
set(EDG_VERSION "3.3" CACHE STRING
  "major.minor version number for EDG (e.g. 3.3, 3.10, 4.0, 4.1).")
string(SUBSTRING ${EDG_VERSION} 0 1 EDG_MAJOR_VERSION)
string(SUBSTRING ${EDG_VERSION} 2 1 EDG_MINOR_VERSION)
add_definitions(-DROSE_EDG_MAJOR_VERSION_NUMBER=${EDG_MAJOR_VERSION})
add_definitions(-DROSE_EDG_MINOR_VERSION_NUMBER=${EDG_MINOR_VERSION})

# Check if we need to download the EDG binary tarball
if(EXISTS "${PROJECT_SOURCE_DIR}/src/frontend/CxxFrontend/EDG/CMakeLists.txt")
  set(have_EDG_source TRUE)
else()
  set(have_EDG_source FALSE)
  add_definitions(-DBINARY_EDG)
  if (NOT WIN32)
  # no need to display this message after the file has been downloaded
  if(NOT EXISTS "${PROJECT_BINARY_DIR}/src/frontend/CxxFrontend/EDG/.libs")
    message(WARNING
      "At build time, CMake will attempt to download a required library "
      "tarball.\n"
      "Please note that this EDG binary tarball is currently only available "
      "for the GCC compiler.\n" )
  endif()
  include(${PROJECT_SOURCE_DIR}/cmake/DownloadEDG.cmake)
  endif()
endif()

# Set ROSE_SHLIBPATH_VAR.  For now, it just has the same value as
# LD_LIBRARY_PATH.
set(ROSE_SHLIBPATH_VAR $ENV{LD_LIBRARY_PATH})

# a set of common features: including endian, stdio.h, printf, size of long int ,etc
#taken from LyX
include(ConfigureChecks)
#include(KDE4Defaults)
#  a collection of macros which extend the built-in cmake commands
include(MacroLibrary)
# Try to find MySQL / MySQL Embedded library
# MYSQL_FOUND,MYSQL_INCLUDE_DIR,MYSQL_LIBRARIES, etc
include(FindMySQL)

find_package(OpenSSL)

# Language configuration options

option(enable-c "Enable C/C++ language support in ROSE." ON)
if (enable-c)
  add_definitions(-DROSE_BUILD_CXX_LANGUAGE_SUPPORT)
  add_definitions(-DROSE_BUILD_C_LANGUAGE_SUPPORT)
endif()

option(enable-cuda "Enable cuda support in ROSE." ON)
if (enable-cuda)
  find_package(CUDA REQUIRED)
  add_definitions(-DROSE_BUILD_CUDA_LANGUAGE_SUPPORT)
endif()

option(enable-fortran "Enable Fortran support in ROSE." ON)
if (enable-fortran)
  add_definitions(-DROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT)
endif()

option(enable-java "Enable Java support in ROSE." ON)
if (enable-java)
  message(Status "Looking for JAVA ...")
  include(FindJava)
  find_program(GCJ gcj)
  find_program(GCJH gcjh)
  find_package(JNI)
  add_definitions(-DROSE_BUILD_JAVA_LANGUAGE_SUPPORT)
endif()

option(enable-opencl "Enable OpenCL support in ROSE." ON)
if (enable-opencl)
  add_definitions(-DROSE_BUILD_OPENCL_LANGUAGE_SUPPORT)
  find_path(with-opencl-inc NAMES cl.h
    DOC "For OpenCL runtime library")
  find_library(with-opencl-lib OpenCL DOC "OpenCL library for runtime examples")
endif()

option(enable-php "Enable PHP support in ROSE." ON)
if (enable-php)
  add_definitions(-DROSE_BUILD_PHP_LANGUAGE_SUPPORT)
  find_path(with-php php
    DOC "Specify the prefix where PHP (and phc) is installed")
endif()

option(enable-python "Enable Python support in ROSE." OFF)
if (enable-python)
  add_definitions(-DROSE_BUILD_PYTHON_LANGUAGE_SUPPORT)
  find_package(PythonLibs REQUIRED)
  option(with-python "Build code that requires a Python interpreter" ON)
  if(with-python)
    find_package(PythonInterp)
  endif()
endif()

# Other configuration options

option(disable-binary-analysis-tests
  "Disable tests of ROSE binary analysis code" ON)

option(disable-tests-directory
  "Disable compilation and testing of the ROSE/tests directory" OFF)

option(disable-tutorial-directory
  "Disable compilation and testing of the ROSE/tutorial directory" OFF)

option(disable-projects-directory
  "Disable compilation and testing of the ROSE/projects directory" OFF)

option(enable-advanced-warnings
  "Support for an advanced uniform warning level for ROSE development" OFF)
if (enable-advanced-warnings)
  message(WARNING
    "Using an advanced uniform warning level for ROSE development.")
  add_definitions(-DROSE_USE_UNIFORM_ADVANCED_WARNINGS_SUPPORT)
endif()

option(enable-assembly-semantics
  "Enable semantics-based analysis of assembly code" OFF)

option(enable-binary-analysis "Enable binary analysis support in ROSE." ON)
if (enable-binary-analysis)
  add_definitions(-DROSE_BUILD_BINARY_ANALYSIS_SUPPORT)
endif()

option(enable-candl "Support for ScopLib" OFF)
if(enable-candl)
  find_path(with-candl "include/candl.h"
    DOC "Path to a valid Candl installation")
endif()

option(enable-cloog "Support for Cloog" OFF)
if(enable-cloog)
  find_path(with-cloog "include/cloog.h"
    DOC "Path to a valid Cloog installation")
endif()

option(enable-compass2 "build the Compass2 static analysis tool" OFF)

option(enable-edg-cuda "Build EDG 4.0 with CUDA support" OFF)

option(enable-edg-opencl "Build EDG 4.0 with OpenCL support" OFF)

option(enable-edg_union_struct_debugging
  "Should EDG Union/Struct debugging support be used?" OFF)
if(enable-edg_union_struct_debugging)
  set(USE_ROSE_EDG_DEBUGGING_SUPPORT 1)
endif()

option(enable-FLTK "Enable FLTK")
if(enable-FLTK)
  find_package(FLTK REQUIRED)
endif()

option(enable-gnu-extensions
  "Enable internal support in ROSE for GNU language extensions" OFF)
if(enable-gnu-extensions)
  add_definitions(-DROSE_SUPPORT_GNU_EXTENSIONS="TRUE")
endif()

option(enable-internalFrontendDevelopment
  "Enable development mode to reduce files required to support work on
  language frontends" OFF)
if (enable-internalFrontendDevelopment)
  add_definitions(-DROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT)
endif()

option(enable-microsoft-extensions
  "Enable internal support in ROSE for GNU language extensions" OFF)
if(enable-microsoft-extensions)
  add_definitions(-DROSE_SUPPORT_MICROSOFT_EXTENSIONS="TRUE")
endif()

set(enable-ofp-version "0.8.3" CACHE STRING "version number for OFP")

option(enable-ppl "Support for Parma Polyhedral Library" OFF)
if(enable-ppl)
  find_path(with-ppl "include/ppl.h"
    DOC "Path to Parma Polyhedral Library installation")
  find_library(libppl ppl)
endif()

option(enable-purify-api "Enable purify API in code" OFF)
if(enable-purify-api)
  set(USE_PURIFY_API 1)
endif()

option(enable-purify-linker "Augment the linker with purify." OFF)
if(enable-purify-linker)
  set(USE_PURIFY 1)
  set(USE_PURIFY_LINKER 1)
endif()

option(enable-rosehpct "enable build of the ROSE-HPCT module" ON)

option(enable-rose-openGL "enable openGL" OFF)
if(enable-rose-openGL)
  find_package(OpenGL)
  find_package(GLUT)
endif()

option(enable-scoplib "Support for ScopLib" OFF)
if(enable-scoplib)
  find_path(with-scoplib "include/scolib.h"
    DOC "Path to a valid ScopLib installation")
endif()

option(enable-yices "Use the Yices Satisfiability Modulo Theories (SMT)")
if(enable-yices)
  find_path(with-yices bin/yices DOC
    "The prefix used to install Yices.
     An executable named /bin/yices should be under the specified prefix.")
endif()

set(with-alternate_backend_Cxx_compiler "" CACHE STRING
  "Specify an alternative C++ back-end compiler")
if(with-alternate_backend_Cxx_compiler)
  if(EXISTS ${with-alternate_backend_Cxx_compiler})
    set(BACKEND_CXX_COMPILER ${with-alternate_backend_Cxx_compiler})
  else()
    message(WARNING
      "${with-alternate_backend_Cxx_compiler} does not exist.
       Specify a full, absolute path to the compiler.")
  endif()
endif()

set(with-alternate_backend_C_compiler "" CACHE STRING
  "Specify an alternative C back-end compiler")
if(with-alternate_backend_C_compiler)
  if(EXISTS ${with-alternate_backend_C_compiler})
    set(BACKEND_C_COMPILER ${with-alternate_backend_C_compiler})
    message(STATUS "just set backend C compiler to ${BACKEND_C_COMPILER}")
  else()
    message(WARNING
      "${with-alternate_backend_C_compiler} does not exist.
       Specify a full, absolute path to the compiler.")
  endif()
endif()

set(with-alternate_backend_fortran_compiler "" CACHE STRING
  "Specify an alternative fortran back-end compiler")
if(with-alternate_backend_fortran_compiler)
  if(EXISTS ${with-alternate_backend_fortran_compiler})
    set(BACKEND_FORTRAN_COMPILER ${with-alternate_backend_fortran_compiler})
  else()
    message(WARNING
      "${with-alternate_backend_fortran_compiler} does not exist.
       Specify a full, absolute path to the compiler.")
  endif()
endif()

set(with-alternate_backend_java_compiler "" CACHE STRING
  "Specify an alternative java back-end compiler")
if(with-alternate_backend_java_compiler)
  if(EXISTS ${with-alternate_backend_java_compiler})
  set(BACKEND_JAVA_COMPILER ${with-alternate_backend_java_compiler})
  else()
    message(WARNING
      "${with-alternate_backend_java_compiler} does not exist.
       Specify a full, absolute path to the compiler.")
  endif()
endif()

find_path(with-backstroke-ross ROSS
  DOC "Specify the path where ROSS is installed")

find_path(with-backstroke-speedes SPEEDES
  DOC "Specify the path where SPEEDES is installed")

find_library(with-dwarf dwarf
  DOC "Specify the path where libdwarf is installed")

find_library(with-gomp_omp_runtime_library gomp_omp
  DOC "Specify the prefix where GOMP Runtime System is installed")

find_path(with-GraphViz_include graphviz.h
  DOC "Specify the prefix where GraphViz include files are installed")

find_path(with-GraphViz_libs GraphViz_libs
  DOC "Specify the prefix where GraphViz libraries are installed")

find_path(with-haskell runghc
  DOC "Path to bin directory containing ghc and runghc.")

find_path(with-ida "ida2sql.py"
  DOC "Specify the prefix where IDA Pro is installed")

find_path(with-insure insure++
  DOC "Specify the prefix where insure++ is installed")

find_path(with-IntelPin include/IntelPinSupport.h
  DOC "Specify the prefix where Intel Pin Package is installed")

find_path(with-ltdl-include ltdl.h
  DOC "use the ltdl headers installed in DIR")

find_library(with-ltdl-lib ltdl DOC "Path to ltdl library")

find_path(with-llvm "llvm"
  DOC "Specify the prefix where LLVM (and opt) is installed")

find_path(with-maple "include/maple.h"
  DOC "Specify the prefix where Maple is installed")

find_path(with-omni_omp_runtime_support include/omni_omp.h
  DOC "Specify the prefix where Omni OpenMP Runtime System is installed")

find_path(with-purify bin/purify
  DOC "Specify the prefix where purify is installed")
if(with-purify)
  set(USE_PURIFY 1)
endif()

find_path(with-QRose QRose DOC "prefix of QRose installation")

option(with-roseQt "Build with roseQt" OFF)

find_path(with-rted rted DOC "Configure option to have RTED enabled.")

find_path(with-smt-solver smt-solver
  DOC "Specify the path to an SMT-LIB compatible SMT solver.
       Used only for testing.")

find_path(with-wine "bin/wine" DOC "Specify the prefix where Wine is installed")

# tps (01/04/2010) : added verbose output 
# ------------------------- Begin Generic CMake Variable Logging ------------------

# if you are building in-source, this is the same as CMAKE_SOURCE_DIR, otherwise 
# this is the top level directory of your build tree 
MESSAGE( STATUS "CMAKE_BINARY_DIR:         " ${CMAKE_BINARY_DIR} )

# if you are building in-source, this is the same as CMAKE_CURRENT_SOURCE_DIR, otherwise this 
# is the directory where the compiled or generated files from the current CMakeLists.txt will go to 
MESSAGE( STATUS "CMAKE_CURRENT_BINARY_DIR: " ${CMAKE_CURRENT_BINARY_DIR} )

# this is the directory, from which cmake was started, i.e. the top level source directory 
MESSAGE( STATUS "CMAKE_SOURCE_DIR:         " ${CMAKE_SOURCE_DIR} )

# this is the directory where the currently processed CMakeLists.txt is located in 
MESSAGE( STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR} )

# contains the full path to the top level directory of your build tree 
MESSAGE( STATUS "PROJECT_BINARY_DIR: " ${PROJECT_BINARY_DIR} )

# contains the full path to the root of your project source directory,
# i.e. to the nearest directory where CMakeLists.txt contains the PROJECT() command 
MESSAGE( STATUS "PROJECT_SOURCE_DIR: " ${PROJECT_SOURCE_DIR} )

# set this variable to specify a common place where CMake should put all executable files
# (instead of CMAKE_CURRENT_BINARY_DIR)
MESSAGE( STATUS "EXECUTABLE_OUTPUT_PATH: " ${EXECUTABLE_OUTPUT_PATH} )

# set this variable to specify a common place where CMake should put all libraries 
# (instead of CMAKE_CURRENT_BINARY_DIR)
MESSAGE( STATUS "LIBRARY_OUTPUT_PATH:     " ${LIBRARY_OUTPUT_PATH} )

# tell CMake to search first in directories listed in CMAKE_MODULE_PATH
# when you use FIND_PACKAGE() or INCLUDE()
MESSAGE( STATUS "CMAKE_MODULE_PATH: " ${CMAKE_MODULE_PATH} )

# this is the complete path of the cmake which runs currently (e.g. /usr/local/bin/cmake) 
MESSAGE( STATUS "CMAKE_COMMAND: " ${CMAKE_COMMAND} )

# this is the CMake installation directory 
MESSAGE( STATUS "CMAKE_ROOT: " ${CMAKE_ROOT} )

# this is the filename including the complete path of the file where this variable is used. 
MESSAGE( STATUS "CMAKE_CURRENT_LIST_FILE: " ${CMAKE_CURRENT_LIST_FILE} )

# this is linenumber where the variable is used
MESSAGE( STATUS "CMAKE_CURRENT_LIST_LINE: " ${CMAKE_CURRENT_LIST_LINE} )

# this is used when searching for include files e.g. using the FIND_PATH() command.
MESSAGE( STATUS "CMAKE_INCLUDE_PATH: " ${CMAKE_INCLUDE_PATH} )

# this is used when searching for libraries e.g. using the FIND_LIBRARY() command.
MESSAGE( STATUS "CMAKE_LIBRARY_PATH: " ${CMAKE_LIBRARY_PATH} )

# the complete system name, e.g. "Linux-2.4.22", "FreeBSD-5.4-RELEASE" or "Windows 5.1" 
MESSAGE( STATUS "CMAKE_SYSTEM: " ${CMAKE_SYSTEM} )

# the short system name, e.g. "Linux", "FreeBSD" or "Windows"
MESSAGE( STATUS "CMAKE_SYSTEM_NAME: " ${CMAKE_SYSTEM_NAME} )

# only the version part of CMAKE_SYSTEM 
MESSAGE( STATUS "CMAKE_SYSTEM_VERSION: " ${CMAKE_SYSTEM_VERSION} )

# the processor name (e.g. "Intel(R) Pentium(R) M processor 2.00GHz") 
MESSAGE( STATUS "CMAKE_SYSTEM_PROCESSOR: " ${CMAKE_SYSTEM_PROCESSOR} )

# is TRUE on all UNIX-like OS's, including Apple OS X and CygWin
MESSAGE( STATUS "UNIX: " ${UNIX} )

# is TRUE on Windows, including CygWin 
MESSAGE( STATUS "WIN32: " ${WIN32} )

# is TRUE on Apple OS X
MESSAGE( STATUS "APPLE: " ${APPLE} )

# is TRUE when using the MinGW compiler in Windows
MESSAGE( STATUS "MINGW: " ${MINGW} )

# is TRUE on Windows when using the CygWin version of cmake
MESSAGE( STATUS "CYGWIN: " ${CYGWIN} )

# is TRUE on Windows when using a Borland compiler 
MESSAGE( STATUS "BORLAND: " ${BORLAND} )

# Microsoft compiler 
MESSAGE( STATUS "MSVC: " ${MSVC} )
MESSAGE( STATUS "MSVC_IDE: " ${MSVC_IDE} )
MESSAGE( STATUS "MSVC60: " ${MSVC60} )
MESSAGE( STATUS "MSVC70: " ${MSVC70} )
MESSAGE( STATUS "MSVC71: " ${MSVC71} )
MESSAGE( STATUS "MSVC80: " ${MSVC80} )
MESSAGE( STATUS "CMAKE_COMPILER_2005: " ${CMAKE_COMPILER_2005} )




# set this to true if you don't want to rebuild the object files if the rules have changed, 
# but not the actual source files or headers (e.g. if you changed the some compiler switches) 
MESSAGE( STATUS "CMAKE_SKIP_RULE_DEPENDENCY: " ${CMAKE_SKIP_RULE_DEPENDENCY} )

# since CMake 2.1 the install rule depends on all, i.e. everything will be built before installing. 
# If you don't like this, set this one to true.
MESSAGE( STATUS "CMAKE_SKIP_INSTALL_ALL_DEPENDENCY: " ${CMAKE_SKIP_INSTALL_ALL_DEPENDENCY} )

# If set, runtime paths are not added when using shared libraries. Default it is set to OFF
MESSAGE( STATUS "CMAKE_SKIP_RPATH: " ${CMAKE_SKIP_RPATH} )

# set this to true if you are using makefiles and want to see the full compile and link 
# commands instead of only the shortened ones 
MESSAGE( STATUS "CMAKE_VERBOSE_MAKEFILE: " ${CMAKE_VERBOSE_MAKEFILE} )

# this will cause CMake to not put in the rules that re-run CMake. This might be useful if 
# you want to use the generated build files on another machine. 
MESSAGE( STATUS "CMAKE_SUPPRESS_REGENERATION: " ${CMAKE_SUPPRESS_REGENERATION} )


# A simple way to get switches to the compiler is to use ADD_DEFINITIONS(). 
# But there are also two variables exactly for this purpose: 

# the compiler flags for compiling C sources 
MESSAGE( STATUS "CMAKE_C_FLAGS: " ${CMAKE_C_FLAGS} )

# the compiler flags for compiling C++ sources 
MESSAGE( STATUS "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS} )


# Choose the type of build.  Example: SET(CMAKE_BUILD_TYPE Debug) 
MESSAGE( STATUS "CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE} )

# if this is set to ON, then all libraries are built as shared libraries by default.
MESSAGE( STATUS "BUILD_SHARED_LIBS: " ${BUILD_SHARED_LIBS} )

# the compiler used for C files 
MESSAGE( STATUS "CMAKE_C_COMPILER: " ${CMAKE_C_COMPILER} )

# the compiler used for C++ files 
MESSAGE( STATUS "CMAKE_CXX_COMPILER: " ${CMAKE_CXX_COMPILER} )

# if the compiler is a variant of gcc, this should be set to 1 
MESSAGE( STATUS "CMAKE_COMPILER_IS_GNUCC: " ${CMAKE_COMPILER_IS_GNUCC} )

# if the compiler is a variant of g++, this should be set to 1 
MESSAGE( STATUS "CMAKE_COMPILER_IS_GNUCXX : " ${CMAKE_COMPILER_IS_GNUCXX} )

# the tools for creating libraries 
MESSAGE( STATUS "CMAKE_AR: " ${CMAKE_AR} )
MESSAGE( STATUS "CMAKE_RANLIB: " ${CMAKE_RANLIB} )


# ------------------------- End of Generic CMake Variable Logging ------------------







if (NOT WIN32)
include(FindOpenSSL)
include(FindSQLITE3)
endif (NOT WIN32)

message(Status "Looking for flex ...")
find_package(FLEX)
if(NOT FLEX_FOUND)
    message(FATAL_ERROR "Could not find flex command")
endif(NOT FLEX_FOUND)

message(Status "Looking for BISON ...")
include(FindBison)
FIND_BISON()
if(NOT BISON_FOUND)
    message(FATAL_ERROR "Could not find BISON command")
endif(NOT BISON_FOUND)

# include(ConvenienceLibs) # Not in use



if (NOT WIN32)
find_library(M_LIB m)
find_library(RT_LIB rt)
endif (NOT WIN32)

# define a global variable to collect all common linked third-party libraries for rose
if (NOT WIN32)
set( link_with_libraries ${Boost_LIBRARIES} ${M_LIB} ${RT_LIB} ${OPENSSL_LIBRARIES} )
else (NOT WIN32)
set( link_with_libraries ${Boost_LIBRARIES}  shlwapi.lib )
endif (NOT WIN32)

# Check compilers and version numbers, Liao 11/25/2009
# the module is located in src/cmake/modules
include(roseChooseBackendCompiler)
include(roseGenerateBackendCompilerSpecificHeaders)

# --------- finish all fundamental tests, now set derivative variables
# Liao 12/1/2009, This is essential to find the right include path from either build or installation tree
# for a translator
message(Status "Running tests ...")
if (HAVE_DLFCN_H)
  if (HAVE_DLADDR)
    set (use_rose_in_build_tree_var TRUE)
# this following line won't work since it only set the environment variable for cmake's session
# not for ctest session. Still no good way to set it within cmake
# fortunately,   
    set ($ENV{ROSE_IN_BUILD_TREE} ${ROSE_TOP_BINARY_DIR})
  endif (HAVE_DLADDR)
else (HAVE_DLFCN_H)
  set (use_rose_in_build_tree_var, FALSE)
endif (HAVE_DLFCN_H)

 set( LIBHARU_SOURCE_DIR ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/libharu-200910170404 )
 set( LIBHARU_BINARY_DIR ${ROSE_TOP_BINARY_DIR}/src/3rdPartyLibraries/libharu-200910170404 )
 set( ROSE_INCLUDES 
     ${ROSE_TOP_BINARY_DIR}
     ${ROSE_TOP_BINARY_DIR}/src/frontend/SageIII/
     ${ROSE_TOP_BINARY_DIR}/src/frontend/SageIII/astFileIO
     ${ROSE_TOP_SRC_DIR}
     ${ROSE_TOP_SRC_DIR}/src/frontend/SageIII 
     ${ROSE_TOP_SRC_DIR}/src/ROSETTA/src/
     ${ROSE_TOP_SRC_DIR}/src 
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
     ${ROSE_TOP_SRC_DIR}/src/frontend/BinaryDisassembly 
     ${ROSE_TOP_SRC_DIR}/src/frontend/Disassemblers 
     ${ROSE_TOP_SRC_DIR}/src/frontend/ExecFormats 
     ${ROSE_TOP_SRC_DIR}/src/backend/unparser 
     ${ROSE_TOP_SRC_DIR}/src/backend/unparser/formatSupport 
     ${ROSE_TOP_SRC_DIR}/src/backend/unparser/languageIndependenceSupport 
     ${ROSE_TOP_SRC_DIR}/src/backend/unparser/CxxCodeGeneration 
     ${ROSE_TOP_SRC_DIR}/src/backend/unparser/FortranCodeGeneration 
     ${ROSE_TOP_SRC_DIR}/src/backend/unparser/JavaCodeGeneration 
     ${ROSE_TOP_SRC_DIR}/src/backend/unparser/PHPCodeGeneration 
     ${ROSE_TOP_SRC_DIR}/src/backend/unparser/PythonCodeGeneration 
     ${ROSE_TOP_SRC_DIR}/src/backend/asmUnparser 
     ${ROSE_TOP_SRC_DIR}/src/util 
     ${ROSE_TOP_SRC_DIR}/src/util/support 
     ${ROSE_TOP_SRC_DIR}/src/util/graphs  
     ${ROSE_TOP_SRC_DIR}/src/util/stringSupport 
     ${ROSE_TOP_SRC_DIR}/src/util/commandlineProcessing 
     ${ROSE_TOP_SRC_DIR}/src/midend/abstractHandle 
     ${ROSE_TOP_SRC_DIR}/src/midend/abstractMemoryObject
     ${ROSE_TOP_SRC_DIR}/src/midend/astDiagnostics 
     ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/astInlining 
     ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/astOutlining  
     ${ROSE_TOP_SRC_DIR}/src/midend/astProcessing 
     ${ROSE_TOP_SRC_DIR}/src/midend/astQuery 
     ${ROSE_TOP_SRC_DIR}/src/midend/astRewriteMechanism 
     ${ROSE_TOP_SRC_DIR}/src/midend/astUtil/annotation 
     ${ROSE_TOP_SRC_DIR}/src/midend/astUtil/astInterface 
     ${ROSE_TOP_SRC_DIR}/src/midend/astUtil/astSupport 
     ${ROSE_TOP_SRC_DIR}/src/midend/astUtil/symbolicVal 
     ${ROSE_TOP_SRC_DIR}/src/midend/binaryAnalyses 
     ${ROSE_TOP_SRC_DIR}/src/midend/binaryAnalyses/dataflowanalyses 
     ${ROSE_TOP_SRC_DIR}/src/midend/binaryAnalyses/graph 
     ${ROSE_TOP_SRC_DIR}/src/midend/binaryAnalyses/instructionSemantics 
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
     ${ROSE_TOP_SRC_DIR}/src/midend/programAnalysis/graphAnalysis 
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
     ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/libharu-2.1.0/include 
     ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/libharu-2.1.0/include 
     ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/qrose/Framework 
     ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/qrose/Widgets  
     ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/POET
     ${ROSE_TOP_SRC_DIR}/projects/dataBase 
     ${ROSE_TOP_SRC_DIR}/src/util/graphs 
     ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/libharu-2.1.0/win32/include/
     ${ROSE_TOP_SRC_DIR}/src/midend/astUtil/astInterface )

IF(WIN32) 
set( ROSE_INCLUDES ${ROSE_INCLUDES}
     ${ROSE_TOP_SRC_DIR}/winspecific )
ENDIF()


include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )
#include_directories( ${ROSE_INCLUDES} )


if(with-roseQt)
  find_package(Qt4 REQUIRED)
  include(${QT_USE_FILE})
  #message(Status "Enabling QT ...")
  list(APPEND ROSE_INCLUDES
    ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/qrose/Components/Common
    ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/qrose/Components/QueryBox
    ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/qrose/Components/SourceBox
    ${ROSE_TOP_SRC_DIR}/src/3rdPartyLibraries/qrose/Components/TreeBox
  )
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
    ${ROSE_SOURCE_DIR}/src/roseExtensions/qtWidgets/WidgetCreator
  )
endif()
set(QT_INCLUDES ${ROSE_INCLUDES} ${Boost_INCLUDE_DIRS} )

# Set default output paths for libraries and directories.
# This causes all the libraries to be built into a single lib directory,
# and all the executables to be built into a single bin directory.
# If these variables are not set, then by default the targets will be
# littered throughout the binary directory, matching the structure of
# the source directory.
#
# Organizing the output of the build directory this way is important
# when building on Windows.  By default, Windows .exes do not know where
# to find their required .dlls.  The easiest way to overcome this problem
# is by placing the .exe and all its dependent .dlls in the same directory.
# We accomplish this with CMake by setting CMAKE_RUNTIME_OUTPUT_DIRECTORY.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(config)
add_subdirectory(src)
add_subdirectory(demo)
add_subdirectory(exampleTranslators)

if(NOT disable-tests-directory)
  add_subdirectory(tests)
endif()

if(NOT disable-tutorial-directory)
  add_subdirectory(tutorial)
endif()

if(NOT disable-projects-directory)
  add_subdirectory(projects)
endif()

enable_testing()

# Copy CTestCustom.cmake into the binary directory.
# This file tells CTest to ignore some output that it otherwise
# considers compilation errors.
configure_file(
  cmake/CTestCustom.cmake.in
  ${CMAKE_BINARY_DIR}/CTestCustom.cmake
  @ONLY)

#TODO: better support for other compilers & operating systems
set(C_INCLUDE_STRING
  "{\"${BACKEND_C_COMPILER_NAME_WITHOUT_PATH}_HEADERS\", \\
  \"${BACKEND_C_COMPILER_NAME_WITHOUT_PATH}_HEADERS/hdrs\",  \\
  \"${BACKEND_C_COMPILER_NAME_WITHOUT_PATH}_HEADERS/hdrs1\", \\
  \"${BACKEND_C_COMPILER_NAME_WITHOUT_PATH}_HEADERS/hdrs2\", \\
  \"${BACKEND_C_COMPILER_NAME_WITHOUT_PATH}_HEADERS/hdrs3\", \\
  \"${BACKEND_C_COMPILER_NAME_WITHOUT_PATH}_HEADERS/hdrs4\", \\
  \"${BACKEND_C_COMPILER_NAME_WITHOUT_PATH}_HEADERS/hdrs5\", \\
  \"${BACKEND_C_COMPILER_NAME_WITHOUT_PATH}_HEADERS/hdrs6\", \\
  \"${BACKEND_C_COMPILER_NAME_WITHOUT_PATH}_HEADERS/hdrs7\"")
set(CXX_INCLUDE_STRING
  "{\"${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH}_HEADERS\", \\
  \"${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH}_HEADERS/hdrs\",  \\
  \"${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH}_HEADERS/hdrs1\", \\
  \"${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH}_HEADERS/hdrs2\", \\
  \"${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH}_HEADERS/hdrs3\", \\
  \"${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH}_HEADERS/hdrs4\", \\
  \"${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH}_HEADERS/hdrs5\", \\
  \"${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH}_HEADERS/hdrs6\", \\
  \"${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH}_HEADERS/hdrs7\"")
find_program(shell sh)
if(shell AND CMAKE_COMPILER_IS_GNUCXX)
  execute_process(
    COMMAND ${shell} ${CMAKE_SOURCE_DIR}/config/get_compiler_header_dirs gcc
    OUTPUT_VARIABLE C_backend_includes
  )
  execute_process(
    COMMAND ${shell} ${CMAKE_SOURCE_DIR}/config/get_compiler_header_dirs g++
    OUTPUT_VARIABLE CXX_backend_includes
  )
  string(REPLACE "\n" ";" C_backend_includes "${C_backend_includes}")
  string(REPLACE "\n" ";" CXX_backend_includes "${CXX_backend_includes}")

  foreach(C_include_dir ${C_backend_includes})
    string(STRIP "${C_include_dir}" C_include_dir)
    set(C_INCLUDE_STRING "${C_INCLUDE_STRING}, \"${C_include_dir}\"")
  endforeach()

  foreach(CXX_include_dir ${CXX_backend_includes})
    string(STRIP "${CXX_include_dir}" CXX_include_dir)
    set(CXX_INCLUDE_STRING "${CXX_INCLUDE_STRING}, \"${CXX_include_dir}\"")
  endforeach()

  # In the Autools build system, ROSE stores compiler specific headers in two
  # distinct locations:
  #    In the build tree, the directory is named $ROSE_BUILD/include-staging
  #    In the installation tree, the directory is named $ROSE_INSTALL/include
  # During make-install, the content of $ROSE_BUILD/include-staging is simply
  # copied over into $ROSE_INSTALL/include.
  #
  # If a ROSE translator is detected as being run from the build tree, then the
  # associated header file include paths are added to the translator's command
  # line, e.g. <translator> -I$ROSE_BUILD/include-staging/<path>. Similarly, if
  # the ROSE translator is detected as being run from the install tree, the
  # command line will contain the associated paths in the installation
  # location, e.g. -I$ROSE_INSTALL/include/<path>
  #
  # The setup of CMake for ROSE, on the other hand, does not make this
  # distinction. Instead, the CMake system utilizes a single directory
  # $ROSE_CMAKE_BIN/include for the same purpose.
  execute_process(
    COMMAND ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_BINARY_DIR}/include-staging ${CMAKE_BINARY_DIR}/include
  )
endif()
set(C_INCLUDE_STRING "${C_INCLUDE_STRING}}")
set(CXX_INCLUDE_STRING "${CXX_INCLUDE_STRING}}")

# ------------------------- Private Configuration File ----------------------------
# The rose_config.h file contains CPP macros describing what features were detected
# during the configuration phase.  This file should not be installed and should not
# be #include'd into user-level code because it pollutes the global name space.
# This needs to be here at the end in order to catch all the variables defined above
# such as compiler names, versions, etc.
configure_file(${ROSE_TOP_SRC_DIR}/rose_config.h.in.cmake ${ROSE_TOP_BINARY_DIR}/rose_config.h)

# ------------------------- Public Configuration File -----------------------------
# The rosePublicConfig.h header is generated from rose_config.h by copying certain
# CPP macros from rose_config.h to rosePublicConfig.h and changing their names so
# as not to pollute the global name space.  See scripts/publicConfiguration.pl for
# details.





########### install files ###############

#install(FILES  rose_config.h DESTINATION ${INCLUDE_INSTALL_DIR} )



message(Status "Finished ...")
