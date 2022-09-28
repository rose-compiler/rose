# Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
# other BLT Project Developers. See the top-level LICENSE file for details
# 
# SPDX-License-Identifier: (BSD-3-Clause)

########################################################################
# host-config for naples
########################################################################

########################################################################
# Dependencies were built with spack (https://github.com/llnl/spack)
########################################################################
# spack install  cmake@3.8.2
# spack install  mpich 
# spack install  py-sphinx
# spack activate py-sphinx
# spack install  doxygen


########################################################################
# cmake path
########################################################################
# /Users/harrison37/Work/blt_tutorial/tpls/spack/opt/spack/darwin-elcapitan-x86_64/clang-7.3.0-apple/cmake-3.8.2-n2i4ijlet37i3jhmjfhzms2wo3b4ybcm/bin/cmake

########################################################################
# mpi from spack
########################################################################
set(ENABLE_MPI ON CACHE PATH "")

set(MPI_BASE_DIR "/Users/harrison37/Work/blt_tutorial/tpls/spack/opt/spack/darwin-elcapitan-x86_64/clang-7.3.0-apple/mpich-3.2-yc7ipshe7e3w4ohtgjtms2agecxruavw/bin" CACHE PATH "")

set(MPI_C_COMPILER   "${MPI_BASE_DIR}/mpicc" CACHE PATH "")
set(MPI_CXX_COMPILER "${MPI_BASE_DIR}/mpicxx" CACHE PATH "")
set(MPIEXEC          "${MPI_BASE_DIR}/mpiexec" CACHE PATH "")

########################################################################
# Cuda Support (standard osx cuda toolkit install)
########################################################################
set(ENABLE_CUDA ON CACHE BOOL "")

set(CUDA_TOOLKIT_ROOT_DIR "/Developer/NVIDIA/CUDA-8.0/" CACHE PATH "")
set(CUDA_BIN_DIR          "/Developer/NVIDIA/CUDA-8.0/bin/" CACHE PATH "")

########################################################################
# sphinx from spack
########################################################################
set(SPHINX_EXECUTABLE "/Users/harrison37/Work/blt_tutorial/tpls/spack/opt/spack/darwin-elcapitan-x86_64/clang-7.3.0-apple/python-2.7.13-jmhznopgz2j5zkmuzjygg5oyxnxtc653/bin/sphinx-build" CACHE PATH "")

########################################################################
# doxygen from spack
########################################################################
set(DOXYGEN_EXECUTABLE "/Users/harrison37/Work/blt_tutorial/tpls/spack/opt/spack/darwin-elcapitan-x86_64/clang-7.3.0-apple/doxygen-1.8.12-mji43fu4hxuu6js5irshpihkwwucn7rv/bin/doxygen" CACHE PATH "")


