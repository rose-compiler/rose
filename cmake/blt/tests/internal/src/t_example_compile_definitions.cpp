// Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
// other BLT Project Developers. See the top-level LICENSE file for details
//
// SPDX-License-Identifier: (BSD-3-Clause)

#include "gtest/gtest.h"

//------------------------------------------------------------------------------

// Simple test that expects symbol BLT_A to be defined as a non-zero number
TEST(blt_compile_definitions,check_BLT_A_defined)
{
  #if BLT_A
  SUCCEED();
  #else
  FAIL() << "Compiler define A was not defined as a non-zero number";
  #endif
}

// Simple test that expects symbol BLT_B to be defined
TEST(blt_compile_definitions,check_BLT_B_defined)
{
  #ifdef BLT_B
  SUCCEED();
  #else
  FAIL() << "Compiler define B was not defined";
  #endif
}
