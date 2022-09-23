.. # Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
.. # other BLT Project Developers. See the top-level LICENSE file for details
.. #
.. # SPDX-License-Identifier: (BSD-3-Clause)

Code Check Macros
==================

.. _blt_add_code_checks:

blt_add_code_checks
~~~~~~~~~~~~~~~~~~~

.. code-block:: cmake

    blt_add_code_checks( PREFIX               <Base name used for created targets>
                         SOURCES              [source1 [source2 ...]]
                         ASTYLE_CFG_FILE      <Path to AStyle config file>
                         CLANGFORMAT_CFG_FILE <Path to ClangFormat config file>
                         UNCRUSTIFY_CFG_FILE  <Path to Uncrustify config file>
                         YAPF_CFG_FILE        <Path to Yapf config file>
                         CMAKEFORMAT_CFG_FILE <Path to CMakeFormat config file>
                         CPPCHECK_FLAGS       <List of flags added to Cppcheck>
                         CLANGQUERY_CHECKER_DIRECTORIES [dir1 [dir2]])

This macro adds all enabled code check targets for the given SOURCES.

PREFIX
  Prefix used for the created code check build targets. For example:
  <PREFIX>_uncrustify_check

SOURCES
  Source list that the code checks will be ran on

ASTYLE_CFG_FILE
  Path to AStyle config file

CLANGFORMAT_CFG_FILE
  Path to ClangFormat config file

UNCRUSTIFY_CFG_FILE
  Path to Uncrustify config file

YAPF_CFG_FILE
  Path to Yapf config file

CMAKEFORMAT_CFG_FILE
  Path to CMakeFormat config file

CPPCHECK_FLAGS
  List of flags added to Cppcheck

CLANGQUERY_CHECKER_DIRECTORIES
  List of directories where clang-query's checkers are located

The purpose of this macro is to enable all code checks in the default manner.  It runs
all code checks from the working directory ``CMAKE_BINARY_DIR``.  If you need more specific
functionality you will need to call the individual code check macros yourself.

.. note::
  For library projects that may be included as a subproject of another code via CMake's
  ``add_subproject()``, we recommend guarding "code check" targets against being included in
  other codes.  The following check ``if ("${PROJECT_SOURCE_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")``
  will stop your code checks from running unless you are the main CMake project.

Sources are filtered based on file extensions for use in these code
checks.  If you need additional file extensions defined add them to
``BLT_C_FILE_EXTS``, ``BLT_Python_FILE_EXTS``, ``BLT_CMAKE_FILE_EXTS``, and
``BLT_Fortran_FILE_EXTS``. Currently this macro only has code checks for
C/C++ and Python; it simply filters out the Fortran files.

This macro supports C/C++ code formatting with either AStyle, ClangFormat, or Uncrustify
(but not all at the same time) only if the following requirements are met:

- AStyle

  * ``ASTYLE_CFG_FILE`` is given
  * ``ASTYLE_EXECUTABLE`` is defined and found prior to calling this macro

- ClangFormat

  * ``CLANGFORMAT_CFG_FILE`` is given
  * ``CLANGFORMAT_EXECUTABLE`` is defined and found prior to calling this macro

- Uncrustify

  * ``UNCRUSTIFY_CFG_FILE`` is given
  * ``UNCRUSTIFY_EXECUTABLE`` is defined and found prior to calling this macro

.. note::
  ClangFormat does not support a command line option for config files.  To work around this,
  we copy the given config file to the build directory where this macro runs from.

This macro also supports Python code formatting with Yapf only if the following requirements
are met:

* ``YAPF_CFG_FILE`` is given
* ``YAPF_EXECUTABLE`` is defined and found prior to calling this macro

This macro also supports CMake code formatting with CMakeFormat only if the following requirements are met:

* ``CMAKEFORMAT_CFG_FILE`` is given
* ``CMAKEFORMAT_EXECUTABLE`` is defined and found prior to calling this macro

Enabled code formatting checks produce a ``check`` build target that will test to see if you
are out of compliance with your code formatting and a ``style`` build target that will actually
modify your source files.  It also creates smaller child build targets that follow the pattern
``<PREFIX>_<astyle|clangformat|uncrustify>_<check|style>``.

If a particular version of a code formatting tool is required, you can
configure BLT to enforce that version by setting
``BLT_REQUIRED_<CLANGFORMAT|ASTYLE|UNCRUSTIFY|YAPF|CMAKEFORMAT>_VERSION``
to as much of the version as you need.  For example:

.. code-block:: cmake

  # If astyle major version 3 is required (3.0, 3.1, etc are acceptable)
  set(BLT_REQUIRED_ASTYLE_VERSION "3")
  # Or, if exactly 3.1 is needed
  set(BLT_REQUIRED_ASTYLE_VERSION "3.1")

