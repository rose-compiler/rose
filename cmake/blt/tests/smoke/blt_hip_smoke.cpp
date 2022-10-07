// Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
// other BLT Project Developers. See the top-level LICENSE file for details
//
// SPDX-License-Identifier: (BSD-3-Clause)

//-----------------------------------------------------------------------------
//
// file: blt_hip_smoke.cpp
//
//-----------------------------------------------------------------------------

#include <iostream>
#include <stdio.h>
#include "hip/hip_runtime.h"

__device__ const char STR[] = "HELLO WORLD!";
const char STR_LENGTH = 12;

__global__ void hello()
{
  printf("%c\n", STR[threadIdx.x % STR_LENGTH]);
}

int main()
{
  int num_threads = STR_LENGTH;
  int num_blocks = 1;
  hipLaunchKernelGGL((hello), dim3(num_blocks), dim3(num_threads),0,0);
  if(hipSuccess != hipDeviceSynchronize())
  {
    std::cout << "ERROR: hipDeviceSynchronize failed!" << std::endl;
  }

  return 0;
}


