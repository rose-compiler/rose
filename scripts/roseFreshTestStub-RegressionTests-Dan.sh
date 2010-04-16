#!/bin/bash

# Stub file to include settings for roseFreshTest-common

export MPIPATH=/usr/apps/mpich2/1.0.5/bin/
export MPILIB=/usr/apps/mpich2/1.0.5/lib/

#source /usr/apps/mpich2/1.0.5/setup.sh
source /usr/apps/graphviz/2.6/setup.sh
export PATH="/usr/apps/java/jdk1.5.0_11/bin:${MPIPATH}:$PATH"
export LD_LIBRARY_PATH="/usr/apps/java/jdk1.5.0_11/lib:/usr/apps/java/jdk1.5.0_11/jre/lib/amd64/server:$LD_LIBRARY_PATH"
export JAVA_HOME=/usr/apps/java/jdk1.5.0_11
export CLASSPATH="/home/dquinlan/ANTLR/antlr-3.0/lib/antlr-3.0.jar:/home/dquinlan/ANTLR/antlr-3.0/lib/antlr-2.7.7.jar:/home/dquinlan/ANTLR/antlr-3.0/lib/stringtemplate-3.0.jar:/home/dquinlan/ROSE/OpenFortranParser/ofp_install/lib/OpenFortranParser.jar"
export LD_LIBRARY_PATH="/home/dquinlan/ROSE/OpenFortranParser/ofp_install/lib/:${MPILIB}:$LD_LIBRARY_PATH"
source /usr/apps/gcc/4.2.2/setup.sh
export PATH="/usr/apps/automake/1.9.6/bin:$PATH"
#export PATH="/home/dquinlan/swig-install/bin:$PATH"
export BOOST_ROOT="/home/dquinlan/local/boost_1_35_0_install"
export LD_LIBRARY_PATH="${BOOST_ROOT}/lib:${LD_LIBRARY_PATH}"

# DQ (7/19/2008): Permissions were not reset after my 3rd striped disk was added! So use another directory for now.
# ROSE_TOP="/home/dquinlan/ROSE/svn-test-rose/$$/"
ROSE_TOP="/home/dquinlan/ROSE/svn-test-rose-alt/$$/"
export ROSE_TOP

export LD_LIBRARY_PATH=/usr/apps/xercesc/2.7.0/lib/:${LD_LIBRARY_PATH}

CONFIGURE_FLAGS=(--with-edg_source_code=true --with-boost=${BOOST_ROOT} --enable-dq-developer-tests --with-ROSE_LONG_MAKE_CHECK_RULE=yes --with-CXX_DEBUG=-g --with-C_DEBUG=-g --with-CXX_WARNINGS=-Wall --prefix=${ROSE_TOP}/install "$@")
BINARY_CONFIGURE_FLAGS=(--with-boost=${BOOST_ROOT} --with-ROSE_LONG_MAKE_CHECK_RULE=yes --with-CXX_DEBUG=-g --with-C_DEBUG=-g --with-CXX_WARNINGS=-Wall --prefix=${ROSE_TOP}/install_binary "$@")

ROSE_SVNROOT=file:///usr/casc/overture/ROSE/svn/ROSE/trunk/ROSE
MAILADDRS="dquinlan@llnl.gov"

# the right version of subversion is essential!!
SVN=/nfs/apps/subversion/1.5.5/bin/svn
SVNOP=checkout

# Variables controling use of ROSE Regression Tests
KEEP_TEST_DIR=1
NORMAL_INSTALL_DIR=1 # no chmod 000 for 'install'
# ENABLE_RRQM=1
ENABLE_RRQM_PLUMHALL=1
ENABLE_RRQM_MOZILLA=1
# ENABLE_RRQM_PUBLISH=1
SKIP_DIST_TEST=1

export LBL_RRQM_ROOT=dquinlan@web-dev.nersc.gov:/www/host/rosecompiler/rrqm
source /home/dquinlan/ROSE/svn-test-rose-alt/ROSE_RegressionTests/scripts/rosetestrc

MAKEFLAGS="-j16"

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
