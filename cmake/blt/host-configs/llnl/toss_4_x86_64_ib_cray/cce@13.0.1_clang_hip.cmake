# Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
# other BLT Project Developers. See the top-level LICENSE file for details
# 
# SPDX-License-Identifier: (BSD-3-Clause)

#------------------------------------------------------------------------------
# Example host-config file for the rznevada cluster at LLNL
#------------------------------------------------------------------------------
# This file provides CMake with paths / details for:
#  C,C++, & Fortran compilers + MPI & HIP
# using tce wrappers, rather than HPE Cray PE compiler drivers
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# HPE Cray cce@13.0.1 compilers
#------------------------------------------------------------------------------
# _blt_tutorial_compiler_config_start
set(CCE_HOME "/usr/tce/packages/cce-tce/cce-13.0.1")
set(CMAKE_C_COMPILER   "${CCE_HOME}/bin/craycc" CACHE PATH "")
set(CMAKE_CXX_COMPILER "${CCE_HOME}/bin/crayCC" CACHE PATH "")

# Fortran support
set(ENABLE_FORTRAN ON CACHE BOOL "")
set(CMAKE_Fortran_COMPILER "${CCE_HOME}/bin/crayftn" CACHE PATH "")
# _blt_tutorial_compiler_config_end

#------------------------------------------------------------------------------
# MPI Support
#------------------------------------------------------------------------------
# _blt_tutorial_mpi_config_start
set(ENABLE_MPI ON CACHE BOOL "")

set(MPI_HOME "/usr/tce/packages/cray-mpich-tce/cray-mpich-8.1.13-cce-13.0.1/")
set(MPI_C_COMPILER "${MPI_HOME}/bin/mpicc" CACHE PATH "")

set(MPI_CXX_COMPILER "${MPI_HOME}/bin/mpicxx" CACHE PATH "")

set(MPI_Fortran_COMPILER "${MPI_HOME}/bin/mpif90" CACHE PATH "")
# _blt_tutorial_mpi_config_end

#------------------------------------------------------------------------------
# HIP support
#------------------------------------------------------------------------------
# _blt_tutorial_hip_config_start
set(ENABLE_HIP ON CACHE BOOL "")

set(ROCM_PATH "/opt/rocm-4.5.2/" CACHE PATH "")
set(CMAKE_HIP_ARCHITECTURES "gfx908" CACHE STRING "gfx architecture to use when generating HIP/ROCm code")

# Recommended link line when not using tce-wrapped compilers
# set(CMAKE_EXE_LINKER_FLAGS "-Wl,--disable-new-dtags -L/opt/rocm-4.5.2/hip/lib -L/opt/rocm-4.5.2/lib -L/opt/rocm-4.5.2/lib64 -Wl,-rpath,/opt/rocm-4.5.2/hip/lib:/opt/rocm-4.5.2/lib:/opt/rocm-4.5.2/lib64 -lamdhip64 -lhsakmt -lhsa-runtime64 -lamd_comgr" CACHE STRING "")

# _blt_tutorial_hip_config_end

