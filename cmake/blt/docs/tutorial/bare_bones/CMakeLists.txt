#------------------------------------------------------------------------------
# BLT Tutorial Example: Bare Bones Project.
#------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.8)
project( bare_bones )

# Note: This is specific to running our tests and shouldn't be exported to documentation
if(NOT BLT_SOURCE_DIR)
    set(BLT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../..")
endif()

#------------------------------------------------------------------------------
# Setup BLT
#------------------------------------------------------------------------------

# _blt_tutorial_include_blt_start
if (DEFINED BLT_SOURCE_DIR)
    # Support having a shared BLT outside of the repository if given a BLT_SOURCE_DIR
    if (NOT EXISTS ${BLT_SOURCE_DIR}/SetupBLT.cmake)
        message(FATAL_ERROR "Given BLT_SOURCE_DIR does not contain SetupBLT.cmake")
    endif()
else()
    # Use internal BLT if no BLT_SOURCE_DIR is given
    set(BLT_SOURCE_DIR "${PROJECT_SOURCE_DIR}/cmake/blt" CACHE PATH "")
    if (NOT EXISTS ${BLT_SOURCE_DIR}/SetupBLT.cmake)
        message(FATAL_ERROR
            "The BLT git submodule is not present. "
            "Either run the following two commands in your git repository: \n"
            "    git submodule init\n"
            "    git submodule update\n"
            "Or add -DBLT_SOURCE_DIR=/path/to/blt to your CMake command." )
    endif()
endif()

# Default to C++11 if not set so GTest/GMock can build
if (NOT BLT_CXX_STD)
    set(BLT_CXX_STD "c++11" CACHE STRING "")
endif()

include(${BLT_SOURCE_DIR}/SetupBLT.cmake)
# _blt_tutorial_include_blt_end
