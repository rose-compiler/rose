.. # Copyright (c) 2017-2020, Lawrence Livermore National Security, LLC and
.. # other BLT Project Developers. See the top-level LICENSE file for details
.. #
.. # SPDX-License-Identifier: (BSD-3-Clause)

Code Metric Macros
==================

.. _blt_add_code_coverage_target:

blt_add_code_coverage_target
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cmake

    blt_add_code_coverage_target( NAME               <Created Target Name>
                                  RUNNER             <The command to run the tests>
                                  SOURCE_DIRECTORIES [dir1 [dir2 ...]] )

Creates a new build target for generating a code coverage report.

NAME
  Name of created build target

RUNNER
  The command used to run the tests, e.g., ``make test``

SOURCE_DIRECTORIES
  The directories containing the source code whose test coverage is to be evaluated

Code coverage is the degree to which the tests for a piece of software "cover" functions
and/or individual lines of code.  It can be used to identify gaps in testing, namely, code
that is not tested.  GCC's ``gcov`` tool is used to generate the coverage data, and its accompanying
``lcov`` tool is used to generate an HTML report containing coverage percentage information and
highlighted source code that indicates which code was or was not executed as part of the test suite.

.. note::
  Coverage analysis is only supported by GNU/Clang compilers.

This functionality requires that BLT's ``ENABLE_COVERAGE`` option is enabled and that ``gcov``, ``lcov``,
and ``genhtml`` are present on your system.  To use a specific version of one of these tools, you can set
``GCOV_EXECUTABLE``, ``LCOV_EXECUTABLE``, and ``GENHTML_EXECUTABLE`` to point at the desired version(s).

.. note::
  The ``ENABLE_COVERAGE`` option will add compiler flags that instrument your code (and slow it down).
  The option should never be enabled by default in a project for performance reasons.
