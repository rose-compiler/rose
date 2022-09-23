# Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
# other BLT Project Developers. See the top-level LICENSE file for details
# 
# SPDX-License-Identifier: (BSD-3-Clause)

#
# Smoke tests for BLT TPLs
#

################
# gtest test
################
if(ENABLE_GTEST)
    blt_add_executable(NAME blt_gtest_smoke
                       SOURCES blt_gtest_smoke.cpp
                       OUTPUT_DIR ${TEST_OUTPUT_DIRECTORY}
                       DEPENDS_ON gtest
                       FOLDER blt/tests )

    blt_add_test( NAME blt_gtest_smoke
                  COMMAND blt_gtest_smoke)
endif()


################
# gmock test
################
if(ENABLE_GMOCK)
    blt_add_executable(NAME blt_gmock_smoke
                       SOURCES blt_gmock_smoke.cpp
                       OUTPUT_DIR ${TEST_OUTPUT_DIRECTORY}
                       DEPENDS_ON gtest gmock
                       FOLDER blt/tests )

    blt_add_test( NAME blt_gmock_smoke
                  COMMAND blt_gmock_smoke)
endif()


################
# gbenchmark test
################
if(ENABLE_GBENCHMARKS)
    blt_add_executable(NAME blt_gbenchmark_smoke
                       SOURCES blt_gbenchmark_smoke.cpp
                       OUTPUT_DIR ${TEST_OUTPUT_DIRECTORY}
                       DEPENDS_ON gbenchmark
                       FOLDER blt/benchmarks )

    if(ENABLE_BENCHMARKS)
        blt_add_benchmark(NAME     blt_gbenchmark_smoke 
                          COMMAND  blt_gbenchmark_smoke "--benchmark_min_time=0.0001")
    else()
        # A user could turn on gbenchmarks but turn off benchmarks
        blt_add_test(NAME     blt_gbenchmark_smoke 
                     COMMAND  blt_gbenchmark_smoke "--benchmark_min_time=0.0001")
    endif()
endif()


################
# fruit test
################
if (ENABLE_FORTRAN AND ENABLE_FRUIT)
    blt_add_executable(NAME blt_fruit_smoke
                       SOURCES blt_fruit_smoke.f90
                       OUTPUT_DIR ${TEST_OUTPUT_DIRECTORY}
                       DEPENDS_ON fruit
                       FOLDER blt/tests )

    blt_add_test(NAME blt_fruit_smoke
                 COMMAND blt_fruit_smoke)
endif()

###############
# fruit mpi test
###############
if (ENABLE_FORTRAN AND ENABLE_FRUIT AND ENABLE_MPI AND ENABLE_FRUIT_MPI)
    blt_add_executable(NAME blt_fruit_mpi_smoke
                       SOURCES blt_fruit_mpi_smoke.f90
                       OUTPUT_DIR ${TEST_OUTPUT_DIRECTORY}
                       DEPENDS_ON fruit_mpi
                       FOLDER blt/tests )

    blt_add_test(NAME blt_fruit_mpi_smoke
                 COMMAND blt_fruit_mpi_smoke)
endif()

################
# OpenMP test
################
if (ENABLE_OPENMP)
    # _blt_tutorial_openmp_executable_start
    blt_add_executable(NAME blt_openmp_smoke 
                       SOURCES blt_openmp_smoke.cpp 
                       OUTPUT_DIR ${TEST_OUTPUT_DIRECTORY}
                       DEPENDS_ON openmp
                       FOLDER blt/tests )
    # _blt_tutorial_openmp_executable_end

    # _blt_tutorial_openmp_test_start
    blt_add_test(NAME            blt_openmp_smoke
                 COMMAND         blt_openmp_smoke
                 NUM_OMP_THREADS 4)
    # _blt_tutorial_openmp_test_end
endif()


################
#  MPI test
################
if (ENABLE_MPI)
    blt_add_executable(NAME blt_mpi_smoke
                       SOURCES blt_mpi_smoke.cpp
                       OUTPUT_DIR ${TEST_OUTPUT_DIRECTORY} 
                       DEPENDS_ON mpi
                       FOLDER blt/tests )

    blt_add_test(NAME blt_mpi_smoke
                 COMMAND blt_mpi_smoke
                 NUM_MPI_TASKS 4)
endif()

