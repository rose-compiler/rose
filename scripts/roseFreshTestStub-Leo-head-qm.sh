#!/bin/bash

# Stub file to include settings for roseFreshTest-common
source /home/liao6/rose/scripts/roseFreshTestStub-Leo-common.sh
#qmtest
source /usr/apps/xercesc/2.7.0/setup.sh

# using a time stamp to avoid overwriting previous tests
TIMESTAMP=$(date +%Y%m%d_%H%M%S) 
ROSE_TOP=/home/liao6/daily-test-rose/${TIMESTAMP}
#ROSE_TOP=/home/liao6/daily-test-rose/20080911_131715
export ROSE_TOP
LBL_RRQM_ROOT=liaoch@web-dev.nersc.gov:/www/host/rosecompiler/rrqm
export LBL_RRQM_ROOT 

# for regular build and tests
CONFIGURE_FLAGS=(--with-boost=${BOOST_ROOT} --enable-dq-developer-tests --with-ROSE_LONG_MAKE_CHECK_RULE=yes --with-CXX_DEBUG=-g --with-C_DEBUG=-g --with-CXX_WARNINGS=-Wall --prefix=${ROSE_TOP}/install "$@")

ROSE_SVNROOT=file:///usr/casc/overture/ROSE/svn/ROSE/trunk/ROSE
MAILADDRS="liaoch@llnl.gov yuan5@llnl.gov"

SVNOP=checkout
# check out the revision of the past 11:00pm PDT (06:00am UTC)
# using data as revision number needs {}
# default is head
#SVNVERSIONOPTION="{`date -d '06:00Z' -u '+%FT%H:%M:%SZ'`}"
#SVNVERSIONOPTION=1303

# How many processes for make
MAKEFLAGS="-j16"

#ENABLE_BUILD_BINARY_EDG=1

# my additional stuff
#ENABLE_UPLOAD_SVN=1
#ENABLE_UPLOAD_WEB=1

#KEEP_TEST_DIR=1
NORMAL_INSTALL_DIR=1 # no chmod 000 for 'install'

source /home/liao6/daily-test-rose/NIGHTLY_ROSE_Regression/scripts/rosetestrc
ENABLE_RRQM_PLUMHALL=1
ENABLE_RRQM_MOZILLA=1
ENABLE_RRQM_PUBLISH=1
SKIP_DIST_TEST=1
set -e
set -o pipefail
