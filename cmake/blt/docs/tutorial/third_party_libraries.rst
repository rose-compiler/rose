.. # Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
.. # other BLT Project Developers. See the top-level LICENSE file for details
.. # 
.. # SPDX-License-Identifier: (BSD-3-Clause)

.. _ThirdPartyLibraries:

Third Party Libraries
=====================

Third party libraries come in three flavors based on the CMake support provided by 
the project and the CMake community as a whole: no Cmake support, CMake's Find* modules,
and First Class project support.   

No CMake Support
~~~~~~~~~~~~~~~~

Some libraries have no support for easily importing their CMake targets into
external projects, either through properly exporting their targets themselves
or the CMake community has not written a Find module that eases this work.

BLT provides a :ref:`blt_import_library` macro allows you to reuse all information needed
for an external dependency under a single name.  This includes any include
directories, libraries, compile flags, link flags, defines, etc.  You can also
hide any warnings created by their headers by setting the
``TREAT_INCLUDES_AS_SYSTEM`` argument.

We will use Lua as an example of this because up until recently (CMake version 3.18),
there was no Find module.  This provides us a great example on how to show two ways
of importing the same library's targets.

The following example shows how to find a library, Lua this time, manually.  By first, 
searching for the include directories and then for the library itself.  Finally it calls
:ref:`blt_import_library` to bundle that information under one easy to remember name, ``lua`` :

.. code-block:: cmake

    # first Check for LUA_DIR
    if (NOT EXISTS "${LUA_DIR}")
        message(FATAL_ERROR "Given LUA_DIR does not exist: ${LUA_DIR}")
    endif()
    
    if (NOT IS_DIRECTORY "${LUA_DIR}")
        message(FATAL_ERROR "Given LUA_DIR is not a directory: ${LUA_DIR}")
    endif()
    
    # Find includes directory
    find_path( LUA_INCLUDE_DIR lua.hpp
               PATHS  ${LUA_DIR}/include/
                      ${LUA_DIR}/include/lua
               NO_DEFAULT_PATH
               NO_CMAKE_ENVIRONMENT_PATH
               NO_CMAKE_PATH
               NO_SYSTEM_ENVIRONMENT_PATH
               NO_CMAKE_SYSTEM_PATH)
    
    # Find libraries
    find_library( LUA_LIBRARY NAMES lua liblua
                  PATHS ${LUA_DIR}/lib
                  NO_DEFAULT_PATH
                  NO_CMAKE_ENVIRONMENT_PATH
                  NO_CMAKE_PATH
                  NO_SYSTEM_ENVIRONMENT_PATH
                  NO_CMAKE_SYSTEM_PATH)
    
    
    include(FindPackageHandleStandardArgs)
    # handle the QUIETLY and REQUIRED arguments and set LUA_FOUND to TRUE
    # if all listed variables are TRUE
    find_package_handle_standard_args(LUA  DEFAULT_MSG
                                      LUA_INCLUDE_DIR
                                      LUA_LIBRARY )
    
    if(NOT LUA_FOUND)
        message(FATAL_ERROR "LUA_DIR is not a path to a valid Lua install")
    endif()
    
    message(STATUS "Lua Includes: ${LUA_INCLUDE_DIR}")
    message(STATUS "Lua Libraries: ${LUA_LIBRARY}")

    blt_import_library(NAME       lua
                       TREAT_INCLUDES_AS_SYSTEM ON
                       DEFINES    HAVE_LUA=1
                       INCLUDES   ${LUA_INCLUDE_DIR}
                       LIBRARIES  ${LUA_LIBRARY}
                       EXPORTABLE ON)

Then ``lua`` is available to be used in the ``DEPENDS_ON`` list in the following
:ref:`blt_add_executable` or :ref:`blt_add_library` calls, or in any CMake command that accepts a target.

.. note::
    CMake targets created by :ref:`blt_import_library` are ``INTERFACE`` libraries that can be installed
    and exported if the ``EXPORTABLE`` option is enabled.  For example, if the ``calc_pi`` project depends on
    Lua, it could export its ``lua`` target.  To avoid introducing target name conflicts for users of the
    ``calc_pi`` project who might also create a target called ``lua``, ``lua`` should be exported as
    ``calc_pi\:\:lua`` .

.. note::
    Because CMake targets are only accessible from within the directory they were defined (including
    subdirectories), the ``include()`` command should be preferred to the ``add_subdirectory()`` command
    for adding CMake files that create imported library targets needed in other directories. The ``GLOBAL``
    option to :ref:`blt_import_library` can also be used to manage visibility.


CMake's Find Modules
~~~~~~~~~~~~~~~~~~~~

This time we will do exactly the same thing but using the new CMake provided ``FindLua.cmake`` module.
Instead of calling having to ensure correctness and calling ``find_path`` and ``find_library``, we
only have to call ``find_package`` and it handles this for us.  Each Find module outputs differently
named variables so it is important to read the documentation on CMake's website.  This is where
:ref:`blt_import_library` shines because you only have to figure those variables once then use the 
new imported library's ``NAME`` in the rest of your project.

.. code-block:: cmake

    # FindLua.cmake takes in LUA_DIR as an environment variable, which is the directory 
    # where Lua was installed to and fills variables: LUA_FOUND, LUA_LIBRARIES, and LUA_INCLUDE_DIR
    set(ENV{LUA_DIR} ${LUA_DIR})

    find_package(Lua)

    if (NOT LUA_FOUND)
        MESSAGE(FATAL_ERROR "Could not find Lua in the provided LUA_DIR: ${LUA_DIR}")
    endif()

    blt_import_library(NAME       lua
                       TREAT_INCLUDES_AS_SYSTEM ON
                       DEFINES    HAVE_LUA=1
                       INCLUDES   ${LUA_INCLUDE_DIR}
                       LIBRARIES  ${LUA_LIBRARIES}
                       EXPORTABLE ON)


First Class Project Support
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Some projects provide what we call First Class support.  They have gone through the effort of
properly exporting all necessary targets to use their project and
install the necessary configuration files inside of their install directory, usually something
like ``<install dir>\lib\cmake\<Project Name>Config.cmake``.

LLNL's `Axom project <https://github.com/LLNL/axom>`_ exports all targets that can be easily 
imported into your project with a single CMake function call:

.. code-block:: cmake

    # use the provided PATH directory and create a cmake target named 'axom'
    find_package(axom REQUIRED)

You can then add the created CMake target, ``axom``, to any ``DEPENDS_ON`` list or use any other
regular CMake function to change it.
