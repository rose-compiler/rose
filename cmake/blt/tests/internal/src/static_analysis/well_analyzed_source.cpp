// Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
// other BLT Project Developers. See the top-level LICENSE file for details
//
// SPDX-License-Identifier: (BSD-3-Clause)

/**
 * This file contains sample code for clang-query to match
 *
 * When a target which analyzes the source code for if statements is run,
 * the if statement should be highlighted.
 *
 * Some useful targets
 *
 * 1) "make filtered"
 *
 *    Will run a matcher "match if-stmt" and produce the output
 *
 *    [100%] Running specified clang_query source code static analysis checks.
 *
 *    Match #1:
 *
 *    /blt/root/dir/blt/tests/internal/src/static_analysis/well_analyzed_source.cpp:2:2: note: "root" binds here
 *     if(true){}
 *     ^~~~~~~~~~
 *    1 match.
 *
 * 2) "checker=interpreter make filtered"
 *
 *    Will open up the clang-query REPL (Interpreter) and allow you to write your own queries in the language
 *    described here, but which can mostly be learned through tab completion in that interpreter:
 *
 *    https://clang.llvm.org/docs/LibASTMatchersReference.html
 */
int main()
{
  if(true) {}
}
