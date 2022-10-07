# Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
# other BLT Project Developers. See the top-level LICENSE file for details
# 
# SPDX-License-Identifier: (BSD-3-Clause)
#################################################
# Setup code metrics - coverage, profiling, etc
#################################################

########################################
# Enable code coverage via gcov
# Note: Only supported for gnu or clang.
########################################
if (ENABLE_COVERAGE)
    ##########################################################################
    # Setup coverage compiler flags 
    ##########################################################################
    # Set the actual flags for coverage in the COVERAGE_FLAGS variable 
    # Note: For gcc '--coverage' is equivalent to 
    # '-fprofile-arcs -ftest-coverage' for compiling and '-lgcov' for linking
    # Additional flags that might be useful: 
    #       " -fno-inline -fno-inline-small-functions -fno-default-inline"
    blt_append_custom_compiler_flag(FLAGS_VAR   COVERAGE_FLAGS 
                                    DEFAULT " "
                                    GNU     "--coverage"
                                    CLANG   "--coverage")
        
    SET( CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${COVERAGE_FLAGS}" )
    SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${COVERAGE_FLAGS}" )
    SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${COVERAGE_FLAGS}" )
    
    if(ENABLE_FORTRAN)
        SET( CMAKE_Fortran_FLAGS  "${CMAKE_Fortran_FLAGS} ${COVERAGE_FLAGS}" )
    endif()

    ######################################
    # Setup Code Coverage Report Targets
    ######################################
    include(${BLT_ROOT_DIR}/cmake/SetupCodeCoverageReports.cmake)
    
endif()

if (VALGRIND_FOUND)
    set(MEMORYCHECK_COMMAND ${VALGRIND_EXECUTABLE} CACHE PATH "")
    set(MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --leak-check=full" CACHE PATH "")
endif()
