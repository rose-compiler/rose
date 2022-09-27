#------------------------------------------------------------------------------
# BLT Tutorial Example: Calc Pi.
#------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.8)

project( pi_playground )

#------------------------------------------------------------------------------
# Setup BLT
#------------------------------------------------------------------------------
# Set BLT_SOURCE_DIR to default location, if not set by user 
if(NOT BLT_SOURCE_DIR)
    set(BLT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../..")
endif()

# Default to C++11 if not set so GTest/GMock can build
if (NOT BLT_CXX_STD)
    set(BLT_CXX_STD "c++11" CACHE STRING "")
endif()

include(${BLT_SOURCE_DIR}/SetupBLT.cmake)

#------------------------------------------------------------------------------
# Example 1: Creating a simple executable.
#------------------------------------------------------------------------------
# _blt_tutorial_example_executable_start
blt_add_executable( NAME    example_1
                    SOURCES example_1.cpp )
# _blt_tutorial_example_executable_end

#------------------------------------------------------------------------------
# Example 2: Creating a library and an executable using our library.
#------------------------------------------------------------------------------
# _blt_tutorial_calcpi_library_start
blt_add_library( NAME    calc_pi
                 HEADERS calc_pi.hpp calc_pi_exports.h
                 SOURCES calc_pi.cpp )
# _blt_tutorial_calcpi_library_end

if(WIN32 AND BUILD_SHARED_LIBS)
    target_compile_definitions(calc_pi PUBLIC WIN32_SHARED_LIBS)
endif()

# _blt_tutorial_calcpi_example2_start 
blt_add_executable( NAME       example_2
                    SOURCES    example_2.cpp 
                    DEPENDS_ON calc_pi)
# _blt_tutorial_calcpi_example2_end

#------------------------------------------------------------------------------
# Test 1: Creating an executable using gtest, using the executable via ctest.
#------------------------------------------------------------------------------
# _blt_tutorial_calcpi_test1_executable_start
blt_add_executable( NAME       test_1
                    SOURCES    test_1.cpp 
                    DEPENDS_ON calc_pi gtest)
# _blt_tutorial_calcpi_test1_executable_end

# _blt_tutorial_calcpi_test1_test_start
blt_add_test( NAME    test_1 
              COMMAND test_1)
# _blt_tutorial_calcpi_test1_test_end

#------------------------------------------------------------------------------
# Test 2: Add mpi version of calc_pi, and expand test 1 to also test 
# the mpi version.
#------------------------------------------------------------------------------
if(MPI_FOUND)
# _blt_tutorial_calcpi_test2_executable_start
    blt_add_library( NAME       calc_pi_mpi
                     HEADERS    calc_pi_mpi.hpp calc_pi_mpi_exports.h
                     SOURCES    calc_pi_mpi.cpp 
                     DEPENDS_ON mpi)

    if(WIN32 AND BUILD_SHARED_LIBS)
        target_compile_definitions(calc_pi_mpi PUBLIC WIN32_SHARED_LIBS)
    endif()

    blt_add_executable( NAME       test_2
                        SOURCES    test_2.cpp 
                        DEPENDS_ON calc_pi calc_pi_mpi gtest)
# _blt_tutorial_calcpi_test2_executable_end

# _blt_tutorial_calcpi_test2_test_start
    blt_add_test( NAME          test_2 
                  COMMAND       test_2
                  NUM_MPI_TASKS 2) # number of mpi tasks to use
# _blt_tutorial_calcpi_test2_test_end
endif()

#------------------------------------------------------------------------------
# Test 3: Add cuda version of calc_pi, and expand test 1 to also test 
# the cuda version.
#------------------------------------------------------------------------------
if(CUDA_FOUND)
# _blt_tutorial_calcpi_cuda_start
	
    blt_add_library( NAME       calc_pi_cuda
                     HEADERS    calc_pi_cuda.hpp calc_pi_cuda_exports.h
                     SOURCES    calc_pi_cuda.cpp 
                     DEPENDS_ON cuda)

    if(WIN32 AND BUILD_SHARED_LIBS)
        target_compile_definitions(calc_pi_cuda PUBLIC WIN32_SHARED_LIBS)
    endif()

    blt_add_executable( NAME       test_3
                        SOURCES    test_3.cpp 
                        DEPENDS_ON calc_pi calc_pi_cuda gtest cuda_runtime)

    blt_add_test( NAME    test_3
                  COMMAND test_3)
# _blt_tutorial_calcpi_cuda_end
endif()


#------------------------------------------------------------------------------
# Add Documentation Examples
#------------------------------------------------------------------------------
if (ENABLE_DOCS)
    add_subdirectory(docs)
endif()
