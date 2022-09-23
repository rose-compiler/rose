// Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
// other BLT Project Developers. See the top-level LICENSE file for details
//
// SPDX-License-Identifier: (BSD-3-Clause)

#include "base_object.hpp"
#include "object.hpp"

int object_number()
{
  return 1 + base_number();
}
