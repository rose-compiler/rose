# Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
# other BLT Project Developers. See the top-level LICENSE file for details
# 
# SPDX-License-Identifier: (BSD-3-Clause)

#------------------------------------------------------------------------------
# Test CUDA virtual device method call from kernels across library boundary
# CUDA needs to be enabled, with appropriate Flags set up for building and
# running on your current GPU architecture. 
#------------------------------------------------------------------------------
set(t_cuda_device_call_from_kernel_headers
    Child.hpp
    Parent.hpp)

set(t_cuda_device_call_from_kernel_sources
    Child.cpp
    Parent.cpp)

blt_add_library(NAME       t_cuda_device_call_from_kernel_lib
                SOURCES    ${t_cuda_device_call_from_kernel_sources}
                HEADERS    ${t_cuda_device_call_from_kernel_headers}
                DEPENDS_ON cuda)

set(t_cuda_device_call_from_kernel_exec_src
    CudaTests.cpp)

blt_add_executable( NAME       t_cuda_device_call_from_kernel_exec
                    SOURCES    ${t_cuda_device_call_from_kernel_exec_src}
                    DEPENDS_ON t_cuda_device_call_from_kernel_lib gtest cuda)

blt_add_test(NAME    t_cuda_device_call_from_kernel
             COMMAND t_cuda_device_call_from_kernel_exec)

