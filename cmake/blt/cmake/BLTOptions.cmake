# Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
# other BLT Project Developers. See the top-level LICENSE file for details
# 
# SPDX-License-Identifier: (BSD-3-Clause)
#------------------------------------------------------------------------------
# Sets up configuration options for BLT
#------------------------------------------------------------------------------

include(CMakeDependentOption)

#------------------------------------------------------------------------------
# Build Targets
#------------------------------------------------------------------------------
option(ENABLE_EXAMPLES   "Enables examples" OFF)
option(ENABLE_TESTS      "Enables tests" OFF)
option(ENABLE_BENCHMARKS "Enables benchmarks" OFF)
option(ENABLE_COVERAGE   "Enables code coverage support" OFF)

#------------------------------------------------------------------------------
# TPL Executable Options
#------------------------------------------------------------------------------
option(ENABLE_GIT          "Enables Git support" ON)

# Documentation
option(ENABLE_DOXYGEN      "Enables Doxygen support" OFF)
option(ENABLE_SPHINX       "Enables Sphinx support" OFF)
option(ENABLE_DOCS       "Enables documentation" OFF)

# Quality & Style
option(ENABLE_CLANGQUERY   "Enables Clang-query support" OFF)
option(ENABLE_CLANGTIDY    "Enables clang-tidy support" OFF)
option(ENABLE_CLANGFORMAT  "Enables ClangFormat support" OFF)

# Allow these options to be ON by default if "Clang" is a substring of the compiler name we are using
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang") # Clang, AppleClang, etc
  set(ENABLE_CLANGQUERY ON)
  set(ENABLE_CLANGTIDY ON)
  set(ENABLE_CLANGFORMAT ON)
endif() 

# CXX Frontend options (EDG/Clang) 
option(EDG_COMPILE "Compile EDG source code if available" ON)
option(ENABLE_CLANG_FRONTEND "Use Clang for C/C++ frontend" OFF)

option(ENABLE_CPPCHECK     "Enables Cppcheck support" OFF)
option(ENABLE_VALGRIND     "Enables Valgrind support" ON)
option(ENABLE_ASTYLE       "Enables AStyle support" OFF)
option(ENABLE_UNCRUSTIFY   "Enables Uncrustify support" OFF)
option(ENABLE_YAPF         "Enables Yapf support" OFF)
option(ENABLE_CMAKEFORMAT  "Enables CMakeFormat support" OFF)

#------------------------------------------------------------------------------
# Build Options
#------------------------------------------------------------------------------
get_property(_languages GLOBAL PROPERTY ENABLED_LANGUAGES)
if(_languages MATCHES "Fortran")
    set(_fortran_already_enabled TRUE)
else()
    set(_fortran_already_enabled FALSE)
endif()
option(ENABLE_FORTRAN      "Enables Fortran compiler support" ${_fortran_already_enabled})

option(ENABLE_MPI          "Enables MPI support" OFF)
option(ENABLE_OPENMP       "Enables OpenMP compiler support" OFF)
option(ENABLE_CUDA         "Enable CUDA support" OFF)
cmake_dependent_option(ENABLE_CLANG_CUDA   "Enable Clang's native CUDA support" OFF
                       "ENABLE_CUDA" OFF)
mark_as_advanced(ENABLE_CLANG_CUDA)
set(BLT_CLANG_CUDA_ARCH "sm_30" CACHE STRING "Compute architecture to use when generating CUDA code with Clang")
mark_as_advanced(BLT_CLANG_CUDA_ARCH)
option(ENABLE_HIP         "Enable HIP support" OFF)
set(CMAKE_HIP_ARCHITECTURES "gfx900" CACHE STRING "gfx architecture to use when generating HIP/ROCm code")

#------------------------------------------------------------------------------
# Test Options
#
# Options that control if Google Test, Google Mock, and Fruit are built 
# and available for use. 
#
# If ENABLE_TESTS=OFF, no testing support is built and these option are ignored.
#
# Google Mock requires and always builds Google Test, so ENABLE_GMOCK=ON
# implies ENABLE_GTEST=ON.
#------------------------------------------------------------------------------
get_property(_languages GLOBAL PROPERTY ENABLED_LANGUAGES)
if(_languages MATCHES "CXX")
  set(_CXX_enabled ON)
else()
  set(_CXX_enabled OFF)
endif()
option(ENABLE_GTEST        "Enable Google Test testing support (if ENABLE_TESTS=ON)" ${_CXX_enabled})
option(ENABLE_GMOCK        "Enable Google Mock testing support (if ENABLE_TESTS=ON)" OFF)
option(ENABLE_FRUIT        "Enable Fruit testing support (if ENABLE_TESTS=ON and ENABLE_FORTRAN=ON)" ON)
option(ENABLE_FRUIT_MPI    "Enable Fruit MPI testing support (if ENABLE_TESTS=ON and ENABLE_FORTRAN=ON and ENABLE_FRUIT=ON and ENABLE_MPI=ON" OFF)
option(ENABLE_GBENCHMARK   "Enable Google Benchmark support (if ENABLE_TESTS=ON)" ${ENABLE_BENCHMARKS})

if( (NOT _CXX_enabled) AND ENABLE_GTEST )
  message( FATAL_ERROR
    "You must have CXX enabled in your project to use GTEST!" )
endif()

#------------------------------------------------------------------------------
# Compiler Options
#------------------------------------------------------------------------------
option(ENABLE_ALL_WARNINGS         "Enables all compiler warnings on all build targets" OFF)

# All warnings On by default for this build type 
if(CMAKE_BUILD_TYPE STREQUAL "Debug") 
  set(ENABLE_ALL_WARNINGS ON) 
endif() 

option(ENABLE_WARNINGS_AS_ERRORS   "Enables treating compiler warnings as errors on all build targets" OFF)
cmake_dependent_option(BLT_ENABLE_MSVC_STATIC_MD_TO_MT
                                   "When linking statically with MS Visual Studio, enables changing /MD to /MT"
                                   ON
                                   "NOT BUILD_SHARED_LIBS"
                                   OFF)
mark_as_advanced(BLT_ENABLE_MSVC_STATIC_MD_TO_MT)

#------------------------------------------------------------------------------
# Generator Options
#------------------------------------------------------------------------------
option(ENABLE_FOLDERS "Organize projects using folders (in generators that support this)" OFF)

#------------------------------------------------------------------------------
# Export/Install Options
#------------------------------------------------------------------------------
option(BLT_EXPORT_THIRDPARTY "Configure the third-party targets created by BLT to be exportable" OFF)

#------------------------------------------------------------------------------
# Advanced configuration options
#------------------------------------------------------------------------------

option(ENABLE_FIND_MPI     "Enables CMake's Find MPI support (Turn off when compiling with the mpi wrapper directly)" ON)

option(
    ENABLE_GTEST_DEATH_TESTS
    "Enables tests that assert application failure. Only valid when tests are enabled"
    OFF )

option(
    ENABLE_WRAP_ALL_TESTS_WITH_MPIEXEC
    "Option to ensure that all tests are invoked through mpiexec. Required on some platforms, like IBM's BG/Q."
    OFF )
       
if (DEFINED ENABLE_SHARED_LIBS)
    message(FATAL_ERROR "ENABLE_SHARED_LIBS is a deprecated BLT option."
                        "Use the standard CMake option, BUILD_SHARED_LIBS, instead.")
endif()

# Provide some overridable target names for custom targets that blt defines.
# This can be useful when working with other build systems since CMake requires
# unique names for targets.
set(BLT_CODE_CHECK_TARGET_NAME "check" CACHE STRING "Name of the master code check target")
set(BLT_CODE_STYLE_TARGET_NAME "style" CACHE STRING "Name of the master code formatting target")
set(BLT_DOCS_TARGET_NAME "docs" CACHE STRING "Name of the master documentation generation target")
set(BLT_RUN_BENCHMARKS_TARGET_NAME "run_benchmarks" CACHE STRING "Name of the target to run benchmark tests")


# All advanced options should be marked as advanced
mark_as_advanced(
    ENABLE_FIND_MPI
    ENABLE_GTEST_DEATH_TESTS
    ENABLE_WRAP_ALL_TESTS_WITH_MPIEXEC 
    BLT_CODE_CHECK_TARGET_NAME
    BLT_CODE_STYLE_TARGET_NAME
    BLT_DOCS_TARGET_NAME
    BLT_RUN_BENCHMARKS_TARGET_NAME )
