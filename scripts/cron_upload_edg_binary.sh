#!/bin/sh -ex
#This script will generate EDG binaries for the current platform and upload them to the ROSE EDG BINARY DISTRIBUTION PATH.
#Never run this on a branch that has additional changes to master as it is Hudson that guarantees that what is on master is tested 
#and ready for distribution.

CONFIGURE_FLAGS="--with-boost=${ROSE_TEST_BOOST_PATH} --with-boost-libdir=${ROSE_TEST_BOOST_PATH}/lib "
git fetch origin
if test "`git rev-parse HEAD`" != "`git rev-parse origin/master`" ; then
  git checkout origin/master
  git submodule update
  ./build
  if test -e ROSE-build ; then chmod -R u+w ROSE-build ; fi
  rm -rf ROSE-build
  mkdir ROSE-build
  cd ROSE-build
  ../configure ${CONFIGURE_FLAGS}
# make -j8 upload_edg_binary BINARY_EDG_TARBALL_STAGING_PATH=/usr/casc/overture/ROSE/git/ROSE_EDG_Binaries
  make -j8 upload_edg_binary BINARY_EDG_TARBALL_STAGING_PATH=`pwd`
  scp roseBinaryEDG* dquinlan@tux281:/usr/casc/overture/ROSE/git/ROSE_EDG_Binaries
fi
