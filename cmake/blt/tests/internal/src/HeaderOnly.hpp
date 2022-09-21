// Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
// other BLT Project Developers. See the top-level LICENSE file for details
//
// SPDX-License-Identifier: (BSD-3-Clause)

#ifndef BLT_HEADER_ONLY_HPP
#define BLT_HEADER_ONLY_HPP

#include "Example.hpp"

#ifndef BLT_EXAMPLE_LIB
  #error Compile definitions were not propagated from "example" library
#endif

namespace blt
{

inline bool ReturnTrue()
{
  Example e;
  return e.ReturnTrue();
}

} // end of namespace blt

#endif
