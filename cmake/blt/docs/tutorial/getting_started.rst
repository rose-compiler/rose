.. # Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
.. # other BLT Project Developers. See the top-level LICENSE file for details
.. # 
.. # SPDX-License-Identifier: (BSD-3-Clause)

.. _GettingStarted:

Getting Started
===============

BLT is easy to include in your CMake project whether it is an existing project or
you are starting from scratch.  This tutorial assumes you are using git and the CMake
Makefile generator but those commands can easily be changed or ignored.


Include BLT in your Git Repository
----------------------------------

There are two standard choices for including the BLT source in your repository:

**Add BLT as a git submodule**

This example adds BLT as a submodule, commits, and pushes the changes to your repository.

.. code-block:: bash

    cd <your repository>
    git submodule add https://github.com/LLNL/blt.git blt
    git commit -m "Adding BLT"
    git push

**Copy BLT into a subdirectory in your repository**

This example will clone a copy of BLT into your repository and remove the unneeded 
git files from the clone. It then commits and pushes the changes to your
repository.

.. code-block:: bash

    cd <your repository>
    git clone https://github.com/LLNL/blt.git
    rm -rf blt/.git
    git commit -m "Adding BLT"
    git push


Include BLT in your CMake Project
---------------------------------

In most projects, including BLT is as simple as including the following CMake
line in your base ``CMakeLists.txt`` after your ``project()`` call.

.. code-block:: cmake

    include(blt/SetupBLT.cmake)

This enables all of BLT's features in your project.

However if your project is likely to be used by other projects.  The following
is recommended:

.. literalinclude:: bare_bones/CMakeLists.txt
   :start-after: _blt_tutorial_include_blt_start
   :end-before:  _blt_tutorial_include_blt_end
   :language: cmake

This is a robust way of setting up BLT and supports an optional external BLT source
directory via the command line option ``BLT_SOURCE_DIR``.
Using the external BLT source directory allows you to use single BLT
instance across multiple independent CMake projects.
This also adds helpful error messages if the BLT submodule is missing
as well as the commands to solve it. 


Running CMake
-------------

To configure a project with CMake, first create a build directory and ``cd`` into it.  
Then run cmake with the path to your project.  

.. code-block:: bash

    cd <your project>
    mkdir build
    cd build
    cmake ..

If you are using BLT outside of your project pass the location of BLT as follows:

.. code-block:: bash

    cd <your project>
    mkdir build
    cd build
    cmake -DBLT_SOURCE_DIR="path/to/blt" ..


Example: Bare Bones BLT Project
-------------------------------

The ``bare_bones`` example project shows you some of BLT's built-in
features. It demonstrates the bare minimum required for testing purposes.

Here is the entire CMakeLists.txt file needed for a bare bones project:

.. code-block:: cmake

    cmake_minimum_required(VERSION 3.8)
    project( bare_bones )

    include(/path/to/blt/SetupBLT.cmake)


BLT also enforces some best practices for building, such as not allowing
in-source builds.  This means that BLT prevents you from generating a
project configuration directly in your project. 

For example if you run the following commands:

.. code-block:: bash

    cd <BLT repository>/docs/tutorial/bare_bones
    cmake .

you will get the following error:

.. code-block:: bash

    CMake Error at blt/SetupBLT.cmake:59 (message):
      In-source builds are not supported.  Please remove CMakeCache.txt from the
      'src' dir and configure an out-of-source build in another directory.
    Call Stack (most recent call first):
      CMakeLists.txt:55 (include)


    -- Configuring incomplete, errors occurred!

To correctly run cmake, create a build directory and run cmake from there:

.. code-block:: bash

    cd <BLT repository>/docs/bare_bones
    mkdir build
    cd build
    cmake ..

This will generate a configured ``Makefile`` in your build directory to build
Bare Bones project.  The generated makefile includes gtest and several built-in
BLT *smoke* tests, depending on the features that you have enabled in your build.  

.. note:: Smoke tests are designed to show when basic functionality is not working.
   For example, if you have turned on MPI in your project but the MPI compiler wrapper
   cannot produce an executable that runs even the most basic MPI code, the
   ``blt_mpi_smoke`` test will fail.  This helps you know that the problem doesn't
   lie in your own code but in the building/linking of MPI.

To build the project, use the following command:

.. code-block:: bash

    make

As with any other ``make``-based project, you can utilize parallel job tasks 
to speed up the build with the following command:

.. code-block:: bash

    make -j8

Next, run all tests in this project with the following command:

.. code-block:: bash

    make test

If everything went correctly, you should have the following output:

.. code-block:: bash

    Running tests...
    Test project blt/docs/tutorial/bare_bones/build
        Start 1: blt_gtest_smoke
    1/1 Test #1: blt_gtest_smoke ..................   Passed    0.01 sec

    100% tests passed, 0 tests failed out of 1

    Total Test time (real) =   0.10 sec

Note that the default options for ``bare_bones`` only include a single test
``blt_gtest_smoke``. As we will see later on, BLT includes additional smoke
tests that are activated when BLT is configured with other options enabled,
like Fortran, MPI, OpenMP, and CUDA. 


Example files
-------------

Files related to setting up the Bare Bones project:

.. container:: toggle

    .. container:: label

        ``CMakeLists.txt``

    .. literalinclude::  ./bare_bones/CMakeLists.txt
        :language: cmake
        :linenos:
