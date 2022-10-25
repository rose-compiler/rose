// Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
// other BLT Project Developers. See the top-level LICENSE file for details
//
// SPDX-License-Identifier: (BSD-3-Clause)

#include "object.hpp"
#include "inherited_base.hpp"

#include <iostream>

int main()
{
  bool failed = false;

  int number = object_number();
  if(number != 6)
  {
    std::cerr << "Error:"
              << number
              << " was returned from all libraries."
              << std::endl
              << "6 was the correct number."
              << std::endl;
    failed = true;
  }
  else
  {
    std::cout << number
              << " was correctly returned from all libraries."
              << std::endl;
  }

  int iNumber = inherited_number();
  if(iNumber != 3)
  {
    std::cerr << "Error:"
              << iNumber
              << " was returned from the inherited library."
              << std::endl
              << "3 was the correct number."
              << std::endl;
    failed = true;
  }
  else
  {
    std::cout << iNumber
              << " was correctly returned from inherited library."
              << std::endl;
  }

  if (failed)
  {
    return 1;
  }
  return 0;
}
