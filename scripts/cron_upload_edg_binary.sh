#!/bin/sh -ex
#This script will generate EDG binaries for the current platform and upload them to the ROSE EDG BINARY DISTRIBUTION PATH.
#Never run this on a branch that has additional changes to master as it is Hudson that guarantees that what is on master is tested 
#and ready for distribution.

# DQ (2/5/2010): Added /usr/local/bin/ so that wget would be available.
# This script only runs on the MAC OSX machine (ninjai).
export PATH=$PATH:/Users/dquinlan/local/git-install/bin:/usr/local/bin/

if test -f .git/config; then
   echo "Verified that this is a git repository."
else
 # echo "This machine must be setup to access tux281.llnl.gov withouth passwd using ssh."
 # git clone ssh://tux281.llnl.gov/nfs/casc/overture/ROSE/git/ROSE.git git-rose
   echo "Error: this is not a git repository... (must be setup manually)."
   exit 1
fi

CONFIGURE_FLAGS="--with-boost=${ROSE_TEST_BOOST_PATH} --with-boost-libdir=${ROSE_TEST_BOOST_PATH}/lib "
# git fetch origin
if test "`git rev-parse HEAD`" != "`git rev-parse origin/master`" ; then
  git checkout master
  git pull
# git fetch origin
# git checkout origin/master
  git submodule update
  ./build
  if test -e ROSE-build ; then chmod -R u+w ROSE-build ; fi
  rm -rf ROSE-build
  mkdir ROSE-build
  cd ROSE-build
  ../configure ${CONFIGURE_FLAGS}
# make -j8 upload_edg_binary BINARY_EDG_TARBALL_STAGING_PATH=/nfs/casc/overture/ROSE/git/ROSE_EDG_Binaries
  make -j8 upload_edg_binary BINARY_EDG_TARBALL_STAGING_PATH=`pwd`
  scp roseBinaryEDG* dquinlan@tux281:/nfs/casc/overture/ROSE/git/ROSE_EDG_Binaries
fi

echo "EDG binaries built and uploaded to /nfs/casc/overture/ROSE/git/ROSE_EDG_Binaries on LC."

echo "**************************"
echo "Build terminated Normally."
echo "**************************"
