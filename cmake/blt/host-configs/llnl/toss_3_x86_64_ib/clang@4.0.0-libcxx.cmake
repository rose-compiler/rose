# Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
# other BLT Project Developers. See the top-level LICENSE file for details
# 
# SPDX-License-Identifier: (BSD-3-Clause)

#------------------------------------------------------------------------------
# Example host-config file for the quartz cluster at LLNL
#------------------------------------------------------------------------------
#
# This file provides CMake with paths / details for:
#  C,C++, & Fortran compilers + MPI
#------------------------------------------------------------------------------
# clang-4.0.0 / gfortran@4.9.3 compilers
# Uses clang's 'libc++' instead of 'libstdc++'

set(CLANG_HOME "/usr/tce/packages/clang/clang-4.0.0")
set(GNU_HOME "/usr/tce/packages/gcc/gcc-4.9.3")

# c compiler
set(CMAKE_C_COMPILER "${CLANG_HOME}/bin/clang" CACHE PATH "")

# cpp compiler
set(CMAKE_CXX_COMPILER "${CLANG_HOME}/bin/clang++" CACHE PATH "")

# fortran support
set(ENABLE_FORTRAN ON CACHE BOOL "")

# fortran compiler
set(CMAKE_Fortran_COMPILER "${GNU_HOME}/bin/gfortran" CACHE PATH "")

#------------------------------------------------------------------------------
# Extra flags
#------------------------------------------------------------------------------

# Use clang's libc++ instead of libstdc++
set(BLT_CXX_FLAGS "-stdlib=libc++" CACHE STRING "")
set(gtest_defines "-DGTEST_HAS_CXXABI_H_=0" CACHE STRING "")

#------------------------------------------------------------------------------
# MPI Support
#------------------------------------------------------------------------------
set(ENABLE_MPI ON CACHE BOOL "")

set(MPI_HOME             "/usr/tce/packages/mvapich2/mvapich2-2.3-clang-4.0.0" CACHE PATH "")
set(MPI_C_COMPILER       "${MPI_HOME}/bin/mpicc"   CACHE PATH "")
set(MPI_CXX_COMPILER     "${MPI_HOME}/bin/mpicxx"  CACHE PATH "")
set(MPI_Fortran_COMPILER "${MPI_HOME}/bin/mpifort" CACHE PATH "")

set(MPIEXEC              "/usr/bin/srun" CACHE PATH "")
set(MPIEXEC_NUMPROC_FLAG "-n" CACHE PATH "")

