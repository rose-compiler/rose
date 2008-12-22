#!/bin/bash

# Stub file to include settings for roseFreshTest-common

source /usr/apps/graphviz/2.6/setup.sh
source /usr/apps/python/2.5.2/setup.sh
export JAVA_HOME=/usr/apps/java/jdk1.5.0_11
export LD_LIBRARY_PATH="${JAVA_HOME}/jre/lib/amd64/server:${LD_LIBRARY_PATH}"
# source /usr/apps/gcc/4.2.2/setup.sh
export PATH="/usr/apps/gcc/4.2.2-64bit/bin:$PATH"
export LD_LIBRARY_PATH="/usr/apps/gcc/4.2.2-64bit/lib:$LD_LIBRARY_PATH"
export PATH="/usr/apps/automake/1.9.6/bin:$PATH"
export PATH="/home/willcock2/swig-install/bin:$PATH"
export BOOST_ROOTX="/home/willcock2/boost-1.35-install64/"
export LD_LIBRARY_PATH="${BOOST_ROOTX}/lib:${LD_LIBRARY_PATH}"
# export LIBRARY_PATH="/usr/lib64"
# export PATH="/home/willcock2/tcl8.5-install/bin:$PATH"
# export LD_LIBRARY_PATH="/home/willcock2/tcl8.5-install/lib:$LD_LIBRARY_PATH"
# export PATH="/home/willcock2/binutils-install/bin/:$PATH"
# export LD_LIBRARY_PATH="/home/willcock2/binutils-install/lib/:$LD_LIBRARY_PATH"

ROSE_TOP="/export/0/tmp.willcock2/svn-test-rose/$$/"

CONFIGURE_FLAGS=( --enable-dq-developer-tests --with-ROSE_LONG_MAKE_CHECK_RULE=yes --with-CXX_DEBUG=-g --with-C_DEBUG=-g --with-CXX_WARNINGS='-Wall -Werror=format' --with-C_WARNINGS='-Wall -Werror=format' --with-CXXFLAGS=-fPIC --with-CFLAGS=-fPIC --prefix=${ROSE_TOP}/install --with-boost=${BOOST_ROOTX} "$@")

ROSE_SVNROOT=https://outreach.scidac.gov/svn/rose/trunk
MAILADDRS="liaoch@llnl.gov dquinlan@llnl.gov"
SKIP_DIST_TEST=1
# SKIP_BUILD_SOURCE=0

# the right version of subversion is essential!!
SVN=/home/willcock2/svn-install/bin/svn

MAKEFLAGS="-j8 -w"

set -e
set -o pipefail
