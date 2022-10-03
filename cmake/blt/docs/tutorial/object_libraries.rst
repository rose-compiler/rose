.. # Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
.. # other BLT Project Developers. See the top-level LICENSE file for details
.. # 
.. # SPDX-License-Identifier: (BSD-3-Clause)

.. _ObjectLibraries:

Object Libraries
================

BLT has simplified the use of CMake object libraries through the 
:ref:`blt_add_library` macro. Object libraries are a collection of object files
that are not linked or archived into a library. They are used in other libraries
or executables through the ``DEPENDS_ON`` macro argument. This is generally
useful for combining smaller libraries into a larger library without 
the linker removing unused symbols in the larger library.

.. code-block:: cmake

    blt_add_library(NAME    myObjectLibrary
                    SOURCES source1.cpp
                    HEADERS header1.cpp
                    OBJECT  TRUE)

    blt_add_exectuble(NAME       helloWorld
                      SOURCES    main.cpp
                      DEPENDS_ON myObjectLibrary)

.. note::
  Due to record keeping on BLT's part to make object libraries as easy to use
  as possible, you need to define object libraries before you use them
  if you need their inheritable information to be correct.

If you are using separable CUDA compilation (relocatable device code) in your
object library, users of that library will be required to use NVCC to link their
executables - in general, only NVCC can perform the "device link" step.  To remove
this restriction, you can enable the ``CUDA_RESOLVE_DEVICE_SYMBOLS`` property on
an object library:

.. code-block:: cmake

   set_target_properties(myObjectLibrary PROPERTIES CUDA_RESOLVE_DEVICE_SYMBOLS ON)

To enable this device linking step for all libraries in your project (including object libraries), you
can set the ``CMAKE_CUDA_RESOLVE_DEVICE_SYMBOLS`` option to ``ON``.  This defaults the
``CUDA_RESOLVE_DEVICE_SYMBOLS`` target property to ``ON`` for all targets created by BLT.

You can read more about this property in the
`CMake documentation <https://cmake.org/cmake/help/latest/prop_tgt/CUDA_RESOLVE_DEVICE_SYMBOLS.html>`_.

.. note::
  These options only apply when an object library in your project is linked later
  into a shared or static library, in which case a separate object file containing
  device symbols is created and added to the "final" library.  Object libraries
  provided directly to users of your project will still require a device link step.

The ``CUDA_RESOLVE_DEVICE_SYMBOLS`` property is also supported for static and shared libraries.
By default, it is enabled for shared libraries but disabled for static libraries.
