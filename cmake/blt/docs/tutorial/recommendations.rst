.. # Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
.. # other BLT Project Developers. See the top-level LICENSE file for details
.. # 
.. # SPDX-License-Identifier: (BSD-3-Clause)

.. _CMakeRecommendations:

CMake Recommendations
=====================

This section includes several recommendations for how to wield CMake. 
Some of them are embodied in BLT, others are broader suggestions for CMake bliss.


Disable In-source Builds
------------------------

*BLT Enforces This*

In-source builds clutter source code with temporary build files and prevent other out-of-source builds 
from being created. Disabling in-source builds avoids clutter and accidental checkins of temporary build files.


Avoid using Globs to Identify Source Files
------------------------------------------

Globs are evaluated at CMake configure time - not build time. This means CMake will not detect new source files 
when they are added to the file system unless there are other changes that trigger CMake to reconfigure. 

The CMake documentation also warns against `this <https://cmake.org/cmake/help/v3.20/command/file.html?highlight=glob#filesystem>`_.


Use Arguments instead of Options in CMake Macros and Functions
--------------------------------------------------------------

``CMAKE_PARSE_ARGUMENTS`` allows Macros or Functions to support options. Options are enabled by passing them 
by name when calling a Macro or Function. Because of this, wrapping an existing Macro or Function in a way 
that passes through options requires if tests and multiple copies of the call. For example:

.. code-block:: cmake

  if(OPTION)
      my_function(arg1 arg2 arg3 OPTION)
  else()
      my_function(arg1 arg2 arg3)
  endif()

Adding more options compounds the logic to achieve these type of calls.

To simplify calling logic, we recommend using an argument instead of an option.

.. code-block:: cmake

  if(OPTION)
      set(arg4_value ON)
  endif()
  
  my_function(arg1 arg2 arg3 ${arg4_value})


Prefer Explicit Paths to Locate Third-party Dependencies
--------------------------------------------------------

Require passing explicit paths (ex: ``ZZZ_DIR``) for third-party dependency locations. 
This avoids surprises with incompatible installs sprinkled in various system locations. 
If you are using off-the-shelf *FindZZZ* logic, also consider adding CMake checks 
to verify that *FindZZZ* logic actually found the dependencies at the location specified.


Error at Configure Time for Third-party Dependency Problems
-----------------------------------------------------------

Emit a configure error if an explicitly identified third-party dependency is not found or an incorrect
version is found. If an explicit path to a dependency is given (ex: ``ZZZ_DIR``) it should be valid or result in a CMake configure error.

In contrast, if you only issue a warning and automatically disable a feature when a third-party dependency
is bad, the warning often goes unnoticed and may not be caught until folks using your software are surprised. 
Emitting a configure error stops CMake and draws attention to the fact that something is wrong.  
Optional dependencies are still supported by including them only if an explicit path 
to the dependency is provided (ex: ``ZZZ_DIR``).


Add Headers as Source Files to Targets
--------------------------------------

*BLT Macros Support This*

This ensures headers are tracked as dependencies and are included in the projects 
created by CMake's IDE generators, like Xcode or Eclipse. 


Always Support `make install`
-----------------------------

This allows CMake to do the right thing based on ``CMAKE_INSTALL_PREFIX``, 
and also helps support CPack create release packages. This is especially important for libraries. 
In addition to targets, header files require an explicit install command.

Here is an example that installs a target and its headers:

.. code-block:: cmake

  #--------------------------------------------------------
  # Install Targets for example lib
  #--------------------------------------------------------
  install(FILES ${example_headers} DESTINATION include)
  install(TARGETS example
    EXPORT example
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
  )