This macro supports the following static analysis tools with their requirements:

- CppCheck

  * ``CPPCHECK_EXECUTABLE`` is defined and found prior to calling this macro
  * <optional> ``CPPCHECK_FLAGS`` added to the cppcheck command line before the sources

- Clang-Query

  * ``CLANGQUERY_EXECUTABLE`` is defined and found prior to calling this macro
  * ``CLANGQUERY_CHECKER_DIRECTORIES`` parameter given or ``BLT_CLANGQUERY_CHECKER_DIRECTORIES`` is defined

- clang-tidy

  * ``CLANGTIDY_EXECUTABLE`` is defined and found prior to calling this macro

These are added as children to the ``check`` build target and produce child build targets
that follow the pattern ``<PREFIX>_<cppcheck|clang_query|clang_tidy>_check``.


.. _blt_add_clang_query_target:

blt_add_clang_query_target
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cmake

    blt_add_clang_query_target( NAME                <Created Target Name>
                                WORKING_DIRECTORY   <Working Directory>
                                COMMENT             <Additional Comment for Target Invocation>
                                CHECKERS            <specifies a subset of checkers>
                                DIE_ON_MATCH        <TRUE | FALSE (default)>
                                SRC_FILES           [source1 [source2 ...]]
                                CHECKER_DIRECTORIES [dir1 [dir2]])

Creates a new build target for running clang-query.

NAME
  Name of created build target

WORKING_DIRECTORY
  Directory in which the clang-query command is run. Defaults to where macro is called.

COMMENT
  Comment prepended to the build target output

CHECKERS
  list of checkers to be run by created build target

DIE_ON_MATCH
  Causes build failure on first clang-query match. Defaults to FALSE.S

SRC_FILES
  Source list that clang-query will be ran on

CHECKER_DIRECTORIES
  List of directories where clang-query's checkers are located

Clang-query is a tool used for examining and matching the Clang AST. It is useful for enforcing
coding standards and rules on your source code.  A good primer on how to use clang-query can be
found `here <https://devblogs.microsoft.com/cppblog/exploring-clang-tooling-part-2-examining-the-clang-ast-with-clang-query/>`__.

A list of checker directories is required for clang-query, this can be defined either by
the parameter ``CHECKER_DIRECTORIES`` or the variable ``BLT_CLANGQUERY_CHECKER_DIRECTORIES``.

Turning on ``DIE_ON_MATCH`` is useful if you're using this in CI to enforce rules about your code.

``CHECKERS`` are the static analysis passes to specifically run on the target. The following checker
options can be given:

    * (no value)          : run all available static analysis checks found
    * (checker1:checker2) : run checker1 and checker2
    * (interpreter)       : run the clang-query interpeter to interactively develop queries


.. _blt_add_cppcheck_target:

blt_add_cppcheck_target
~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cmake

    blt_add_cppcheck_target( NAME                <Created Target Name>
                             WORKING_DIRECTORY   <Working Directory>
                             PREPEND_FLAGS       <Additional flags for cppcheck>
                             APPEND_FLAGS        <Additional flags for cppcheck>
                             COMMENT             <Additional Comment for Target Invocation>
                             SRC_FILES           [source1 [source2 ...]] )

Creates a new build target for running cppcheck

NAME
  Name of created build target

WORKING_DIRECTORY
  Directory in which the clang-query command is run. Defaults to where macro is called.

PREPEND_FLAGS
  Additional flags added to the front of the cppcheck flags

APPEND_FLAGS
 Additional flags added to the end of the cppcheck flags

COMMENT
  Comment prepended to the build target output

SRC_FILES
  Source list that cppcheck will be ran on

Cppcheck is a static analysis tool for C/C++ code. More information about
Cppcheck can be found `here <http://cppcheck.sourceforge.net/>`__.


.. _blt_add_clang_tidy_target:

blt_add_clang_tidy_target
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cmake

    blt_add_clang_tidy_target( NAME              <Created Target Name>
                               WORKING_DIRECTORY <Working Directory>
                               COMMENT           <Additional Comment for Target Invocation>
                               CHECKS            <If specified, enables a specific set of checks>
                               FIX               <TRUE | FALSE (default)>
                               SRC_FILES         [source1 [source2 ...]] )

Creates a new build target for running clang-tidy.

NAME
  Name of created build target

WORKING_DIRECTORY
  Directory in which the clang-tidy command is run. Defaults to where macro is called.

COMMENT
  Comment prepended to the build target output

CHECKS
  List of checks to be run on the selected source files, available checks are listed
  `here <https://clang.llvm.org/extra/clang-tidy/checks/list.html>`__.

FIX
  Applies fixes for checks (a subset of clang-tidy checks specify how they should be resolved)

