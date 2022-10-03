.. # Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
.. # other BLT Project Developers. See the top-level LICENSE file for details
.. # 
.. # SPDX-License-Identifier: (BSD-3-Clause)

.. _ImportingTargets:

Importing Targets
=================

One key goal for BLT is to simplify the use of external dependencies and libraries when building
your project. To accomplish this, BLT provides a ``DEPENDS_ON`` option for the :ref:`blt_add_library`
and :ref:`blt_add_executable` macros that supports both your own projects CMake targets 
and imported targets. We have logically broken this topic into two groups:

:ref:`CommonHPCDependencies`
   Dependencies such as MPI, CUDA, HIP, and OpenMP, are bundled and ready to use included
   with BLT as regular named CMake targets. For example, just adding ``openmp`` to any ``DEPENDS_ON``
   will add the necessary OpenMP compiler and link flags to any target. 

:ref:`ThirdPartyLibraries`
   These are external libraries that your project depend on, such as Lua. They are imported into your
   project in different ways depending on the level of CMake support provided
   by that project.  BLT provides a macro, :ref:`blt_import_library`, which allows you to bundle all 
   necessary information under a single name.  Some projects properly export their CMake targets and
   only need to be imported via a call to ``include()``.

.. toctree::
    :hidden:

    common_hpc_dependencies
    third_party_libraries
