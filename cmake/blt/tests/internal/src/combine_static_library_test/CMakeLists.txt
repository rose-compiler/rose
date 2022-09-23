# Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
# other BLT Project Developers. See the top-level LICENSE file for details
# 
# SPDX-License-Identifier: (BSD-3-Clause)

###############################################################################
#
# Simple example that uses the blt_combine_static_libraries macro
#
###############################################################################

blt_add_library( NAME Foo1
                 SOURCES Foo1.cpp
                 HEADERS Foo1.hpp
                 SHARED FALSE
               )

blt_add_library( NAME Foo23
                 SOURCES Foo2.cpp Foo3.cpp
                 HEADERS Foo2.hpp Foo3.hpp
                 SHARED FALSE
               )

blt_combine_static_libraries( NAME FooStatic
                       SOURCE_LIBS Foo1 Foo23
                       LIB_TYPE STATIC
                       LINK_PREPEND ""
                       LINK_POSTPEND ""
                     )

blt_add_executable( NAME blt_combine_static_libraries_static_smoke
                    SOURCES blt_combine_static_libraries_static_smoke.cpp
                    DEPENDS_ON FooStatic gtest
                  )
blt_add_test( NAME blt_combine_static_libraries_static_smoke
              COMMAND blt_combine_static_libraries_static_smoke
            )

if(NOT WIN32)
    blt_combine_static_libraries( NAME FooShared
                       SOURCE_LIBS Foo1 Foo23
                       LIB_TYPE SHARED
                       LINK_PREPEND ""
                       LINK_POSTPEND ""
                     )

    blt_add_executable( NAME blt_combine_static_libraries_shared_smoke
                    SOURCES blt_combine_static_libraries_shared_smoke.cpp
                    DEPENDS_ON FooShared gtest
                  )

    blt_add_test( NAME blt_combine_static_libraries_shared_smoke
              COMMAND blt_combine_static_libraries_shared_smoke
            )
endif()
