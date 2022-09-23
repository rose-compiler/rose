// Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
// other BLT Project Developers. See the top-level LICENSE file for details
//
// SPDX-License-Identifier: (BSD-3-Clause)

/**
 * This file contains a subtle C++ error that results in a runtime crash.
 * 
 * The "subtle_error" target ("make subtle_error") should behave incorrectly when run.
 * 
 * The "check_for_subtle_error" target ("make check_for_subtle_error")
 * will run the clang-tidy static analyzer and report the error via stdout.
 * 
 * See https://clang.llvm.org/extra/clang-tidy/checks/list.html
 * for the full list of available checks
 */

#include <iostream>

int main()
{
  int array[1] = {5};
  int first_element = array[1];
  std::cout << first_element << "\n";
}
