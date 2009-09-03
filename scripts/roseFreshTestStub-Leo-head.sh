#!/bin/bash

# Stub file to include settings for roseFreshTest-common

# this file stores all sharable environment variables for all test stubs on tux268
source /home/liao6/rose/scripts/roseFreshTestStub-Leo-common.sh

#required by make docs!
#source /usr/apps/subversion/1.5.5/setup.sh
#export XERCESCROOT=/home/liao6/opt/xerces-c-3.0.1
#source /usr/apps/graphviz/2.20.3/setup.sh
#
## QT and QROSE
#source /usr/apps/qt/4.5.1/setup.sh
#
#export JAVA_HOME=/usr/apps/java/jdk1.6.0_11
#export BOOST_ROOT="/home/liao6/opt/boost_1_35_0"
#export LD_LIBRARY_PATH="/usr/apps/java/jdk1.6.0_11/lib:${JAVA_HOME}/jre/lib/i386/server:$LD_LIBRARY_PATH"
#export LD_LIBRARY_PATH="${BOOST_ROOT}/lib:${LD_LIBRARY_PATH}"
#
##source /usr/apps/gcc/4.2.2/setup.sh
#export PATH="$JAVA_HOME/bin:/usr/apps/automake/1.9.6/bin:$PATH"
#
## the right version of subversion is essential!!
#SVN=/nfs/apps/subversion/1.5.5/bin/svn
## installation path of spec cpu and the config file for using rose
#SPEC_CPU2006_INS="/home/liao6/opt/spec_cpu2006"
#SPEC_CPU2006_CONFIG="tux268-rose-gcc42.cfg"
#-------------end of environment

ROSE_SVNROOT=file:///usr/casc/overture/ROSE/svn/ROSE/trunk/ROSE
MAILADDRS="liaoch@llnl.gov panas2@llnl.gov quinlan1@llnl.gov matzke1@llnl.gov"

# using a time stamp to avoid overwriting previous tests
TIMESTAMP=$(date +%Y%m%d_%H%M%S) 
#ROSE_TOP="/home/liao6/daily-test-rose/20090520_150650"
ROSE_TOP=/home/liao6/daily-test-rose/${TIMESTAMP}

# for regular build and tests
CONFIGURE_FLAGS=(--with-boost=${BOOST_ROOT} --enable-dq-developer-tests --with-ROSE_LONG_MAKE_CHECK_RULE=yes --with-CXX_DEBUG=-g --with-CXX_WARNINGS=-Wall --with-qt=/usr/apps/qt/4.5.1 --with-haskell=/home/liao6/opt/ghc-6.10.4/bin --prefix=${ROSE_TOP}/install "$@")

SVNOP=checkout
# check out the revision of the past 11:00pm PDT (06:00am UTC)
# using data as revision number needs {}
# default is head
#SVNVERSIONOPTION="{`date -d '06:00Z' -u '+%FT%H:%M:%SZ'`}"
#SVNVERSIONOPTION=1303

# How many processes for make
MAKEFLAGS="-j16"

ENABLE_BUILD_BINARY_EDG=1
KEEP_TEST_DIR=0
# my additional stuff
ENABLE_UPLOAD_SVN=1
ENABLE_UPLOAD_CHANGELOG2=1
ENABLE_UPLOAD_WEB=1
# using external benchmarks
ENABLE_EXTERNAL_TEST=1
#KEEP_TEST_DIR=1
NORMAL_INSTALL_DIR=1 # no chmod 000 for 'install'

set -e
set -o pipefail
