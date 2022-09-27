// Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
// other BLT Project Developers. See the top-level LICENSE file for details
//
// SPDX-License-Identifier: (BSD-3-Clause)

#include <iostream>
#include "Foo1.hpp"
#include "Foo2.hpp"

using namespace blt_test;
int main( int, char **  )
{
  Foo1 foo1;
  Foo2 foo2;
  Foo3 foo3;

  std::cout<<foo1.output()<<std::endl;
  std::cout<<foo2.output()<<std::endl;
  std::cout<<foo3.output()<<std::endl;

  return 0;
}
