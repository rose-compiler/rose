#!/bin/bash
# Stub file to test the head of the external ROSE repository on tux242
# 10/24/2008

#required by make docs!
export JAVA_HOME="/home/liao6/64home/opt/jdk1.6.0_07"
export BOOST_ROOT="/home/liao6/64home/opt/boost-1.36.0"
export LD_LIBRARY_PATH="${JAVA_HOME}/lib:${JAVA_HOME}/jre/lib/amd64/server:$LD_LIBRARY_PATH"
export LD_LIBRARY_PATH="${BOOST_ROOT}/lib:${LD_LIBRARY_PATH}"
export PATH="${JAVA_HOME}/bin:$PATH:/home/liao6/64home/opt/graphviz-2.20.3/bin"
source /usr/apps/qt/4.5.1/setup.sh

# using a time stamp to avoid overwriting previous tests
TIMESTAMP=$(date +%Y%m%d_%H%M%S) 
ROSE_TOP=/home/liao6/daily-test-rose/${TIMESTAMP}

# for regular build and tests
CONFIGURE_FLAGS=( --with-boost=${BOOST_ROOT} --enable-dq-developer-tests --with-ROSE_LONG_MAKE_CHECK_RULE=yes --with-CXX_DEBUG=-g --with-CXX_WARNINGS=-Wall --with-qt=/usr/apps/qt/4.5.1 --prefix=${ROSE_TOP}/install "$@")

#ROSE_SVNROOT=file:///usr/casc/overture/ROSE/svn/ROSE/trunk/ROSE
ROSE_SVNROOT=https://outreach.scidac.gov/svn/rose/trunk
MAILADDRS="liaoch@llnl.gov"

# the right version of subversion is essential!!
SVN=/home/liao6/64home/opt/subversion-1.5.3/bin/svn
SVNOP=checkout

# check out the revision of the past 11:00pm PDT (06:00am UTC)
# using data as revision number needs {}
# default is head
#SVNVERSIONOPTION="{`date -d '06:00Z' -u '+%FT%H:%M:%SZ'`}"
#SVNVERSIONOPTION=1303

# How many processes for make
MAKEFLAGS="-j4"

# my additional stuff, not suitable for the external repository
#ENABLE_UPLOAD_SVN=1
#ENABLE_UPLOAD_WEB=1

#KEEP_TEST_DIR=1
NORMAL_INSTALL_DIR=1 # no chmod 000 for 'install'
# skip make dist, make distcheck, 
#SKIP_DIST_TEST=1
# end-users are not supposed to run ./build in source tree
#SKIP_BUILD_SOURCE=1
#make source_with_binary_edg_dist etc
#ENABLE_BUILD_BINARY_EDG=1
set -e
set -o pipefail