SRC_FILES
  Source list that clang-tidy will be ran on

Clang-tidy is a tool used for diagnosing and fixing typical programming errors. It is useful for enforcing
coding standards and rules on your source code.  Clang-tidy is documented `here <https://clang.llvm.org/extra/clang-tidy/index.html>`__.

``CHECKS`` are the static analysis "rules" to specifically run on the target.
If no checks are specified, clang-tidy will run the default available static analysis checks.


.. _blt_add_astyle_target:

blt_add_astyle_target
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cmake

    blt_add_astyle_target( NAME              <Created Target Name>
                           MODIFY_FILES      [TRUE | FALSE (default)]
                           CFG_FILE          <AStyle Configuration File>
                           PREPEND_FLAGS     <Additional Flags to AStyle>
                           APPEND_FLAGS      <Additional Flags to AStyle>
                           COMMENT           <Additional Comment for Target Invocation>
                           WORKING_DIRECTORY <Working Directory>
                           SRC_FILES         [FILE1 [FILE2 ...]] )

Creates a new build target for running AStyle

NAME
  Name of created build target

MODIFY_FILES
  Modify the files in place. Defaults to FALSE.

CFG_FILE
  Path to AStyle config file

PREPEND_FLAGS
  Additional flags added to the front of the AStyle flags

APPEND_FLAGS
 Additional flags added to the end of the AStyle flags

COMMENT
  Comment prepended to the build target output

WORKING_DIRECTORY
  Directory in which the AStyle command is run. Defaults to where macro is called.

SRC_FILES
  Source list that AStyle will be ran on

AStyle is a Source Code Beautifier for C/C++ code. More information about
AStyle can be found `here <http://astyle.sourceforge.net/>`__.

When ``MODIFY_FILES`` is set to ``TRUE``, modifies the files in place and adds the created build
target to the parent `style` build target.  Otherwise the files are not modified and the
created target is added to the parent ``check`` build target. This target will notify you
which files do not conform to your style guide.

.. Note::
  Setting ``MODIFY_FILES`` to ``FALSE`` is only supported in AStyle v2.05 or greater.


.. _blt_add_clangformat_target:

blt_add_clangformat_target
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cmake

    blt_add_clangformat_target( NAME              <Created Target Name>
                                MODIFY_FILES      [TRUE | FALSE (default)]
                                CFG_FILE          <ClangFormat Configuration File>
                                PREPEND_FLAGS     <Additional Flags to ClangFormat>
                                APPEND_FLAGS      <Additional Flags to ClangFormat>
                                COMMENT           <Additional Comment for Target Invocation>
                                WORKING_DIRECTORY <Working Directory>
                                SRC_FILES         [FILE1 [FILE2 ...]] )

Creates a new build target for running ClangFormat

NAME
  Name of created build target

MODIFY_FILES
  Modify the files in place. Defaults to FALSE.

CFG_FILE
  Path to ClangFormat config file

PREPEND_FLAGS
  Additional flags added to the front of the ClangFormat flags

APPEND_FLAGS
 Additional flags added to the end of the ClangFormat flags

COMMENT
  Comment prepended to the build target output

WORKING_DIRECTORY
  Directory in which the ClangFormat command is run. Defaults to where macro is called.

SRC_FILES
  Source list that ClangFormat will be ran on

ClangFormat is a Source Code Beautifier for C/C++ code. More information about
ClangFormat can be found `here <https://clang.llvm.org/docs/ClangFormat.html>`__.

When ``MODIFY_FILES`` is set to ``TRUE``, modifies the files in place and adds the created build
target to the parent ``style`` build target.  Otherwise the files are not modified and the
created target is added to the parent `check` build target. This target will notify you
which files do not conform to your style guide.

.. note::
  ClangFormat does not support a command line option for config files.  To work around this,
  we copy the given config file to the given working directory. We recommend using the build
  directory ``${PROJECT_BINARY_DIR}``. Also if someone is directly including your CMake project
  in theirs, you may conflict with theirs.  We recommend guarding your code checks against this
  with the following check ``if ("${PROJECT_SOURCE_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")``.

.. note::
  ClangFormat does not support a command line option for check ``--dry-run`` until version 10.
  This version is not widely used or available at this time. To work around this, we use an
  included script called ``run-clang-format.py`` that does not use ``PREPEND_FLAGS`` or ``APPEND_FLAGS``
  in the ``check`` build target because the script does not support command line flags passed
  to ``clang-format``. This script is not used in the ``style`` build target.


.. _blt_add_uncrustify_target:

