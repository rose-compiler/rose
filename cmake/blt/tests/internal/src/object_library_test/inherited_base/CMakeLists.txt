# Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
# other BLT Project Developers. See the top-level LICENSE file for details
# 
# SPDX-License-Identifier: (BSD-3-Clause)

blt_add_library( NAME    inherited_base
                 SOURCES inherited_base.cpp
                 HEADERS inherited_base.hpp
                 OBJECT  TRUE)
target_include_directories(inherited_base PUBLIC ${CMAKE_CURRENT_LIST_DIR})
blt_add_target_definitions(TO                 inherited_base
                           TARGET_DEFINITIONS INHERITED_FLAG)