################
#  CUDA tests
################
if (ENABLE_CUDA)
    blt_add_executable(NAME blt_cuda_smoke
                       SOURCES blt_cuda_smoke.cpp
                       OUTPUT_DIR ${TEST_OUTPUT_DIRECTORY} 
                       DEPENDS_ON cuda
                       FOLDER blt/tests )

    blt_add_test(NAME blt_cuda_smoke
                 COMMAND blt_cuda_smoke)

    blt_add_executable(NAME blt_cuda_runtime_smoke
                       SOURCES blt_cuda_runtime_smoke.cpp
                       OUTPUT_DIR ${TEST_OUTPUT_DIRECTORY} 
                       DEPENDS_ON cuda_runtime
                       FOLDER blt/tests )

    blt_add_test(NAME blt_cuda_runtime_smoke
                 COMMAND blt_cuda_runtime_smoke)

    blt_add_executable(NAME blt_cuda_version_smoke
                       SOURCES blt_cuda_version_smoke.cpp
                       OUTPUT_DIR ${TEST_OUTPUT_DIRECTORY} 
                       DEPENDS_ON cuda_runtime
                       FOLDER blt/tests )

    blt_add_test(NAME blt_cuda_version_smoke
                 COMMAND blt_cuda_version_smoke)
    if (ENABLE_OPENMP)
        blt_add_executable(NAME blt_cuda_openmp_smoke
                           SOURCES blt_cuda_openmp_smoke.cpp
                           OUTPUT_DIR ${TEST_OUTPUT_DIRECTORY} 
                           DEPENDS_ON cuda openmp
                           FOLDER blt/tests )

        blt_add_test(NAME blt_cuda_openmp_smoke
                     COMMAND blt_cuda_openmp_smoke)
    endif()

    if (ENABLE_MPI)
        blt_add_executable(NAME       blt_cuda_mpi_smoke
                           SOURCES    blt_cuda_mpi_smoke.cpp
                           OUTPUT_DIR ${TEST_OUTPUT_DIRECTORY}
                           DEPENDS_ON cuda mpi
                           FOLDER blt/tests )
        blt_add_test(NAME    blt_cuda_mpi_smoke
                     COMMAND blt_cuda_mpi_smoke
                     NUM_MPI_TASKS 4)
    endif()

    if(ENABLE_GTEST)
        blt_add_executable(NAME blt_cuda_gtest_smoke
                           SOURCES blt_cuda_gtest_smoke.cpp
                           OUTPUT_DIR ${TEST_OUTPUT_DIRECTORY}
                           DEPENDS_ON cuda gtest
                           FOLDER blt/tests )

        blt_add_test(NAME    blt_cuda_gtest_smoke
                     COMMAND blt_cuda_gtest_smoke)
    endif()
endif()


################
#  HIP test
################
if (ENABLE_HIP)
    blt_add_executable(NAME blt_hip_smoke
                       SOURCES blt_hip_smoke.cpp
                       OUTPUT_DIR ${TEST_OUTPUT_DIRECTORY}
                       DEPENDS_ON blt::hip
                       FOLDER blt/tests )

    blt_add_test(NAME blt_hip_smoke
                 COMMAND blt_hip_smoke)

    blt_add_executable(NAME blt_hip_runtime_smoke
                       SOURCES blt_hip_runtime_smoke.cpp
                       OUTPUT_DIR ${TEST_OUTPUT_DIRECTORY}
                       DEPENDS_ON blt::hip_runtime
                       FOLDER blt/tests )

    blt_add_test(NAME blt_hip_runtime_smoke
                 COMMAND blt_hip_runtime_smoke)

    blt_add_executable(NAME blt_hip_runtime_c_smoke
                       SOURCES blt_hip_runtime_c_smoke.c
                       OUTPUT_DIR ${TEST_OUTPUT_DIRECTORY}
                       DEPENDS_ON blt::hip_runtime
                       FOLDER blt/tests )

    blt_add_test(NAME blt_hip_runtime_c_smoke
                 COMMAND blt_hip_runtime_c_smoke)

    if(ENABLE_GTEST)
        blt_add_executable(NAME blt_hip_gtest_smoke
                           SOURCES blt_hip_gtest_smoke.cpp
                           OUTPUT_DIR ${TEST_OUTPUT_DIRECTORY}
                           DEPENDS_ON gtest blt::hip
                           FOLDER blt/tests )

        blt_add_test(NAME    blt_hip_gtest_smoke
                     COMMAND blt_hip_gtest_smoke)
    endif()

endif()
