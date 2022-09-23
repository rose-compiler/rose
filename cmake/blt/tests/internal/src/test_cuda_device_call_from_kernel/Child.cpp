// Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
// other BLT Project Developers. See the top-level LICENSE file for details
//
// SPDX-License-Identifier: (BSD-3-Clause)

#include <math.h>
#include "Child.hpp"

__global__ void kernelCreateChild(Parent **myGpuParent, 
                                  double a, double b,
                                  double c, double d)
{
  *myGpuParent = new Child(a, b, c, d);
}

__host__ __device__ Child::Child(double a, double b, double c, double d)
  : Parent("", 0)
  , m_a(a)
  , m_b(b)
  , m_c(c)
  , m_d(d)
{
  #ifndef __CUDA_ARCH__
  cudaMalloc(&(m_gpuParent), sizeof(Parent **));

  kernelCreateChild<<<1,1>>>(m_gpuParent, a, b, c, d);
  cudaDeviceSynchronize();
  #endif
}

__host__ __device__ Child::~Child()
{
}

__host__ __device__ 
double Child::Evaluate(const double ain, const double bin,
                       const double cin, const double din) const
{
  double bb, cc;
  double e = 0.0;

  e = (din-m_d);
  e = (ain-m_a);
  e = (bin-m_b);
  e = (cin-m_c);
  bb = (bin-m_b);
  cc = (cin-m_c);
  e = sqrt(bb*bb+cc*cc);

  return e;
}
