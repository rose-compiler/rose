.. # Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
.. # other BLT Project Developers. See the top-level LICENSE file for details
.. # 
.. # SPDX-License-Identifier: (BSD-3-Clause)

.. _HostConfigs:

Host-configs
============

To capture (and revision control) build options, third party library paths, etc.,
we recommend using CMake's initial-cache file mechanism. This feature allows you
to pass a file to CMake that provides variables to bootstrap the configuration
process. 

You can pass initial-cache files to cmake via the ``-C`` command line option.

.. code-block:: bash

    cmake -C config_file.cmake


We call these initial-cache files ``host-config`` files since we typically create
a file for each platform or for specific hosts, if necessary. 

These files use standard CMake commands. CMake ``set()`` commands need to specify
``CACHE`` as follows:

.. code-block:: cmake

    set(CMAKE_VARIABLE_NAME {VALUE} CACHE PATH "")

Here is a snippet from a host-config file that specifies compiler details for
using specific gcc (version 4.9.3 in this case) on the LLNL Pascal cluster. 

.. literalinclude:: ../../host-configs/llnl/toss_3_x86_64_ib/gcc@4.9.3_nvcc.cmake
   :start-after: _blt_tutorial_compiler_config_start
   :end-before:  _blt_tutorial_compiler_config_end
   :language: cmake


Building and Testing on Pascal
------------------------------

Since compute nodes on the Pascal cluster have CPUs and GPUs, here is how you
can use the host-config file to configure a build of the ``calc_pi``  project with
MPI and CUDA enabled on Pascal:

.. code-block:: bash
    
    # create build dir
    mkdir build
    cd build
    # configure using host-config
    cmake -C ../../host-configs/llnl/toss_3_x86_64_ib/gcc@4.9.3_nvcc.cmake  ..

After building (``make``), you can run ``make test`` on a batch node (where the GPUs reside) 
to run the unit tests that are using MPI and CUDA:

.. code-block:: console

  bash-4.1$ salloc -A <valid bank>
  bash-4.1$ make   
  bash-4.1$ make test
  
  Running tests...
  Test project blt/docs/tutorial/calc_pi/build
      Start 1: test_1
  1/8 Test #1: test_1 ...........................   Passed    0.01 sec
      Start 2: test_2
  2/8 Test #2: test_2 ...........................   Passed    2.79 sec
      Start 3: test_3
  3/8 Test #3: test_3 ...........................   Passed    0.54 sec
      Start 4: blt_gtest_smoke
  4/8 Test #4: blt_gtest_smoke ..................   Passed    0.01 sec
      Start 5: blt_fruit_smoke
  5/8 Test #5: blt_fruit_smoke ..................   Passed    0.01 sec
      Start 6: blt_mpi_smoke
  6/8 Test #6: blt_mpi_smoke ....................   Passed    2.82 sec
      Start 7: blt_cuda_smoke
  7/8 Test #7: blt_cuda_smoke ...................   Passed    0.48 sec
      Start 8: blt_cuda_runtime_smoke
  8/8 Test #8: blt_cuda_runtime_smoke ...........   Passed    0.11 sec

  100% tests passed, 0 tests failed out of 8

  Total Test time (real) =   6.80 sec


Building and Testing on Ray
---------------------------

Here is how you can use the host-config file to configure a build of the ``calc_pi``  project with MPI and CUDA 
enabled on the LLNL BlueOS Ray cluster:

.. code-block:: bash
    
    # create build dir
    mkdir build
    cd build
    # configure using host-config
    cmake -C ../../host-configs/llnl/blueos_3_ppc64le_ib_p9/clang@upstream_nvcc_xlf.cmake  ..

And here is how to build and test the code on Ray:

.. code-block:: console

  bash-4.2$ lalloc 1 -G <valid group>
  bash-4.2$ make
  bash-4.2$ make test
  
  Running tests...
  Test project projects/blt/docs/tutorial/calc_pi/build
      Start 1: test_1
  1/7 Test #1: test_1 ...........................   Passed    0.01 sec
      Start 2: test_2
  2/7 Test #2: test_2 ...........................   Passed    1.24 sec
      Start 3: test_3
  3/7 Test #3: test_3 ...........................   Passed    0.17 sec
      Start 4: blt_gtest_smoke
  4/7 Test #4: blt_gtest_smoke ..................   Passed    0.01 sec
      Start 5: blt_mpi_smoke
  5/7 Test #5: blt_mpi_smoke ....................   Passed    0.82 sec
      Start 6: blt_cuda_smoke
  6/7 Test #6: blt_cuda_smoke ...................   Passed    0.15 sec
      Start 7: blt_cuda_runtime_smoke
  7/7 Test #7: blt_cuda_runtime_smoke ...........   Passed    0.04 sec
  
  100% tests passed, 0 tests failed out of 7
  
  Total Test time (real) =   2.47 sec  


