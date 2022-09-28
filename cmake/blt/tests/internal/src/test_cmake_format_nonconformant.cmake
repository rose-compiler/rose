# Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and other
# BLT Project Developers. See the top-level LICENSE file for details
#
# SPDX-License-Identifier: (BSD-3-Clause)
# ------------------------------------------------------------------------------
# Example CMake file used to test cmake-format
# ------------------------------------------------------------------------------
macro(cmake_format_example)
  # Test for 80 column wrap
  set(greek_letters alpha beta gamme delta epsilon zeta eta theta iota kappa lambda mu nu xi omicron pi rho sigma tau upsilon phi chi psi omega)

  # Test for reflowing multi-line command to single line
  set(shopkeeper
    "the"
    "frogurt"
    "is"
    "also"
    "cursed")

  # Test that cmake-format doesn't break up a long string argument
  set(bird_physics_quote "A five ounce bird could not carry a one pound coconut.")
  
  # Indent conditionals
  if(first_condition)
  if(second_condition)
  # Condense blank lines below comment to single line



  # Reflow a long comment into multiple lines because it's extremely long -- longer than the line length limit.
  set(foo bar baz quux)

  # Reflow a mulitple
  # line short comment
  # to a single line
  endif()
  endif()  
endmacro()
