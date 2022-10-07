// Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
// other BLT Project Developers. See the top-level LICENSE file for details
//
// SPDX-License-Identifier: (BSD-3-Clause)

#include <omp.h>
#include <iostream>

/**
 * Simple test program that uses OpenMP without guards.  Should only be compiled when OpenMP is enabled.
 */
int main()
{

  #pragma omp parallel
  {
    int thId = omp_get_thread_num();
    int thNum = omp_get_num_threads();
    int thMax = omp_get_max_threads();

    #pragma omp critical
    std::cout <<"\nMy thread id is: " << thId
              <<"\nNum threads is: " << thNum
              <<"\nMax threads is: " << thMax
              << std::endl;
  }

  return 0;
}

