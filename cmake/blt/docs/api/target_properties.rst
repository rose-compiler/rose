.. # Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
.. # other BLT Project Developers. See the top-level LICENSE file for details
.. # 
.. # SPDX-License-Identifier: (BSD-3-Clause)

Target Property Macros
======================

.. _blt_add_target_compile_flags:

blt_add_target_compile_flags
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cmake

    blt_add_target_compile_flags( TO    <target>
                                  SCOPE <PUBLIC (Default)| INTERFACE | PRIVATE>
                                  FLAGS [FOO [BAR ...]])

Appends compiler flags to a CMake target by appending to the target's existing flags.

TO
  Name of CMake target

SCOPE
  Defines the scope of the given flags. Defaults to PUBLIC and is case insensitive.

FLAGS
  List of compile flags

This macro provides very similar functionality to CMake's native 
``add_compile_options()`` and ``target_compile_options()`` commands, but
provides more fine-grained scoping for the compile flags on a
per target basis.

The given target must be added via CMake's ``add_executable()`` or ``add_library()`` commands
or with the corresponding :ref:`blt_add_executable` and :ref:`blt_add_library` macros.

``PRIVATE`` flags are used for the given target. ``INTERFACE`` flags are inherited
by any target that depends on this target. ``PUBLIC`` flags are both ``INTERFACE`` and ``PRIVATE``.

.. note::
   This macro will strip away leading and trailing whitespace from each flag.


.. _blt_add_target_definitions:

blt_add_target_definitions
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cmake

    blt_add_target_definitions( TO    <target>
                                SCOPE <PUBLIC (Default)| INTERFACE | PRIVATE>
                                TARGET_DEFINITIONS [FOO [BAR ...]])

Appends pre-processor definitions to the given target's existing flags.

TO
  Name of CMake target

SCOPE
  Defines the scope of the given definitions. Defaults to ``PUBLIC`` and is case insensitive.

TARGET_DEFINITIONS
  List of definitions flags

This macro provides very similar functionality to CMake's native 
``add_definitions()`` and ``target_add_definitions()`` commands, but provides
more fine-grained scoping for the compile definitions on a per target basis.
Given a list of definitions, e.g., ``FOO`` and ``BAR``, this macro adds compiler
definitions to the compiler command for the given target, i.e., it will pass
``-DFOO`` and ``-DBAR``.

The given target must be added via CMake's ``add_executable()`` or ``add_library()`` commands
or with the corresponding :ref:`blt_add_executable` and :ref:`blt_add_library` macros.

``PRIVATE`` flags are used for the given target. ``INTERFACE`` flags are inherited
by any target that depends on this target. ``PUBLIC`` flags are both ``INTERFACE`` and ``PRIVATE``.

.. note::
   The target definitions can either include or omit the "-D" characters. 
   E.g. the following are all valid ways to add two compile definitions 
   (A=1 and B) to target ``foo``.

.. note::
   This macro will strip away leading and trailing whitespace from each definition.

.. code-block:: cmake
    :caption: **Example**
    :linenos:

    blt_add_target_definitions(TO foo TARGET_DEFINITIONS A=1 B)
    blt_add_target_definitions(TO foo TARGET_DEFINITIONS -DA=1 -DB)
    blt_add_target_definitions(TO foo TARGET_DEFINITIONS "A=1;-DB")
    blt_add_target_definitions(TO foo TARGET_DEFINITIONS " " -DA=1;B)


.. _blt_add_target_link_flags:

blt_add_target_link_flags
~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cmake

    blt_add_target_link_flags( TO    <target>
                               SCOPE <PUBLIC (Default)| INTERFACE | PRIVATE>
                               FLAGS [FOO [BAR ...]])

Appends linker flags to a the given target's existing flags.

TO
  Name of CMake target

SCOPE
  Defines the scope of the given flags. Defaults to PUBLIC and is case insensitive.

FLAGS
  List of linker flags

This macro provides very similar functionality to CMake's native 
``add_link_options()`` and ``target_link_options()`` commands, but provides
more fine-grained scoping for the compile definitions on a per target basis.

The given target must be added via CMake's ``add_executable()`` or ``add_library()`` commands
or with the corresponding :ref:`blt_add_executable` and :ref:`blt_add_library` macros.

``PRIVATE`` flags are used for the given target. ``INTERFACE`` flags are inherited
by any target that depends on this target. ``PUBLIC`` flags are both ``INTERFACE`` and ``PRIVATE``.

If ``CUDA_LINK_WITH_NVCC`` is set to ``ON``, this macro will automatically
convert ``-Wl,-rpath,`` to :literal:`-Xlinker -rpath -Xlinker\ `.

.. note::
   This macro also handles the various changes that CMake made in 3.13.  For example,
   the target property ``LINK_FLAGS`` was changes to ``LINK_OPTIONS`` and was changed from a
   string to a list. New versions now support Generator Expressions.  Also pre-3.13,
   there were no macros to add link flags to targets so we do this by setting the properties
   directly.

.. note::
   In CMake versions prior to 3.13, this list is converted to a string internally
   and any ; characters will be removed.

.. note::
   In CMake versions 3.13 and above, this list is prepended with ``SHELL:`` which stops
   CMake from de-duplicating flags.  This is especially bad when linking with NVCC when 
   you have groups of flags like ``-Xlinker -rpath -Xlinker <directory>``.


.. _blt_print_target_properties:

blt_print_target_properties
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cmake

    blt_print_target_properties(TARGET <target>)

Prints out all properties of the given target.

TARGET
  Name of CMake target

The given target must be added via ``add_executable()`` or ``add_library()`` or
with the corresponding :ref:`blt_add_executable`, :ref:`blt_add_library`, 
:ref:`blt_import_library`, or :ref:`blt_register_library` macros.

Output is of the form for each property:
 | [<target> property] <property>: <value>


.. _blt_set_target_folder:

blt_set_target_folder
~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cmake

    blt_set_target_folder( TARGET <target>
                           FOLDER <folder>)

Sets the FOLDER property of the given CMake target.

TARGET
  Name of CMake target

FOLDER
  Name of the folder

This is used to organize properties in an IDE.

This feature is only available when BLT's ``ENABLE_FOLDERS`` option is ``ON`` and 
in CMake generators that support folders (but is safe to call regardless
of the generator or value of ``ENABLE_FOLDERS``).

.. note::
  Do not use this macro on header-only, ``INTERFACE`` library targets, since 
  this will generate a CMake configuration error.

