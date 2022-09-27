# Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
# other BLT Project Developers. See the top-level LICENSE file for details
# 
# SPDX-License-Identifier: (BSD-3-Clause)

#------------------------------------------------------------------------------
# BLT Internal Testing Project
#------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.8)

project(blt-internal-tests LANGUAGES C CXX)

#------------------------------------------------------------------------------
# Setup BLT
#------------------------------------------------------------------------------
# Set BLT_SOURCE_DIR to default location, if not set by user 
if(NOT BLT_SOURCE_DIR)
    set(BLT_SOURCE_DIR "${PROJECT_SOURCE_DIR}/../..")
endif()

if (NOT BLT_CXX_STD)
    set(BLT_CXX_STD "c++11" CACHE STRING "")
endif()

include(${BLT_SOURCE_DIR}/SetupBLT.cmake)

#------------------------------------------------------------------------------
# Project options
#------------------------------------------------------------------------------
option(TEST_GIT_MACROS 
       "Toggle smoke tests for git macros. Off by default since it requires a modified git repo." 
       OFF)


#------------------------------------------------------------------------------
# Add a library
#------------------------------------------------------------------------------

blt_add_library( NAME    example
                 SOURCES "src/Example.cpp"
                 HEADERS "src/Example.hpp"
                 )

if(WIN32 AND BUILD_SHARED_LIBS)
    target_compile_definitions(example PUBLIC WIN32_SHARED_LIBS)
endif()

