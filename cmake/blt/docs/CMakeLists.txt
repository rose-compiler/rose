# Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
# other BLT Project Developers. See the top-level LICENSE file for details
# 
# SPDX-License-Identifier: (BSD-3-Clause)

cmake_minimum_required(VERSION 3.8)

project( blt_docs )

#------------------------------------------------------------------------------
# Setup BLT
#------------------------------------------------------------------------------

# Set BLT_SOURCE_DIR to default location, if not set by user 
if(NOT BLT_SOURCE_DIR)
    set(BLT_SOURCE_DIR "${PROJECT_SOURCE_DIR}/..")
endif()

include(${BLT_SOURCE_DIR}/SetupBLT.cmake)

#------------------------------------------------------------------------------
# Calc PI Tutorial
#------------------------------------------------------------------------------
add_subdirectory(tutorial/calc_pi)

#------------------------------------------------------------------------------
# Docs
#------------------------------------------------------------------------------

if (SPHINX_FOUND)
    blt_add_sphinx_target(blt_sphinx_tutorial_docs)
endif()
