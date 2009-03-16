#!/bin/bash

# Stub file to include settings for roseFreshTest-common

export MPIPATH=/usr/apps/mpich2/1.0.5/bin/
export MPILIB=/usr/apps/mpich2/1.0.5/lib/

#source /usr/apps/mpich2/1.0.5/setup.sh
source /usr/apps/graphviz/2.6/setup.sh
export PATH="/usr/apps/java/jdk1.5.0_11/bin:/usr/apps/swig/1.3.36/bin:${MPIPATH}:$PATH"
export LD_LIBRARY_PATH="/usr/apps/java/jdk1.5.0_11/lib:/usr/apps/java/jdk1.5.0_11/jre/lib/amd64/server:$LD_LIBRARY_PATH"
export JAVA_HOME=/usr/apps/java/jdk1.5.0_11
#export CLASSPATH="/home/dquinlan/ANTLR/antlr-3.0/lib/antlr-3.0.jar:/home/dquinlan/ANTLR/antlr-3.0/lib/antlr-2.7.7.jar:/home/dquinlan/ANTLR/antlr-3.0/lib/stringtemplate-3.0.jar"
#export LD_LIBRARY_PATH="/home/dquinlan/ROSE/OpenFortranParser/ofp_install/lib/:${MPILIB}:$LD_LIBRARY_PATH"
export LD_LIBRARY_PATH="${MPILIB}:$LD_LIBRARY_PATH"

# DQ (9/17/2008): Test with the 4.1.2 to be a little different
source /usr/apps/gcc/4.2.2/setup.sh
# source /usr/apps/gcc/4.1.2/setup.sh

# Setup use of SWIG from non-default location
export PATH="/usr/apps/swig/1.3.36/bin:$PATH"

export PATH="/usr/apps/automake/1.9.6/bin:$PATH"
#export PATH="/home/dquinlan/swig-install/bin:$PATH"
#export BOOST_ROOT="/home/dquinlan/local/boost_1_35_0_install"
export BOOST_ROOT="/home/dquinlan/local/boost_1_37_0_installTree-gxx-4.2.2"
export LD_LIBRARY_PATH="${BOOST_ROOT}/lib:${LD_LIBRARY_PATH}"

# Use this directory for the location of all tests of ROSE.
ROSE_TOP="/home/dquinlan/ROSE/svn-test-rose/$$/"

# CONFIGURE_FLAGS=(--with-edg_source_code=true --with-boost=${BOOST_ROOT} --enable-dq-developer-tests --with-ROSE_LONG_MAKE_CHECK_RULE=yes --with-CXX_DEBUG=-g --with-C_DEBUG=-g --with-CXX_WARNINGS=-Wall --prefix=${ROSE_TOP}/install "$@")
CONFIGURE_FLAGS=(--with-boost=${BOOST_ROOT} --enable-dq-developer-tests --with-ROSE_LONG_MAKE_CHECK_RULE=yes --with-CXX_DEBUG=-g --with-C_DEBUG=-g --with-CXX_WARNINGS=-Wall --prefix=${ROSE_TOP}/install "$@")

# DQ (1/2/2009): Commenting this out causes the tests using ROSE/scripts/roseFreshTestBinaryDistribution to fail.
# BINARY_CONFIGURE_FLAGS=(--with-boost=${BOOST_ROOT} --with-ROSE_LONG_MAKE_CHECK_RULE=yes --with-CXX_DEBUG=-g --with-C_DEBUG=-g --with-CXX_WARNINGS=-Wall --prefix=${ROSE_TOP}/install_binary "$@")

ROSE_SVNROOT=file:///usr/casc/overture/ROSE/svn/ROSE/trunk/ROSE
MAILADDRS="dquinlan@llnl.gov liaoch@llnl.gov panas2@llnl.gov matzke1@llnl.gov saebjornsen1@llnl.gov"

# the right version of subversion is essential!!
SVN=/nfs/apps/subversion/1.4.5/bin/svn
SVNOP=checkout

# Run in parallel (using gmake option).
MAKEFLAGS="-j16"

# Controls generation and contribution of binary for EDG support to distribution.
ENABLE_BUILD_BINARY_EDG=1

# TP (31Jul2008) make sure that the mpi deamon runs in the background
export mpdvar=$(ps ax | grep mpd | wc -l )
#echo $mpdvar
if (($mpdvar >= 2)); then
:
#   echo "MPD RUNNING"
 elif (($mpdvar < 2)); then
#   echo "Starting MPD"
   mpd &
#   echo "MPD started."
fi

set -e
set -o pipefail
