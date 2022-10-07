// Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
// other BLT Project Developers. See the top-level LICENSE file for details
//
// SPDX-License-Identifier: (BSD-3-Clause)

#ifndef __Parent_h
#define __Parent_h

class Parent
{
public:

  Parent** m_gpuParent;
  Parent*** m_gpuExtractedParents;

  __host__ __device__ Parent(const char *id, int order);

  __host__ __device__ virtual double Evaluate(const double * args) const = 0;


};

#endif
