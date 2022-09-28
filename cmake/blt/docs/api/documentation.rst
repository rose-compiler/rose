.. # Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
.. # other BLT Project Developers. See the top-level LICENSE file for details
.. #
.. # SPDX-License-Identifier: (BSD-3-Clause)

Documenation Macros
===================

.. _blt_add_doxygen_target:

blt_add_doxygen_target
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cmake

    blt_add_doxygen_target(doxygen_target_name)

Creates a build target for invoking Doxygen to generate docs. Expects to
find a ``Doxyfile.in`` in the directory the macro is called in.

This macro sets up the doxygen paths so that the doc builds happen
out of source. For ``make install``, this will place the resulting docs in
``docs/doxygen/<doxygen_target_name>``.


.. _blt_add_sphinx_target:

blt_add_sphinx_target
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cmake

    blt_add_sphinx_target(sphinx_target_name)

Creates a build target for invoking Sphinx to generate docs. Expects
to find a ``conf.py`` or ``conf.py.in`` in the directory the macro is called
in. Requires that a CMake variable named ``SPHINX_EXECUTABLE``
contains the path to the ``sphinx-build`` executable.

If ``conf.py`` is found, it is directly used as input to Sphinx.

If ``conf.py.in`` is found, this macro uses CMake's ``configure_file()`` command
to generate a ``conf.py``, which is then used as input to Sphinx.

This macro sets up the sphinx paths so that the doc builds happen
out of source. For ``make install``, this will place the resulting docs in
``docs/sphinx/<sphinx_target_name>``.
