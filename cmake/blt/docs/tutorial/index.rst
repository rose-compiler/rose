.. # Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
.. # other BLT Project Developers. See the top-level LICENSE file for details
.. # 
.. # SPDX-License-Identifier: (BSD-3-Clause)

User Tutorial
=============

This tutorial provides instructions for:

    * Adding BLT to a CMake project
    * Building, linking, and installing libraries and executables
    * Setting up unit tests with GTest
    * Setting up *host-config* files to handle multiple platform configurations
    * Using external project dependencies
    * Exporting your project's CMake targets for outside projects to use
    * Creating documentation with Sphinx and Doxygen

The two example CMake projects used are included in BLT's source tree at:

* ``<blt-dir>/cmake/docs/tutorial/bare_bones``
* ``<blt-dir>/cmake/docs/tutorial/calc_pi``

Here are direct links to the projects in BLT's GitHub repo:

* https://github.com/LLNL/blt/tree/develop/docs/tutorial/bare_bones
* https://github.com/LLNL/blt/tree/develop/docs/tutorial/calc_pi


``bare_bones`` provides a minimum template for starting a new project and
``calc_pi`` provides several examples that calculate the value of :math:`\pi` 
by approximating the integral :math:`f(x) = \int_0^14/(1+x^2)` using numerical
integration. The code is adapted from ANL's `using MPI examples <https://www.mcs.anl.gov/research/projects/mpi/usingmpi/examples-usingmpi/simplempi/cpi_c.html>`_.


Most of the tutorial focuses on the BLT features used to create the complete
``calc_pi`` project.


The tutorial requires a C++ compiler and CMake, we recommend using CMake 3.8.0 or newer.
Parts of the tutorial also require MPI, CUDA, Sphinx, and Doxygen.

We provide instructions to build and run these projects on several
LLNL HPC platforms and ORNL's Summit platform. See :ref:`HostConfigs`.

.. toctree::
    :maxdepth: 3
    :caption: Tutorial Contents
    :includehidden:

    getting_started
    creating_targets
    adding_tests
    host_configs
    importing_targets
    creating_documentation
    advanced_topics
    recommendations