if(ENABLE_GTEST)
    #------------------------------------------------------
    # Add an executable
    # (which happens to be a test)
    #------------------------------------------------------
    blt_add_executable(
             NAME t_example_smoke
             SOURCES "src/t_example_smoke.cpp"
             DEPENDS_ON example gtest)

    #------------------------------------------------------
    # Register our test w/ ctest
    #------------------------------------------------------
    blt_add_test(NAME t_example_smoke
                 COMMAND t_example_smoke)


    #------------------------------------------------------
    #  Header-only test
    #------------------------------------------------------

    blt_add_target_definitions(TO                 example
                               TARGET_DEFINITIONS BLT_EXAMPLE_LIB)

    blt_add_library(NAME blt_header_only
                    HEADERS "src/HeaderOnly.hpp"
                    DEPENDS_ON example)

    # This executable depends on the header-only library

    blt_add_executable(
      NAME t_header_only_smoke
      SOURCES "src/t_header_only_smoke.cpp"
      DEPENDS_ON blt_header_only gtest)

    blt_add_test(NAME t_header_only_smoke
                 COMMAND t_header_only_smoke)

    #------------------------------------------------------
    # Tests blt_combine_static_libs macro
    # Note: Does not currently work on Windows or BGQ
    #------------------------------------------------------
    set(_try_combine_static_libs ON)

    if(CMAKE_HOST_WIN32)
        set(_try_combine_static_libs OFF)
    endif()

    if ( ${_try_combine_static_libs} )
        add_subdirectory( src/combine_static_library_test )
    endif ()

    #------------------------------------------------------
    # Git Macros test
    #------------------------------------------------------
    if ( GIT_FOUND AND TEST_GIT_MACROS)

      blt_is_git_repo( OUTPUT_STATE is_git_repo
                       SOURCE_DIR ${PROJECT_SOURCE_DIR} )

      if ( ${is_git_repo} )

        ## get the latest tag from the master branch
        blt_git_tag( OUTPUT_TAG blt_tag
                     RETURN_CODE rc
                     ON_BRANCH master
                     SOURCE_DIR ${PROJECT_SOURCE_DIR}
                     )
        if ( NOT ${rc} EQUAL 0 )
          message(FATAL_ERROR "blt_git_tag failed!")
        endif()

        ## get the name of the current (i.e., checked out) branch
        blt_git_branch( BRANCH_NAME blt_branch
                        RETURN_CODE rc
                        SOURCE_DIR ${PROJECT_SOURCE_DIR}
                        )
        if ( NOT ${rc} EQUAL 0 )
          message(FATAL_ERROR "blt_git_branch failed!" )
        endif()

        ## get sha1 at the tip of the current branch
        blt_git_hashcode ( HASHCODE blt_sha1
                           RETURN_CODE rc
                           SOURCE_DIR ${PROJECT_SOURCE_DIR}
                           )
        if ( NOT ${rc} EQUAL 0 )
          message(FATAL_ERROR "blt_git_hashcode failed!")
        endif()

        set(BLT_TEST_TAG ${blt_tag})
        set(BLT_TEST_SHA1 ${blt_sha1})
        set(BLT_TEST_BRANCH ${blt_branch})

        configure_file( src/t_git_macros_smoke.cpp.in
                        ${CMAKE_BINARY_DIR}/t_git_macros_smoke.cpp )

        blt_add_executable(
          NAME       t_git_macros_smoke
          SOURCES    ${CMAKE_BINARY_DIR}/t_git_macros_smoke.cpp
          DEPENDS_ON gtest
          )

        blt_add_test( NAME    t_git_macros_smoke
                      COMMAND t_git_macros_smoke )

      endif() # endif is_git_repo

    endif() # endif Git_FOUND

    #------------------------------------------------------
    # CUDA tests
    #------------------------------------------------------
    if (ENABLE_CUDA AND CUDA_SEPARABLE_COMPILATION)
        add_subdirectory(src/test_cuda_device_call_from_kernel)
    endif()
    
    #------------------------------------------------------
    # Tests blt_add_target_definitions macro
    #
    # Four variants of a test with a list of two definitions
    #------------------------------------------------------
    set(_variant_1 BLT_A=1 BLT_B)         # neither use '-D'
    set(_variant_2 -DBLT_A=1 -DBLT_B)     # both uses '-D'
    set(_variant_3 "BLT_A=1;-DBLT_B")     # list passed in as string
    set(_variant_4 " " "-DBLT_A=1;BLT_B") # list can contain empty strings
    foreach(i RANGE 1 4)
        set(_casename "_variant_${i}")
        set(_testname "t_example_compile_definitions_test${_casename}")

        blt_add_executable( 
            NAME ${_testname}
            SOURCES src/t_example_compile_definitions.cpp 
            DEPENDS_ON gtest)

        blt_add_target_definitions(
            TO ${_testname}
            TARGET_DEFINITIONS ${${_casename}})

        blt_add_test( 
            NAME ${_testname}
            COMMAND ${_testname})
    endforeach()

    #------------------------------------------------------
    # Tests the IF clause of the blt_list_append macro
    #
    # We expect variables that are not defined to be omitted.
    # For defined variables, we expect them to be added when
    # they evaluate to TRUE, whether or not they are escaped.
    #------------------------------------------------------
    
    unset(_existing_true_var)
    unset(_existing_false_var)
    
    set(_existing_true_var TRUE)
    set(_existing_false_var FALSE)
    set(_defined_empty_var "")
    set(_defined_nonempty_var "<evaluates-to-true-before-but-not-after-escaping>")
    unset(_undefined_var)
    
    unset(_actual_list) # blt_list_append can work on an initially undefined list
    
    # The following will be added to the list
    blt_list_append(TO _actual_list ELEMENTS "true_literal"         IF TRUE)
    blt_list_append(TO _actual_list ELEMENTS "true_nonescaped"      IF _existing_true_var)
    blt_list_append(TO _actual_list ELEMENTS "true_escaped"         IF ${_existing_true_var})
    blt_list_append(TO _actual_list ELEMENTS "nonempty_nonescaped"  IF _defined_nonempty_var)
    set(_expected_list "true_literal" "true_nonescaped" "true_escaped" "nonempty_nonescaped")
    set(_expected_size 4)
    
    # The following will not be added to the list
    blt_list_append(TO _actual_list ELEMENTS "false_literal"        IF FALSE)
    blt_list_append(TO _actual_list ELEMENTS "false_nonescaped"     IF _existing_false_var)
    blt_list_append(TO _actual_list ELEMENTS "false_escaped"        IF ${_existing_false_var})
    blt_list_append(TO _actual_list ELEMENTS "undefined_nonescaped" IF _nonexisting_var)
    blt_list_append(TO _actual_list ELEMENTS "undefined_escaped"    IF ${_nonexisting_var})
    blt_list_append(TO _actual_list ELEMENTS "empty_nonescaped"     IF _defined_empty_var)
    blt_list_append(TO _actual_list ELEMENTS "empty_escaped"        IF ${_defined_empty_var})
    blt_list_append(TO _actual_list ELEMENTS "nonempty_escaped"     IF ${_defined_nonempty_var})

    # Check that ELEMENTS can be empty or missing when the IF condition is false
    blt_list_append(TO _actual_list                                 IF FALSE)
    blt_list_append(TO _actual_list ELEMENTS ""                     IF FALSE)

    # Check the results
    list(LENGTH _actual_list _actual_size)
    if(NOT "${_actual_list}" STREQUAL "${_expected_list}"
       OR NOT ${_actual_size} EQUAL ${_expected_size})
        message(FATAL_ERROR "[blt_list_append] Unexpected evaluation: "
                            "\n\t" "Expected: '${_expected_list}'"
                            "\n\t" "Got:      '${_actual_list}'" )
    endif()

endif() # endif ENABLE_GTEST

if (ENABLE_CUDA AND ENABLE_MPI AND 
    "${BLT_CXX_STD}" MATCHES c\\+\\+1)
    blt_add_executable(
      NAME       test_cuda_mpi
      SOURCES    src/test_cuda_mpi.cpp
      DEPENDS_ON cuda mpi hwloc)

    # Tests on a 2^24 elements array.
    # It can go much bigger, but will not
    # exceed memory capacity in most computers.
    blt_add_test(NAME          test_cuda_mpi
                 COMMAND       test_cuda_mpi 24
                 NUM_MPI_TASKS 4)
endif()

message(STATUS "Exercising blt_print_target_properties macro on some targets and non-targets.")
message(STATUS "")
foreach(_target gtest example t_example_smoke not-a-target blt_header_only mpi cuda cuda_runtime blt::hip blt::hip_runtime)
    blt_print_target_properties(TARGET ${_target})
endforeach()

add_subdirectory(src/object_library_test)

if(ENABLE_CLANGQUERY OR ENABLE_CLANGTIDY)
  add_subdirectory(src/static_analysis)
endif()

if(ENABLE_HIP)
  add_subdirectory(src/hip_defines_test)
endif()

add_subdirectory(unit)
#------------------------------------------------------------------------------
# Format the testing code using ClangFormat
#------------------------------------------------------------------------------
if(CLANGFORMAT_FOUND)

  set(smoke_tests_srcs
    ../smoke/blt_cuda_mpi_smoke.cpp
    ../smoke/blt_cuda_openmp_smoke.cpp
    ../smoke/blt_cuda_runtime_smoke.cpp
    ../smoke/blt_cuda_gtest_smoke.cpp
    ../smoke/blt_cuda_smoke.cpp
    ../smoke/blt_fruit_smoke.f90
    ../smoke/blt_gbenchmark_smoke.cpp
    ../smoke/blt_gmock_smoke.cpp
    ../smoke/blt_gtest_smoke.cpp
    ../smoke/blt_hip_runtime_smoke.cpp
    ../smoke/blt_hip_gtest_smoke.cpp
    ../smoke/blt_hip_smoke.cpp
    ../smoke/blt_mpi_smoke.cpp
    ../smoke/blt_openmp_smoke.cpp
    ../smoke/fortran_driver.cpp
    ../smoke/fortran_mpi_driver.cpp
  )

  set(internal_tests_srcs
    src/Example.cpp
    src/Example.hpp
    src/Example_Exports.h
    src/HeaderOnly.hpp

    src/combine_static_library_test/Foo1.cpp
    src/combine_static_library_test/Foo1.hpp
    src/combine_static_library_test/Foo2.cpp
    src/combine_static_library_test/Foo2.hpp
    src/combine_static_library_test/Foo3.cpp
    src/combine_static_library_test/Foo3.hpp
    src/combine_static_library_test/blt_combine_static_libraries_shared_smoke.cpp
    src/combine_static_library_test/blt_combine_static_libraries_static_smoke.cpp
    src/combine_static_library_test/dummy.cpp
    src/combine_static_library_test/main.cpp

    src/object_library_test/base_object.cpp
    src/object_library_test/base_object.hpp

    src/object_library_test/inherited_base/inherited_base.cpp
    src/object_library_test/inherited_base/inherited_base.hpp
    src/object_library_test/main.cpp
    src/object_library_test/object.cpp
    src/object_library_test/object.hpp

    src/static_analysis/well_analyzed_source.cpp
    src/t_example_compile_definitions.cpp
    src/t_example_smoke.cpp
    src/t_header_only_smoke.cpp
    src/test_cuda_mpi.cpp
    
    src/test_cuda_device_call_from_kernel/Child.cpp
    src/test_cuda_device_call_from_kernel/Child.hpp
    src/test_cuda_device_call_from_kernel/CudaTests.cpp
    src/test_cuda_device_call_from_kernel/Parent.cpp
    src/test_cuda_device_call_from_kernel/Parent.hpp

    src/hip_defines_test/foo.cpp
    src/hip_defines_test/bar.cpp
  )

  # Specify the major version for astyle
  set(BLT_REQUIRED_ASTYLE_VERSION "3")

  blt_add_code_checks(
      PREFIX          smoke_tests
      SOURCES         ${smoke_tests_srcs}
      ASTYLE_CFG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/astyle.cfg )

  blt_add_code_checks(
      PREFIX          internal_tests
      SOURCES         ${internal_tests_srcs}
      ASTYLE_CFG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/astyle.cfg )
      
