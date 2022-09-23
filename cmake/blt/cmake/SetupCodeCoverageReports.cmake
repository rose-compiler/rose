# Copyright (c) 2012 - 2015, Lars Bilke. See the top-level LICENSE file for details
# 
# SPDX-License-Identifier: (BSD-3-Clause)


# 2012-01-31, Lars Bilke
# - Enable Code Coverage
#
# 2013-09-17, Joakim Soderberg
# - Added support for Clang.
# - Some additional usage instructions.
#
# (Original LCOV + GENHTML CMake Macro) from Lars Bilke + Joakim Soderberg
#
# 2015-07-06, Aaron Black
# - Modified for use by BLT.
#
# 2016-04-04, Kenny Weiss
# - Removed support for clang; simplified flags
#
# 2017-07-25, Cyrus Harrison
# - Refactored to only include report gen logic, not coverage flags
#
# 2020-11-18, Josh Essman
# - Continue generation after failing tests, allow user to specify src dir

set(BLT_CODE_COVERAGE_REPORTS ON)

# Check for lcov
if(NOT EXISTS ${LCOV_EXECUTABLE})
    message(STATUS "Code coverage: Unable to find lcov, disabling code coverage reports.")
    set(BLT_CODE_COVERAGE_REPORTS OFF)
endif()

# Check for genthml
if(NOT EXISTS ${GENHTML_EXECUTABLE})
    message(STATUS "Code coverage: Unable to find genhtml, disabling code coverage reports.")
    set(BLT_CODE_COVERAGE_REPORTS OFF)
endif()

# Check for gcov
if(NOT EXISTS ${GCOV_EXECUTABLE})
   message(STATUS "Code coverage: GCOV_EXECUTABLE is not set, disabling code coverage reports")
   set(BLT_CODE_COVERAGE_REPORTS OFF)
endif()
    
mark_as_advanced(BLT_CODE_COVERAGE_REPORTS)


##------------------------------------------------------------------------------
## blt_add_code_coverage_target( NAME              <Created Target Name>
##                               RUNNER            <The command to run the tests>
##                               SOURCE_DIRECTORIES [dir1 [dir2 ...]])
##
## Creates a new target with the given NAME that generates a code coverage report.
##------------------------------------------------------------------------------
function(blt_add_code_coverage_target)

    set(options)
    set(singleValueArgs NAME)
    set(multiValueArgs RUNNER SOURCE_DIRECTORIES)

    # parse the arguments
    cmake_parse_arguments(arg
        "${options}" "${singleValueArgs}" "${multiValueArgs}" ${ARGN} )

    # Check/Set required parameters
    if(NOT DEFINED arg_NAME)
        message(FATAL_ERROR "blt_add_code_coverage_target requires a NAME parameter")
    endif()

    if(NOT DEFINED arg_RUNNER)
        message(FATAL_ERROR "blt_add_code_coverage_target requires a RUNNER parameter")
    endif()

    set(_coverage_directories "--directory=${CMAKE_BINARY_DIR}")
    if(DEFINED arg_SOURCE_DIRECTORIES)
        foreach(_src_dir ${arg_SOURCE_DIRECTORIES})
            list(APPEND _coverage_directories "--directory=${_src_dir}")
        endforeach()
    else()
        # Default to everything
        list(APPEND _coverage_directories "--directory=${CMAKE_CURRENT_SOURCE_DIR}")
    endif()

    # Setup target
    add_custom_target(${arg_NAME}

        # Cleanup lcov
        ${LCOV_EXECUTABLE} --no-external --gcov-tool ${GCOV_EXECUTABLE} ${_coverage_directories} --zerocounters

        # Run tests - allow for failing tests
        COMMAND ${arg_RUNNER} || (exit 0)

        # Capture lcov counters and generating report
        COMMAND ${LCOV_EXECUTABLE} --no-external --gcov-tool ${GCOV_EXECUTABLE} ${_coverage_directories} --capture --output-file ${arg_NAME}.info
        COMMAND ${LCOV_EXECUTABLE} --no-external --gcov-tool ${GCOV_EXECUTABLE} ${_coverage_directories} --remove ${arg_NAME}.info '/usr/include/*' --output-file ${arg_NAME}.info.cleaned
        COMMAND ${GENHTML_EXECUTABLE} -o ${arg_NAME} ${arg_NAME}.info.cleaned
        BYPRODUCTS ${arg_NAME}.info ${arg_NAME}.info.cleaned
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Resetting code coverage counters to zero.\nProcessing code coverage counters and generating report."
    )

    # Show info where to find the report
    add_custom_command(TARGET ${arg_NAME} POST_BUILD
        COMMAND ;
        COMMENT "Open ./${arg_NAME}/index.html in your browser to view the coverage report."
    )
endfunction()


if(BLT_CODE_COVERAGE_REPORTS)
        # Add code coverage target
        blt_add_code_coverage_target(NAME coverage RUNNER make test)
        message(STATUS "Code coverage: reports enabled via lcov, genthml, and gcov.")
endif()