blt_add_uncrustify_target
~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cmake

    blt_add_uncrustify_target( NAME              <Created Target Name>
                               MODIFY_FILES      [TRUE | FALSE (default)]
                               CFG_FILE          <Uncrustify Configuration File>
                               PREPEND_FLAGS     <Additional Flags to Uncrustify>
                               APPEND_FLAGS      <Additional Flags to Uncrustify>
                               COMMENT           <Additional Comment for Target Invocation>
                               WORKING_DIRECTORY <Working Directory>
                               SRC_FILES         [source1 [source2 ...]] )

Creates a new build target for running Uncrustify

NAME
  Name of created build target

MODIFY_FILES
  Modify the files in place. Defaults to FALSE.

CFG_FILE
  Path to Uncrustify config file

PREPEND_FLAGS
  Additional flags added to the front of the Uncrustify flags

APPEND_FLAGS
 Additional flags added to the end of the Uncrustify flags

COMMENT
  Comment prepended to the build target output

WORKING_DIRECTORY
  Directory in which the Uncrustify command is run. Defaults to where macro is called.

SRC_FILES
  Source list that Uncrustify will be ran on

Uncrustify is a Source Code Beautifier for C/C++ code. More information about
Uncrustify can be found `here <http://uncrustify.sourceforge.net/>`__.

When ``MODIFY_FILES`` is set to ``TRUE``, modifies the files in place and adds the created build
target to the parent ``style`` build target.  Otherwise the files are not modified and the
created target is added to the parent ``check`` build target. This target will notify you
which files do not conform to your style guide.

.. Note::
  Setting ``MODIFY_FILES`` to ``FALSE`` is only supported in Uncrustify v0.61 or greater.


.. _blt_add_yapf_target:

blt_add_yapf_target
~~~~~~~~~~~~~~~~~~~

.. code-block:: cmake

    blt_add_yapf_target( NAME              <Created Target Name>
                         MODIFY_FILES      [TRUE | FALSE (default)]
                         CFG_FILE          <Yapf Configuration File>
                         PREPEND_FLAGS     <Additional Flags to Yapf>
                         APPEND_FLAGS      <Additional Flags to Yapf>
                         COMMENT           <Additional Comment for Target Invocation>
                         WORKING_DIRECTORY <Working Directory>
                         SRC_FILES         [source1 [source2 ...]] )

Creates a new build target for running Yapf

NAME
  Name of created build target

MODIFY_FILES
  Modify the files in place. Defaults to FALSE.

CFG_FILE
  Path to Yapf config file

PREPEND_FLAGS
  Additional flags added to the front of the Yapf flags

APPEND_FLAGS
 Additional flags added to the end of the Yapf flags

COMMENT
  Comment prepended to the build target output

WORKING_DIRECTORY
  Directory in which the Yapf command is run. Defaults to where macro is called.

SRC_FILES
  Source list that Yapf will be ran on

Yapf is a Source Code Beautifier for Python code. More information about
Yapf can be found `here <https://github.com/google/yapf>`__.

When ``MODIFY_FILES`` is set to ``TRUE``, modifies the files in place and adds the created build
target to the parent ``style`` build target.  Otherwise the files are not modified and the
created target is added to the parent ``check`` build target. This target will notify you
which files do not conform to your style guide.


.. _blt_add_cmake_format_target:

blt_add_cmakeformat_target
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cmake

    blt_add_cmakeformat_target( NAME              <Created Target Name>
                                MODIFY_FILES      [TRUE | FALSE (default)]
                                CFG_FILE          <CMakeFormat Configuration File>
                                PREPEND_FLAGS     <Additional Flags to CMakeFormat>
                                APPEND_FLAGS      <Additional Flags to CMakeFormat>
                                COMMENT           <Additional Comment for Target Invocation>
                                WORKING_DIRECTORY <Working Directory>
                                SRC_FILES         [FILE1 [FILE2 ...]] )

Creates a new build target for running CMakeFormat

NAME
  Name of created build target

MODIFY_FILES
  Modify the files in place. Defaults to FALSE.

CFG_FILE
  Path to CMakeFormat config file

PREPEND_FLAGS
  Additional flags added to the front of the CMakeFormat flags

APPEND_FLAGS
 Additional flags added to the end of the CMakeFormat flags

COMMENT
  Comment prepended to the build target output

WORKING_DIRECTORY
  Directory in which the CMakeFormat command is run. Defaults to where macro is called.

SRC_FILES
  Source list that CMakeFormat will be ran on

CMakeFormat is a Source Code Beautifier for CMake code. More information about
CMakeFormat can be found `here <https://cmake-format.readthedocs.io/en/latest/index.html>`__.

When ``MODIFY_FILES`` is set to ``TRUE``, modifies the files in place and adds the created build
target to the parent ``style`` build target.  Otherwise the files are not modified and the
created target is added to the parent `check` build target. This target will notify you
which files do not conform to your style guide.
