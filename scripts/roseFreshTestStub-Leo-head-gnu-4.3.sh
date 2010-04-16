#!/bin/bash

# Stub file to include settings for roseFreshTest-common
# this file stores all sharable environment variables for all test stubs on tux268
source /home/liao6/rose/scripts/roseFreshTestStub-Leo-common.sh
source /usr/apps/gcc/4.3.3/setup.sh

ROSE_SVNROOT=file:///usr/casc/overture/ROSE/svn/ROSE/trunk/ROSE
MAILADDRS="liaoch@llnl.gov"
#MAILADDRS="liaoch@llnl.gov panas2@llnl.gov quinlan1@llnl.gov matzke1@llnl.gov andreas.saebjoernsen@gmail.com"

# using a time stamp to avoid overwriting previous tests
TIMESTAMP=$(date +%Y%m%d_%H%M%S) 
#ROSE_TOP="/home/liao6/daily-test-rose/20090520_150650"
ROSE_TOP=/home/liao6/daily-test-rose/${TIMESTAMP}

# for regular build and tests
CONFIGURE_FLAGS=(--with-boost=${BOOST_ROOT} --with-CXX_DEBUG=-g --with-CXX_WARNINGS=-Wall --prefix=${ROSE_TOP}/install "$@")

SVNOP=update
# check out the revision of the past 11:00pm PDT (06:00am UTC)
# using data as revision number needs {}
# default is head
#SVNVERSIONOPTION="{`date -d '06:00Z' -u '+%FT%H:%M:%SZ'`}"
#SVNVERSIONOPTION=1303

# How many processes for make
MAKEFLAGS="-j8"
# If we are using update then assume build has been run...
SKIP_BUILD_SOURCE=1
# Skip running the distcheck rule (since we just want to build the compiler specific binary)
SKIP_DIST_TEST=1
#ENABLE_SVN_UPDATE=1

ENABLE_BUILD_BINARY_EDG=1
KEEP_TEST_DIR=0
BUILD_AND_UPDATE_EDG_BINARY_ONLY=1
# my additional stuff
NORMAL_INSTALL_DIR=1 # no chmod 000 for 'install'

set -e
set -o pipefail