Building and Testing on Summit
-------------------------------

Here is how you can use the host-config file to configure a build of the ``calc_pi``  project with MPI and CUDA 
enabled on the OLCF Summit cluster:

.. code-block:: bash
    
    # load the cmake module
    module load cmake
    # create build dir
    mkdir build
    cd build
    # configure using host-config
    cmake -C ../../host-configs/olcf/summit/gcc@6.4.0_nvcc.cmake  ..

 
And here is how to build and test the code on Summit:

.. code-block:: console

  bash-4.2$ bsub -W 30 -nnodes 1 -P <valid project>  -Is /bin/bash
  bash-4.2$ module load gcc cuda
  bash-4.2$ make
  bash-4.2$ make test

  Running tests...
  Test project /projects/blt/docs/tutorial/calc_pi/build
        Start  1: test_1
   1/11 Test  #1: test_1 ...........................   Passed    0.00 sec
        Start  2: test_2
   2/11 Test  #2: test_2 ...........................   Passed    1.03 sec
        Start  3: test_3
   3/11 Test  #3: test_3 ...........................   Passed    0.21 sec
        Start  4: blt_gtest_smoke
   4/11 Test  #4: blt_gtest_smoke ..................   Passed    0.00 sec
        Start  5: blt_fruit_smoke
   5/11 Test  #5: blt_fruit_smoke ..................   Passed    0.00 sec
        Start  6: blt_mpi_smoke
   6/11 Test  #6: blt_mpi_smoke ....................   Passed    0.76 sec
        Start  7: blt_cuda_smoke
   7/11 Test  #7: blt_cuda_smoke ...................   Passed    0.22 sec
        Start  8: blt_cuda_runtime_smoke
   8/11 Test  #8: blt_cuda_runtime_smoke ...........   Passed    0.07 sec
        Start  9: blt_cuda_version_smoke
   9/11 Test  #9: blt_cuda_version_smoke ...........   Passed    0.06 sec
        Start 10: blt_cuda_mpi_smoke
  10/11 Test #10: blt_cuda_mpi_smoke ...............   Passed    0.80 sec
        Start 11: blt_cuda_gtest_smoke
  11/11 Test #11: blt_cuda_gtest_smoke .............   Passed    0.21 sec

  100% tests passed, 0 tests failed out of 11

  Total Test time (real) =   3.39 sec


Example Host-configs
--------------------

Basic TOSS3 (for example: Quartz) host-config that has C, C++, and Fortran Compilers along with MPI support:

.. container:: toggle

    .. container:: label

        ``gcc@8.3.1 host-config``

    .. literalinclude::  ../../host-configs/llnl/toss_3_x86_64_ib/gcc@8.3.1.cmake
        :language: cmake
        :linenos:

Here are the full example host-config files for LLNL's Pascal, Ray, and Quartz Clusters that uses
the default compilers on the system:

.. container:: toggle

    .. container:: label

        ``gcc@4.9.3 host-config``

    .. literalinclude::  ../../host-configs/llnl/toss_3_x86_64_ib/gcc@4.9.3_nvcc.cmake
        :language: cmake
        :linenos:

More complicated BlueOS host-config that has C, C++, MPI, and CUDA support:

.. container:: toggle

    .. container:: label

        ``clang@upstream C++17 host-config``

    .. literalinclude::  ../../host-configs/llnl/blueos_3_ppc64le_ib_p9/clang@upstream_nvcc_c++17_no_separable.cmake
        :language: cmake
        :linenos:

Here is a full example host-config file for an OSX laptop, using a set of dependencies built with Spack:

.. container:: toggle

    .. container:: label

        ``OSX clang@7.3.0 host-config``

    .. literalinclude::  ../../host-configs/darwin/elcapitan-x86_64/naples-clang@7.3.0.cmake
        :language: cmake
        :linenos:

