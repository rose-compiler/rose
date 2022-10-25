// Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
// other BLT Project Developers. See the top-level LICENSE file for details
//
// SPDX-License-Identifier: (BSD-3-Clause)

#ifndef __Child_h
#define __Child_h

#include "Parent.hpp"

class Child : public Parent
{
public:
  __host__ __device__ Child(double a, double b, double c, double d);

  __host__ __device__ virtual ~Child() ;

  __host__ __device__ double Evaluate(const double ain, const double bin,
                                      const double cin, const double din) const;

  __host__ __device__ inline double Evaluate(const double *args) const 
  { 
    return Evaluate(args[0], args[1], args[2], args[3]) ; 
  }

private:
  double m_a, m_b, m_c, m_d;
};

#endif
