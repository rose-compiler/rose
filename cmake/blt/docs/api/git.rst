.. # Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
.. # other BLT Project Developers. See the top-level LICENSE file for details
.. # 
.. # SPDX-License-Identifier: (BSD-3-Clause)

Git Macros
==========

.. _blt_git:

blt_git
~~~~~~~

.. code-block:: cmake

    blt_git(SOURCE_DIR      <dir>
            GIT_COMMAND     <command>
            OUTPUT_VARIABLE <out>
            RETURN_CODE     <rc>
            [QUIET] )

Runs the supplied git command on the given Git repository.

This macro runs the user-supplied Git command, given by ``GIT_COMMAND``, on the
given Git repository corresponding to ``SOURCE_DIR``. The supplied ``GIT_COMMAND``
is just a string consisting of the Git command and its arguments. The
resulting output is returned to the supplied CMake variable provided by
the ``OUTPUT_VARIABLE`` argument.

A return code for the Git command is returned to the caller via the CMake
variable provided with the ``RETURN_CODE`` argument. A non-zero return code
indicates that an error has occured.

Note, this macro assumes ``FindGit()`` was invoked and was successful. It relies
on the following variables set by ``FindGit()``:

    * ``Git_FOUND`` flag that indicates if git is found
    * ``GIT_EXECUTABLE`` points to the Git binary

If ``Git_FOUND`` is ``False`` this macro will throw a ``FATAL_ERROR`` message.

.. code-block:: cmake
    :caption: **Example**
    :linenos:

    blt_git( SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}
             GIT_COMMAND describe --tags master
             OUTPUT_VARIABLE axom_tag
             RETURN_CODE rc )
    if (NOT ${rc} EQUAL 0)
        message( FATAL_ERROR "blt_git failed!" )
    endif()


.. _blt_is_git_repo:

blt_is_git_repo
~~~~~~~~~~~~~~~

.. code-block:: cmake

    blt_is_git_repo(OUTPUT_STATE <state>
                    [SOURCE_DIR  <dir>] )

Checks if we are working with a valid Git repository.

This macro checks if the corresponding source directory is a valid Git repo.
Nominally, the corresponding source directory that is used is set to
``CMAKE_CURRENT_SOURCE_DIR``. A different source directory may be optionally
specified using the ``SOURCE_DIR`` argument.

The resulting state is stored in the CMake variable specified by the caller
using the ``OUTPUT_STATE`` parameter.

.. code-block:: cmake
    :caption: **Example**
    :linenos:

    blt_is_git_repo( OUTTPUT_STATE is_git_repo )
    if ( ${is_git_repo} )
        message(STATUS "Pointing to a valid Git repo!")
    else()
        message(STATUS "Not a Git repo!")
    endif()


.. _blt_git_tag:

blt_git_tag
~~~~~~~~~~~

.. code-block:: cmake

    blt_git_tag( OUTPUT_TAG  <tag>
                 RETURN_CODE <rc>
                 [SOURCE_DIR <dir>]
                 [ON_BRANCH  <branch>] )

Returns the latest tag on a corresponding Git repository.

This macro gets the latest tag from a Git repository that can be specified
via the ``SOURCE_DIR`` argument. If ``SOURCE_DIR`` is not supplied, the macro will
use ``CMAKE_CURRENT_SOURCE_DIR``. By default the macro will return the latest
tag on the branch that is currently checked out. A particular branch may be
specified using the ``ON_BRANCH`` option.

The tag is stored in the CMake variable specified by the caller using the
the ``OUTPUT_TAG`` parameter.

A return code for the Git command is returned to the caller via the CMake
variable provided with the ``RETURN_CODE`` argument. A non-zero return code
indicates that an error has occured.

.. code-block:: cmake
    :caption: **Example**
    :linenos:

    blt_git_tag( OUTPUT_TAG tag RETURN_CODE rc ON_BRANCH master )
    if ( NOT ${rc} EQUAL 0 )
        message( FATAL_ERROR "blt_git_tag failed!" )
    endif()
    message( STATUS "tag=${tag}" )


.. _blt_git_branch:

blt_git_branch
~~~~~~~~~~~~~~

.. code-block:: cmake

    blt_git_branch( BRANCH_NAME <branch>
                    RETURN_CODE <rc>
                    [SOURCE_DIR <dir>] )

Returns the name of the active branch in the checkout space.

This macro gets the name of the current active branch in the checkout space
that can be specified using the ``SOURCE_DIR`` argument. If ``SOURCE_DIR`` is not
supplied by the caller, this macro will point to the checkout space
corresponding to ``CMAKE_CURRENT_SOURCE_DIR``.

A return code for the Git command is returned to the caller via the CMake
variable provided with the ``RETURN_CODE`` argument. A non-zero return code
indicates that an error has occured.

.. code-block:: cmake
    :caption: **Example**
    :linenos:

    blt_git_branch( BRANCH_NAME active_branch RETURN_CODE rc )
    if ( NOT ${rc} EQUAL 0 )
        message( FATAL_ERROR "blt_git_tag failed!" )
    endif()
    message( STATUS "active_branch=${active_branch}" )


.. _blt_git_hashcode:

blt_git_hashcode
~~~~~~~~~~~~~~~~

.. code-block:: cmake

    blt_git_hashcode( HASHCODE    <hc>
                      RETURN_CODE <rc>
                      [SOURCE_DIR <dir>]
                      [ON_BRANCH  <branch>])

Returns the SHA-1 hashcode at the tip of a branch.

This macro returns the SHA-1 hashcode at the tip of a branch that may be
specified with the ``ON_BRANCH`` argument. If the ``ON_BRANCH`` argument is not
supplied, the macro will return the SHA-1 hash at the tip of the current
branch. In addition, the caller may specify the target Git repository using
the ``SOURCE_DIR`` argument. Otherwise, if ``SOURCE_DIR`` is not specified, the
macro will use ``CMAKE_CURRENT_SOURCE_DIR``.

A return code for the Git command is returned to the caller via the CMake
variable provided with the ``RETURN_CODE`` argument. A non-zero return code
indicates that an error has occured.

.. code-block:: cmake
    :caption: **Example**
    :linenos:

    blt_git_hashcode( HASHCODE sha1 RETURN_CODE rc )
    if ( NOT ${rc} EQUAL 0 )
        message( FATAL_ERROR "blt_git_hashcode failed!" )
    endif()
    message( STATUS "sha1=${sha1}" )

