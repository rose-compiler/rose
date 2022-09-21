# Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
# other BLT Project Developers. See the top-level LICENSE file for details
# 
# SPDX-License-Identifier: (BSD-3-Clause)

#------------------------------------------------------------------------------
# Example host-config file for 'sqa-uno` windows machine at LLNL
#------------------------------------------------------------------------------
#
# This file provides CMake with paths / details for:
# The Microsoft Visual Studio 15 compiler + MPI
#
# Run the following from a build dir for a 32-bit configuration
#   cmake -G "Visual Studio 15 2017"                         \
#         -C ..\host-configs\llnl-sqa-uno-windows-msvc@15.cmake  \
#         <path-to-blt-project>
#
# Run the following from a build dir for a 64-bit configuration
#   cmake -G "Visual Studio 15 2017 Win64"                   \
#         -C ..\host-configs\llnl-sqa-uno-windows-msvc@15.cmake  \
#         <path-to-blt-project>
#
# Build the code from the command line as follows 
# (use -m for parallel build in msbuild, if desired):
#   cmake --build . --config {Release,Debug,RelWithDebInfo} [-- /m:8]
#
# Test the code as follows (use -j for parallel testing):
#   ctest -j8 -C {Release,Debug,RelWithDebInfo}
# 
# Install the code from the command line as follows:
#   cmake --build . --config {Release,Debug,RelWithDebInfo} --target install
#
#------------------------------------------------------------------------------

# Set the HOME variable (%USERPROFILE% in Windows)
string(REPLACE "\\" "/" HOME "$ENV{USERPROFILE}")

# Setup MPI -- assumes MPI is installed in default location
set(ENABLE_MPI ON CACHE BOOL "")
set(MPI_HOME  "C:/Program Files/Microsoft HPC Pack 2008 R2" CACHE PATH "")
set(MPI_GUESS_LIBRARY_NAME "MSMPI" CACHE STRING "")

### Set some additional options
set(ENABLE_FOLDERS ON CACHE BOOL "")
set(ENABLE_GTEST_DEATH_TESTS ON CACHE BOOL "")
