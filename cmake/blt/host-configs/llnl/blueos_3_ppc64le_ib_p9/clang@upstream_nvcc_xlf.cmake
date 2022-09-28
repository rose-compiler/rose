# Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
# other BLT Project Developers. See the top-level LICENSE file for details
# 
# SPDX-License-Identifier: (BSD-3-Clause)

#------------------------------------------------------------------------------
# Example host-config file for the blue_os cluster at LLNL
#------------------------------------------------------------------------------
#
# This file provides CMake with paths / details for:
#  C/C++:   Clang
#  Fortran: XLF
#  MPI
#  Cuda
# 
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Compilers
#------------------------------------------------------------------------------

# Use Clang compilers for C/C++
set(CLANG_VERSION "clang-upstream-2019.08.15" CACHE STRING "")
set(CLANG_HOME "/usr/tce/packages/clang/${CLANG_VERSION}")

set(CMAKE_C_COMPILER   "${CLANG_HOME}/bin/clang" CACHE PATH "")
set(CMAKE_CXX_COMPILER "${CLANG_HOME}/bin/clang++" CACHE PATH "")
set(BLT_CXX_STD "c++11" CACHE STRING "")

# Use XL compiler for Fortran
set(ENABLE_FORTRAN ON CACHE BOOL "")
set(XL_VERSION "xl-2019.06.12")
set(XL_HOME "/usr/tce/packages/xl/${XL_VERSION}")
set(CMAKE_Fortran_COMPILER "${XL_HOME}/bin/xlf2003" CACHE PATH "")

#------------------------------------------------------------------------------
# MPI Support
#------------------------------------------------------------------------------

set(ENABLE_MPI ON CACHE BOOL "")

set(MPI_HOME               "/usr/tce/packages/spectrum-mpi/spectrum-mpi-rolling-release-${CLANG_VERSION}")
set(MPI_Fortran_HOME       "/usr/tce/packages/spectrum-mpi/spectrum-mpi-rolling-release-${XL_VERSION}")

set(MPI_C_COMPILER         "${MPI_HOME}/bin/mpicc" CACHE PATH "")
set(MPI_CXX_COMPILER       "${MPI_HOME}/bin/mpicxx" CACHE PATH "")
set(MPI_Fortran_COMPILER   "${MPI_Fortran_HOME}/bin/mpif90" CACHE PATH "")

set(MPIEXEC                "${MPI_HOME}/bin/mpirun" CACHE PATH "")
set(MPIEXEC_NUMPROC_FLAG   "-np" CACHE PATH "")
set(BLT_MPI_COMMAND_APPEND "mpibind" CACHE PATH "")

#------------------------------------------------------------------------------
# Other machine specifics
#------------------------------------------------------------------------------

set(CMAKE_Fortran_COMPILER_ID "XL" CACHE PATH "All of BlueOS compilers report clang due to nvcc, override to proper compiler family")
set(BLT_FORTRAN_FLAGS "-WF,-C!" CACHE PATH "Converts C-style comments to Fortran style in preprocessed files")

#------------------------------------------------------------------------------
# CUDA support
#------------------------------------------------------------------------------
#_blt_tutorial_useful_cuda_flags_start
set(ENABLE_CUDA ON CACHE BOOL "")

set(CUDA_TOOLKIT_ROOT_DIR "/usr/tce/packages/cuda/cuda-11.1.1" CACHE PATH "")
set(CMAKE_CUDA_COMPILER "${CUDA_TOOLKIT_ROOT_DIR}/bin/nvcc" CACHE PATH "")
set(CMAKE_CUDA_HOST_COMPILER "${MPI_CXX_COMPILER}" CACHE PATH "")

set(CMAKE_CUDA_ARCHITECTURES "70" CACHE STRING "")
set(_cuda_arch "sm_${CMAKE_CUDA_ARCHITECTURES}")
set(CMAKE_CUDA_FLAGS "-restrict -arch ${_cuda_arch} --expt-extended-lambda -G" CACHE STRING "")

set(CUDA_SEPARABLE_COMPILATION ON CACHE BOOL "" )

# nvcc does not like gtest's 'pthreads' flag
set(gtest_disable_pthreads ON CACHE BOOL "")
#_blt_tutorial_useful_cuda_flags_end
