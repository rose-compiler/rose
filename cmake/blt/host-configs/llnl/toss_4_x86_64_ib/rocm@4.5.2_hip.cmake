# Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
# other BLT Project Developers. See the top-level LICENSE file for details
# 
# SPDX-License-Identifier: (BSD-3-Clause)

#------------------------------------------------------------------------------
# Example host-config file for the corona cluster at LLNL
#------------------------------------------------------------------------------
# This file provides CMake with paths / details for:
#  C,C++, & Fortran compilers + MPI & HIP
# using ROCM compilers
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# ROCM@4.5.2 compilers
#------------------------------------------------------------------------------
set(ROCM_HOME "/usr/tce/packages/rocmcc-tce/rocmcc-4.5.2")
set(CMAKE_C_COMPILER   "${ROCM_HOME}/bin/amdclang" CACHE PATH "")
set(CMAKE_CXX_COMPILER "${ROCM_HOME}/bin/hipcc" CACHE PATH "")

# Fortran support
set(ENABLE_FORTRAN ON CACHE BOOL "")
set(CMAKE_Fortran_COMPILER "${ROCM_HOME}/bin/amdflang" CACHE PATH "")

#------------------------------------------------------------------------------
# MPI Support
#------------------------------------------------------------------------------
set(ENABLE_MPI ON CACHE BOOL "")

set(MPI_HOME "/usr/tce/packages/mvapich2-tce/mvapich2-2.3.6-rocmcc-4.5.2/")
set(MPI_C_COMPILER "${MPI_HOME}/bin/mpicc" CACHE PATH "")
set(MPI_CXX_COMPILER "${MPI_HOME}/bin/mpicxx" CACHE PATH "")
set(MPI_Fortran_COMPILER "${MPI_HOME}/bin/mpif90" CACHE PATH "")

#------------------------------------------------------------------------------
# HIP support
#------------------------------------------------------------------------------
set(ENABLE_HIP ON CACHE BOOL "")
set(ROCM_PATH "/opt/rocm-4.5.2/" CACHE PATH "")
set(CMAKE_HIP_ARCHITECTURES "gfx906" CACHE STRING "")

# Recommended link line when not using tce-wrapped compilers
# set(CMAKE_EXE_LINKER_FLAGS "-Wl,--disable-new-dtags -L/opt/rocm-4.5.2/hip/lib -L/opt/rocm-4.5.2/lib -L/opt/rocm-4.5.2/lib64 -Wl,-rpath,/opt/rocm-4.5.2/hip/lib:/opt/rocm-4.5.2/lib:/opt/rocm-4.5.2/lib64 -lamdhip64 -lhsakmt -lhsa-runtime64 -lamd_comgr" CACHE STRING "")
