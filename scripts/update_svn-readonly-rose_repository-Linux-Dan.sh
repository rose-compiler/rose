#!/bin/bash -x

ROSE_TOP="/home/dquinlan/ROSE/svn-test-rose/svn-readonly-rose"

# the right version of subversion is essential!!
SVN=/nfs/apps/subversion/1.5.5/bin/svn

# Check the EDG binaries already present
ls -l ${ROSE_TOP}/src/frontend/CxxFrontend

# Remove them so that we can cleanly update from the SVN repository
rm ${ROSE_TOP}/src/frontend/CxxFrontend/*.gz

# Update all of this checkedout version of ROSE
${SVN} update ${ROSE_TOP}

# Check the resulting binaries
ls -l ${ROSE_TOP}/src/frontend/CxxFrontend

# Also run the build script to generate the configure script
cd ${ROSE_TOP}; ./build
