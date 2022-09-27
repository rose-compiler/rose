# Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
# other BLT Project Developers. See the top-level LICENSE file for details
# 
# SPDX-License-Identifier: (BSD-3-Clause)

# The CMake command line to use this host config file must have certain
# defines enabled.  Here is an example script for an out-of-place build of the
# tutorial blank project template:
## hip.sh
##!/bin/bash
#rm -rf build-hip-release 2>/dev/null
#mkdir build-hip-release && cd build-hip-release

#BLT_DIR=$(git rev-parse --show-toplevel)

#cmake \
#  -DCMAKE_BUILD_TYPE=Release \
#  -DENABLE_HIP=ON -DBLT_SOURCE_DIR=${BLT_DIR} \
#  -C ${BLT_DIR}/host-configs/hip.cmake \
#  -DCMAKE_INSTALL_PREFIX=../install-hip-release \
#  "$@" \
#  ${BLT_DIR}/docs/tutorial/blank_project


###########################################################
# standard ROCm HIP compiler
###########################################################

set(ENABLE_HIP ON CACHE BOOL "")
set(ENABLE_OPENMP OFF CACHE BOOL "")

set(HIP_ROOT_DIR "/opt/rocm" CACHE PATH "HIP ROOT directory path")