endif()

if(YAPF_FOUND)
  # Test case where a style target running yapf should do nothing, and
  # a corresponding check target should return 0
  set(internal_conformant_python_srcs
    src/test_yapf_conformant.py)

  blt_add_code_checks(
    PREFIX           yapf_conformant_tests
    SOURCES          ${internal_conformant_python_srcs}
    YAPF_CFG_FILE    ${CMAKE_CURRENT_SOURCE_DIR}/yapf.cfg )

  # Test case where a style target running yapf should reformat the source file,
  # and a corresponding check target should return a nonzero code
  #
  # After styling, test_yapf_conformant.py and
  # test_yapf_nonconformant.py should have the same contents
  set(internal_nonconformant_python_srcs
    src/test_yapf_nonconformant.py)

  blt_add_code_checks(
    PREFIX           yapf_nonconformant_tests
    SOURCES          ${internal_nonconformant_python_srcs}
    YAPF_CFG_FILE    ${CMAKE_CURRENT_SOURCE_DIR}/yapf.cfg)
endif() # end if(YAPF_FOUND)

if (CMAKEFORMAT_FOUND)
  # Test case where a style target running cmake-format should do nothing, and
  # a corresponding check target should return 0
  set(internal_conformant_cmake_srcs
    src/test_cmake_format_conformant.cmake)

  blt_add_code_checks(
    PREFIX               cmake_format_conformant_tests
    SOURCES              ${internal_conformant_cmake_srcs}
    CMAKEFORMAT_CFG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake-format-style.py )

  # Test case where a style target running cmake-format should reformat the source file,
  # and a corresponding check target should return a nonzero code
  #
  # After styling, cmake_format_conformant.cmake and
  # cmake_format_nonconformant.cmake should have the same contents
  set(internal_nonconformant_cmake_srcs
    src/test_cmake_format_nonconformant.cmake)

  blt_add_code_checks(
    PREFIX               cmake_format_nonconformant_tests
    SOURCES              ${internal_nonconformant_cmake_srcs}
    CMAKEFORMAT_CFG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake-format-style.py )
endif() # end if(CMAKEFORMAT_FOUND)
  
# Check blt_add_test with a command that is not a target
if(WIN32)
    configure_file(return_true_win32.in tests/return_true.bat COPYONLY)
    blt_add_test(NAME    test_executable_not_a_target
                 COMMAND tests/return_true.bat)
else()
    configure_file(return_true.in tests/return_true COPYONLY)
    blt_add_test(NAME    test_executable_not_a_target
                 COMMAND tests/return_true)
endif()

# create example depending on all available TPLs to test export
set(_example_tpl_depends)
blt_list_append(TO _example_tpl_depends ELEMENTS cuda cuda_runtime IF ENABLE_CUDA)
blt_list_append(TO _example_tpl_depends ELEMENTS blt_hip blt_hip_runtime IF ENABLE_HIP)
blt_list_append(TO _example_tpl_depends ELEMENTS openmp IF ENABLE_OPENMP)
blt_list_append(TO _example_tpl_depends ELEMENTS mpi IF ENABLE_MPI)

blt_add_library( NAME    example-with-tpls
                 SOURCES "src/Example.cpp"
                 HEADERS "src/Example.hpp"
                 DEPENDS_ON ${_example_tpl_depends}
                 )

install(TARGETS example-with-tpls EXPORT blt-exports ARCHIVE DESTINATION lib)

# test blt_export_tpl_targets macro
blt_export_tpl_targets(NAMESPACE blt
                       EXPORT blt-exports)

install(EXPORT blt-exports DESTINATION lib/cmake/blt)

# Add tutorials
add_subdirectory(../../docs/tutorial/calc_pi calc_pi)
