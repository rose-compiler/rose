// Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
// other BLT Project Developers. See the top-level LICENSE file for details
//
// SPDX-License-Identifier: (BSD-3-Clause)

#include "gtest/gtest.h"
#include <iostream>
#include <string>
#include "Foo1.hpp"
#include "Foo2.hpp"
#include "Foo3.hpp"

using namespace blt_test;
//------------------------------------------------------------------------------
// Simple smoke test for gtest
//------------------------------------------------------------------------------
TEST(blt_combine_static_library_smoke_static, test)
{
  Foo1 foo1;
  Foo2 foo2;
  Foo3 foo3;

  EXPECT_TRUE( foo1.output() == "I am Foo #1" );
  EXPECT_TRUE( foo2.output() == "I am Foo #2" );
  EXPECT_TRUE( foo3.output() == "I am Foo #3" );
}


