# Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
# other BLT Project Developers. See the top-level LICENSE file for details
# 
# SPDX-License-Identifier: (BSD-3-Clause)

###############################################################################
#
# Simple test that uses the blt_add_library macro with object libraries
#
###############################################################################

add_subdirectory(inherited_base)

blt_add_library( NAME       base_object
                 SOURCES    base_object.cpp
                 HEADERS    base_object.hpp
                 DEPENDS_ON inherited_base
                 OBJECT     TRUE)

blt_add_library( NAME       object
                 SOURCES    object.cpp
                 HEADERS    object.hpp
                 DEPENDS_ON base_object
                 OBJECT     TRUE)

blt_add_executable( NAME       blt_test_object_libraries
                    SOURCES    main.cpp
                    DEPENDS_ON object)
blt_add_test( NAME    blt_test_object_libraries
              COMMAND blt_test_object_libraries)