// Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
// other BLT Project Developers. See the top-level LICENSE file for details
//
// SPDX-License-Identifier: (BSD-3-Clause)

#ifndef TESTS_INTERNAL_SRC_COMBINE_STATIC_LIBRARY_TEST_FOO2_HPP_
#define TESTS_INTERNAL_SRC_COMBINE_STATIC_LIBRARY_TEST_FOO2_HPP_

#include<string>

namespace blt_test
{

class Foo2
{
public:
  Foo2();
  ~Foo2();

  std::string output();

};

} /* namespace blt_test */

#endif /* TESTS_INTERNAL_SRC_COMBINE_STATIC_LIBRARY_TEST_FOO2_HPP_ */
