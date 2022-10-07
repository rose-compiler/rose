# Copyright (c) 2017-2021, Lawrence Livermore National Security, LLC and
# other BLT Project Developers. See the top-level LICENSE file for details
# 
# SPDX-License-Identifier: (BSD-3-Clause)

#------------------------------------------------------------------------------
# Example host-config file for the quartz cluster at LLNL
#------------------------------------------------------------------------------
#
# This file provides CMake with paths / details for:
#  C,C++, & Fortran compilers + MPI
# 
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# intel-oneapi.2021.3 compilers
#------------------------------------------------------------------------------

set(INTELLLVM_VERSION "intel-oneapi.2021.3")
set(INTELLLVM_HOME "/usr/tce/packages/intel/${INTELLLVM_VERSION}")

# c compiler
set(CMAKE_C_COMPILER "${INTELLLVM_HOME}/bin/icx" CACHE PATH "")

# cpp compiler
set(CMAKE_CXX_COMPILER "${INTELLLVM_HOME}/bin/icpx" CACHE PATH "")

# fortran support
set(ENABLE_FORTRAN ON CACHE BOOL "")

# fortran compiler
set(CMAKE_Fortran_COMPILER "${INTELLLVM_HOME}/bin/ifx" CACHE PATH "")

#------------------------------------------------------------------------------
# Extra flags
#------------------------------------------------------------------------------

set(BLT_CXX_STD "c++17")

# Use non-system default stdlib
set(GCC_VERSION "gcc-9.3.1")
# Note: gcc-8.3.1 and above need the "/rh" directory. Below that version do not.
set(GCC_HOME "/usr/tce/packages/gcc/${GCC_VERSION}/rh")

set(BLT_C_FLAGS "--gcc-toolchain=${GCC_HOME}" CACHE STRING "")
set(BLT_CXX_FLAGS "--gcc-toolchain=${GCC_HOME}" CACHE STRING "")
set(BLT_FORTRAN_FLAGS "-gcc-name=${GCC_HOME}/bin/gcc" CACHE STRING "")

#------------------------------------------------------------------------------
# MPI Support
#------------------------------------------------------------------------------
set(ENABLE_MPI ON CACHE BOOL "")

set(MPI_HOME             "/usr/tce/packages/mvapich2/mvapich2-2.3.6-${INTELLLVM_VERSION}" CACHE PATH "")

set(MPI_C_COMPILER       "${MPI_HOME}/bin/mpicc" CACHE PATH "")
set(MPI_CXX_COMPILER     "${MPI_HOME}/bin/mpicxx" CACHE PATH "")
set(MPI_Fortran_COMPILER "${MPI_HOME}/bin/mpif90" CACHE PATH "")

set(MPIEXEC              "/usr/bin/srun" CACHE PATH "")
set(MPIEXEC_NUMPROC_FLAG "-n" CACHE PATH "")
