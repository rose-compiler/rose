// Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
// other BLT Project Developers. See the top-level LICENSE file for details
//
// SPDX-License-Identifier: (BSD-3-Clause)

#include "gtest/gtest.h"
#include <iostream>


//------------------------------------------------------------------------------
// Simple smoke test for gtest
//------------------------------------------------------------------------------
TEST(blt_gtest_smoke,basic_assert_example)
{
  EXPECT_TRUE( true );
}


//------------------------------------------------------------------------------
// Tests the gtest death test feature, which are disabled by default.
//
// Exits program with non-zero exit code, passing the test.
// Note: To enable death tests, configure BLT with ENABLE_GTEST_DEATH_TESTS.
//------------------------------------------------------------------------------
TEST(blt_gtest_smoke,death_test)
{
  #if (GTEST_HAS_DEATH_TEST == 0)
  std::cout <<"Note: Death tests disabled in this configuration." << std::endl;
  #endif

  // Invoke death test function whether death tests enabled or disabled.
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";
  EXPECT_DEATH_IF_SUPPORTED( exit(1), "");
}

