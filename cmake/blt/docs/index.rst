.. # Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
.. # other BLT Project Developers. See the top-level LICENSE file for details
.. # 
.. # SPDX-License-Identifier: (BSD-3-Clause)

BLT: Build, Link, and Test
==========================

BLT is a composition of CMake macros and several widely used open source tools 
assembled to simplify HPC software development. 

BLT was released by Lawrence Livermore National Laboratory (LLNL) under a BSD-style open source license. 
It is developed on GitHub under `LLNL's GitHub organization <https://github.com/LLNL/blt>`_.

.. note::
  BLT officially supports CMake 3.8 and above.  However we only print a warning if you
  are below this version.  Some features in earlier versions may or may not work. Use at your own risk.


BLT at a Glance
---------------

* Simplifies setting up a CMake-based build system

  * CMake macros for:

    * Creating libraries and executables
    * Managing compiler flags
    * Managing external dependencies

  * Handles differences across CMake versions

  * Multi-platform support (HPC Platforms, OSX, Windows)

* Batteries included

  * Built-in support for HPC Basics: MPI, OpenMP, CUDA, and HIP
  * Built-in support for unit testing in C/C++ and Fortran
  
* Streamlines development processes

  * Support for documentation generation
  * Support for code health tools:

    * Runtime and static analysis, benchmarking


Questions
---------

Any questions can be sent to blt-dev@llnl.gov.  If you are an LLNL employee or collaborator, we have an
internal Microsoft Teams group chat named "BLT" as well.


Contributions
-------------

We welcome all kinds of contributions: new features, bug fixes, documentation edits.

To contribute, make a `pull request <https://github.com/LLNL/blt/pulls>`_, with ``develop``
as the destination branch. We use CI testing and your branch must pass these tests before
being merged.

For more information, see the `contributing guide <https://github.com/LLNL/blt/blob/develop/CONTRIBUTING.md>`_.


Authors
-------

Thanks to all of BLT's `contributors <https://github.com/LLNL/blt/graphs/contributors>`_.


.. toctree::
    :caption: Table of Contents
    :maxdepth: 2

    User Tutorial <tutorial/index>
    API Documentation <api/index>
    Developer Guide <developer/index>
