// Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
// other BLT Project Developers. See the top-level LICENSE file for details
//
// SPDX-License-Identifier: (BSD-3-Clause)

#ifndef TESTS_INTERNAL_SRC_COMBINE_STATIC_LIBRARY_TEST_FOO1_HPP_
#define TESTS_INTERNAL_SRC_COMBINE_STATIC_LIBRARY_TEST_FOO1_HPP_

#include<string>

namespace blt_test
{

class Foo1
{
public:
  Foo1();
  ~Foo1();

  std::string output();
};

} /* namespace blt_test */

#endif /* TESTS_INTERNAL_SRC_COMBINE_STATIC_LIBRARY_TEST_FOO1_HPP_ */
