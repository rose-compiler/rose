# Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
# other BLT Project Developers. See the top-level LICENSE file for details
# 
# SPDX-License-Identifier: (BSD-3-Clause)

#------------------------------------------------------------------------------
# Example host-config file for the blue_os cluster at LLNL, specifically Lassen
#------------------------------------------------------------------------------
#
# This file provides CMake with paths / details for:
#  C/C++:   Clang with GCC 8.3.1 toolchain
#  Cuda
#  MPI
# 
#------------------------------------------------------------------------------

#---------------------------------------
# Compilers
#---------------------------------------

set(_CLANG_VERSION "clang-upstream-2019.08.15")
set(_CLANG_DIR "/usr/tce/packages/clang/${_CLANG_VERSION}")
set(_GCC_DIR "/usr/tce/packages/gcc/gcc-8.3.1")

set(CMAKE_C_COMPILER "${_CLANG_DIR}/bin/clang" CACHE PATH "")
set(CMAKE_CXX_COMPILER "${_CLANG_DIR}/bin/clang++" CACHE PATH "")

set(BLT_CXX_STD "c++14" CACHE STRING "")

set(CMAKE_C_FLAGS "--gcc-toolchain=${_GCC_DIR}" CACHE PATH "")
set(CMAKE_CXX_FLAGS "--gcc-toolchain=${_GCC_DIR}" CACHE PATH "")

set(BLT_EXE_LINKER_FLAGS " -Wl,-rpath,${_GCC_DIR}/lib" CACHE PATH "Adds a missing libstdc++ rpath")

#---------------------------------------
# MPI
#---------------------------------------
set(ENABLE_MPI ON CACHE BOOL "")

set(_MPI_BASE_DIR "/usr/tce/packages/spectrum-mpi/spectrum-mpi-rolling-release-${_CLANG_VERSION}")

set(MPI_C_COMPILER "${_MPI_BASE_DIR}/bin/mpicc" CACHE PATH "")
set(MPI_CXX_COMPILER "${_MPI_BASE_DIR}/bin/mpicxx" CACHE PATH "")

#------------------------------------------------------------------------------
# Cuda
#------------------------------------------------------------------------------

set(ENABLE_CUDA ON CACHE BOOL "")

set(CUDA_TOOLKIT_ROOT_DIR "/usr/tce/packages/cuda/cuda-11.1.1" CACHE PATH "")

set(CMAKE_CUDA_COMPILER "${CUDA_TOOLKIT_ROOT_DIR}/bin/nvcc" CACHE PATH "")
set(CMAKE_CUDA_HOST_COMPILER "${CMAKE_CXX_COMPILER}" CACHE PATH "")

set(CMAKE_CUDA_ARCHITECTURES "70" CACHE STRING "")
set(_cuda_arch "sm_${CMAKE_CUDA_ARCHITECTURES}")
set(CMAKE_CUDA_FLAGS "-Xcompiler=--gcc-toolchain=${_GCC_DIR} -restrict -arch ${_cuda_arch} -std=${BLT_CXX_STD} --expt-extended-lambda -G" CACHE STRING "")

set(CUDA_SEPARABLE_COMPILATION ON CACHE BOOL "" )

# nvcc does not like gtest's 'pthreads' flag
set(gtest_disable_pthreads ON CACHE BOOL "")
set(ENABLE_GTEST_DEATH_TESTS OFF CACHE BOOL "")

# Very specific fix for working around CMake adding implicit link directories returned by the BlueOS
# compilers to link CUDA executables 
set(BLT_CMAKE_IMPLICIT_LINK_DIRECTORIES_EXCLUDE "/usr/tce/packages/gcc/gcc-4.9.3/lib64/gcc/powerpc64le-unknown-linux-gnu/4.9.3;/usr/tce/packages/gcc/gcc-4.9.3/lib64" CACHE STRING "")

#------------------------------------------------------------------------------
# Enable TPL export
#------------------------------------------------------------------------------
set(BLT_EXPORT_THIRDPARTY ON CACHE BOOL "")