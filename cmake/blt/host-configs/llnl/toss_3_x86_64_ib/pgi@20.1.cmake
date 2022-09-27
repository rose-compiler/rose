# Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
# other BLT Project Developers. See the top-level LICENSE file for details
# 
# SPDX-License-Identifier: (BSD-3-Clause)

#------------------------------------------------------------------------------
# Example pgi@20.1 host-config for LLNL toss3 machines
#------------------------------------------------------------------------------

set(COMPILER_HOME "/usr/tce/packages/pgi/pgi-20.1")

# c compiler
set(CMAKE_C_COMPILER "${COMPILER_HOME}/bin/pgcc" CACHE PATH "")

# cpp compiler
set(CMAKE_CXX_COMPILER "${COMPILER_HOME}/bin/pgc++" CACHE PATH "")

# fortran support
set(ENABLE_FORTRAN ON CACHE BOOL "")

# fortran support
set(CMAKE_Fortran_COMPILER "${COMPILER_HOME}/bin/pgfortran" CACHE PATH "")

#------------------------------------------------------------------------------
# Extra options and flags
#------------------------------------------------------------------------------

set(ENABLE_OPENMP ON CACHE BOOL "")


#------------------------------------------------------------------------------
# MPI Support
#------------------------------------------------------------------------------

set(ENABLE_MPI ON CACHE BOOL "")

set(MPI_HOME             "/usr/tce/packages/mvapich2/mvapich2-2.3-pgi-20.1")

set(MPI_C_COMPILER       "${MPI_HOME}/bin/mpicc" CACHE PATH "")
set(MPI_CXX_COMPILER     "${MPI_HOME}/bin/mpicxx" CACHE PATH "")
set(MPI_Fortran_COMPILER "${MPI_HOME}/bin/mpif90" CACHE PATH "")
set(MPIEXEC              "/usr/bin/srun" CACHE PATH "")
set(MPIEXEC_NUMPROC_FLAG "-n" CACHE PATH "")
