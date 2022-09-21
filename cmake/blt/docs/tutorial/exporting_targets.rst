.. # Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
.. # other BLT Project Developers. See the top-level LICENSE file for details
.. # 
.. # SPDX-License-Identifier: (BSD-3-Clause)

.. _ExportingTargets:

Exporting Targets
=================

BLT provides several built-in targets for commonly used libraries:

``mpi``
    Available when ``ENABLE_MPI`` is ``ON``

``openmp``
    Available when ``ENABLE_OPENMP`` is ``ON``

``cuda`` and ``cuda_runtime``
    Available when ``ENABLE_CUDA`` is ``ON``

``blt_hip`` and ``blt_hip_runtime``
    Available when ``ENABLE_HIP`` is ``ON``

These targets can be made exportable in order to make them available to users of
your project via CMake's ``install()`` command.  Setting BLT's ``BLT_EXPORT_THIRDPARTY``
option to ``ON`` will mark all active targets in the above list as ``EXPORTABLE``
(see the :ref:`blt_import_library` API documentation for more info).

.. note::  As with other ``EXPORTABLE`` targets created by :ref:`blt_import_library`,
    these targets should be prefixed with the name of the project.  Either the ``EXPORT_NAME``
    target property or the ``NAMESPACE`` option to CMake's ``install``
    command can be used to modify the name of an installed target.

.. note:: If a target in your project is added to an export set, any of its dependencies
    marked ``EXPORTABLE`` must be added to the same export set.  Failure to add them will
    result in a CMake error in the exporting project.

.. note:: The recommended usage of the HIP targets is via the ``blt::hip`` and
    ``blt::hip_runtime`` aliases. Alias targets cannot be exported, so the
    ``blt_hip``/``blt_hip_runtime`` names can be used for this purpose.

Typical usage of the ``BLT_EXPORT_THIRDPARTY`` option is as follows:

.. code-block:: cmake

    # BLT configuration - enable MPI
    set(ENABLE_MPI ON CACHE BOOL "")
    # and mark the subsequently created MPI target as exportable
    set(BLT_EXPORT_THIRDPARTY ON CACHE BOOL "")
    # Both of the above must happen before SetupBLT.cmake is included
    include(/path/to/SetupBLT.cmake)

    # Later, a project might mark a target as dependent on MPI
    blt_add_executable( NAME    example_1
                        SOURCES example_1.cpp
                        DEPENDS_ON mpi )

    # Add the example_1 target to the example-targets export set
    install(TARGETS example_1 EXPORT example-targets)

    # Add BLT's targets to the same export set - this is required
    # because the mpi target was marked exportable
    blt_export_tpl_targets(EXPORT example-targets)

To avoid collisions with projects that import "example-targets", it is recommended to provide
a namespace for the ``mpi`` target:

.. code-block:: cmake

    blt_export_tpl_targets(EXPORT example-targets NAMESPACE example)

With this approach the ``example_1`` target's exported name is unchanged - a 
project that imports the ``example-targets`` export set will have ``example_1``
and ``example::mpi`` targets made available.  The imported ``example_1`` will
depend on ``example::mpi``.
