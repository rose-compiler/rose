// Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
// other BLT Project Developers. See the top-level LICENSE file for details
//
// SPDX-License-Identifier: (BSD-3-Clause)

#include "base_object.hpp"
#include "inherited_base.hpp"

int base_number()
{
  #ifdef INHERITED_FLAG
  return inherited_number() + 2;
  #else
  // should not get here
  return 0;
  #endif
}
