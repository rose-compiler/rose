# Copyright (c) 2017-2021, Lawrence Livermore National Security, LLC and
# other BLT Project Developers. See the top-level LICENSE file for details
# 
# SPDX-License-Identifier: (BSD-3-Clause)

#------------------------------------------------------------------------------
# BLT Internal Testing Project
#------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.8)

project(blt-unit-tests LANGUAGES C CXX)

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
# Test blt_split_source_list_by_language
#------------------------------------------------------------------------------

message( 
"*****************************************************\n"
"Testing `blt_split_source_list_by_language`...\n"
"*****************************************************")

# create list of source paths and sorted, correct list of answers
set(original_srcs
    src/Example.cpp
    src/Example.hpp
    multiple.exts.c
    quadrature_rule.order2.c
    source/.f90/with/extension/in/middle.c
    fortran.F
    fortran2.f
    fortran3.f90
    fortran4.F90
    python.py
    some_other.multi.py
    CMakeLists.txt
    long/path/CMakeLists.txt
    macro_file.cmake
    )

set(correct_c_srcs
    src/Example.cpp
    src/Example.hpp
    multiple.exts.c
    quadrature_rule.order2.c
    source/.f90/with/extension/in/middle.c
    )

set(correct_fortran_srcs
    fortran.F
    fortran2.f
    fortran3.f90
    fortran4.F90
    )

set(correct_python_srcs
    python.py
    some_other.multi.py
    )

set(correct_cmake_srcs
    CMakeLists.txt
    long/path/CMakeLists.txt
    macro_file.cmake
    )

# Split sources
set(c_srcs)
set(fortran_srcs)
set(python_srcs)
set(cmake_srcs)
blt_split_source_list_by_language( SOURCES      ${original_srcs}
                                   C_LIST       c_srcs
                                   Fortran_LIST fortran_srcs
                                   Python_LIST  python_srcs
                                   CMAKE_LIST   cmake_srcs)

# Macro to see if lists have the same items and errors on non-equality
macro(compare_source_lists)

    set(options)
    set(singleValueArgs A_LIST B_LIST)
    set(multiValueArgs )

    # Parse the arguments
    cmake_parse_arguments(arg "${options}" "${singleValueArgs}"
                            "${multiValueArgs}" ${ARGN} )

    message(STATUS "Comparing lists for equality:\n"
                   "  List A: ${${arg_A_LIST}}\n"
                   "  List B: ${${arg_B_LIST}}")

    set(a_len)
    set(b_len)
    list(LENGTH ${arg_A_LIST} a_len)
    list(LENGTH ${arg_B_LIST} b_len)
    if (NOT a_len EQUAL b_len)
        message(FATAL_ERROR "Split source test failed. Lists had differing lengths.")
    endif()

    set(sorted_a_list ${${arg_A_LIST}})
    set(sorted_b_list ${${arg_B_LIST}})
    list(SORT sorted_a_list)
    list(SORT sorted_b_list)

    math(EXPR _range_stop "${a_len} - 1")
    foreach(i RANGE 0 ${_range_stop})
        set(a_item)
        set(b_item)
        list(GET sorted_a_list ${i} a_item)
        list(GET sorted_b_list ${i} b_item)

        if(NOT a_item STREQUAL b_item)
          message(FATAL_ERROR "Split source test failed. ${a_item} != ${b_item}")
        endif()
    endforeach()

    # Success if we reached here
endmacro(compare_source_lists)

message(STATUS "Full mixed source list: ${original_srcs}")

message(STATUS "Checking C/CXX filtering...")
compare_source_lists(A_LIST c_srcs B_LIST correct_c_srcs)

message(STATUS "Checking Fortran filtering...")
compare_source_lists(A_LIST fortran_srcs B_LIST correct_fortran_srcs)

message(STATUS "Checking Python filtering...")
compare_source_lists(A_LIST python_srcs B_LIST correct_python_srcs)

message(STATUS "Checking CMake filtering...")
compare_source_lists(A_LIST cmake_srcs B_LIST correct_cmake_srcs)

message( 
"*****************************************************\n"
"Tests passed for `blt_split_source_list_by_language`.\n"
"*****************************************************")

