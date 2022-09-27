.. # Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
.. # other BLT Project Developers. See the top-level LICENSE file for details
.. # 
.. # SPDX-License-Identifier: (BSD-3-Clause)

Release Process
===============

.. note:: No significant code development is performed on a release branch.
          In addition to preparing release notes and other documentation, the
          only code changes that should be done are bug fixes identified
          during release preparations

Here are the steps to follow when creating a BLT release.

1: Start Release Candidate Branch
---------------------------------

Create a release candidate branch off of the develop branch to initiate a
release. The name of a release branch must contain the associated release version
number. Typically, we use a name like v0.4.0-rc
(i.e., version 0.4.0 release candidate).

.. code:: bash

    git checkout -b v0.4.0-rc

2: Update Versions in Code
--------------------------

**Update BLT_VERSION**

   * ``SetupBLT.cmake``: ``set(BLT_VERSION "0.4.0" CACHE STRING "")``


**Update Release Notes**

#. Update ``RELEASE-NOTES.md`` by changing the 
   ``unreleased`` section from:

.. code:: text

    ## [Unreleased] - Release date yyyy-mm-dd

Also add to a versioned section with the current date while leaving the unreleased section:

.. code::

    ## [Unreleased] - Release date yyyy-mm-dd

    ## [Version 0.4.0] - Release date 2021-04-09

Finally, add a link to the bottom as well:

.. code:: text

    [Unreleased]:    https://github.com/LLNL/blt/compare/v0.3.6...develop

to:

.. code:: text

    [Unreleased]:    https://github.com/LLNL/blt/compare/v0.4.0...develop
    [Version 0.4.0]:    https://github.com/LLNL/blt/compare/v0.3.6...v0.4.0


3: Create Pull Request and push a git `tag` for the release
-----------------------------------------------------------

#. Commit the changes and push them to Github.
#. Create a pull request from release candidate branch to ``main`` branch.
#. Merge pull request after reviewed and passing tests.
#. Checkout main locally: ``git checkout main && git pull``
#. Create release tag:  ``git tag v0.4.0``
#. Push tag to Github: ``git push --tags``


4: Draft a Github Release
-------------------------

`Draft a new Release on Github <https://github.com/LLNL/blt/releases/new>`_

#. Enter the desired tag version, e.g., *v0.4.0*

#. Select **main** as the target branch to tag a release.

#. Enter a Release title with the same as the tag *v0.4.0*

#. Copy and paste the information for the release from the
   ``RELEASE-NOTES.md`` into the release description (omit any sections if empty).

#. Publish the release. This will add a corresponding entry in the
   `Releases section <https://github.com/LLNL/blt/releases>`_

.. note::

   Github will add a corresponding tarball and zip archives consisting of the
   source files for each release.


5: Create Release Branch and Mergeback to develop
-------------------------------------------------

1. Create a branch off main that is for the release branch.

.. code:: bash

    git pull
    git checkout main
    git checkout -b release-v0.4.0
    git push --set-upstream origin release-v0.4.0


2. Create a pull request to merge ``main`` into ``develop`` through Github. When approved, merge it.


7: Build Release Documentation
------------------------------

Enable the build on `readthedocs version page <https://readthedocs.org/projects/llnl-blt/versions/>`_
for the version branch created in step 5.
