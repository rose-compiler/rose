// Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
// other BLT Project Developers. See the top-level LICENSE file for details
//
// SPDX-License-Identifier: (BSD-3-Clause)

#ifndef TESTS_INTERNAL_SRC_COMBINE_STATIC_LIBRARY_TEST_FOO3_HPP_
#define TESTS_INTERNAL_SRC_COMBINE_STATIC_LIBRARY_TEST_FOO3_HPP_

#include<string>

namespace blt_test
{

class Foo3
{
public:
  Foo3();
  ~Foo3();

  std::string output();

};

} /* namespace blt_test */

#endif /* TESTS_INTERNAL_SRC_COMBINE_STATIC_LIBRARY_TEST_FOO3_HPP_ */
