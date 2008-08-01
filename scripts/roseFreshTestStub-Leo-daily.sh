#!/bin/bash

# Stub file to include settings for roseFreshTest-common

source /usr/apps/graphviz/2.6/setup.sh
export PATH="/usr/apps/java/jdk1.5.0_11/bin:$PATH"
export LD_LIBRARY_PATH="/usr/apps/java/jdk1.5.0_11/lib:/usr/apps/java/jdk1.5.0_11/jre/lib/amd64/server:$LD_LIBRARY_PATH"
export JAVA_HOME=/usr/apps/java/jdk1.5.0_11
export CLASSPATH="/home/liao6/opt/antlr-3.0.1/lib/antlr-2.7.7.jar:/home/liao6/opt/antlr-3.0.1/lib/antlr-3.0.1.jar:/home/liao6/opt/antlr-3.0.1/lib/antlr-runtime-3.0.1.jar:/home/liao6/opt/antlr-3.0.1/lib/stringtemplate-3.1b1.jar:/home/dquinlan/ROSE/OpenFortranParser/ofp_install/lib/OpenFortranParser.jar:."


export LD_LIBRARY_PATH="/home/dquinlan/ROSE/OpenFortranParser/ofp_install/lib/:$LD_LIBRARY_PATH"
source /usr/apps/gcc/4.2.2/setup.sh
export PATH="/usr/apps/automake/1.9.6/bin:$PATH"
export BOOST_ROOT="/home/liao6/opt/boost_1_35_0"
export LD_LIBRARY_PATH="${BOOST_ROOT}/lib:${LD_LIBRARY_PATH}"

# using a time stamp to avoid overwriting previous tests
TIMESTAMP=$(date +%Y%m%d_%H%M%S) 
ROSE_TOP=/home/liao6/daily-test-rose/${TIMESTAMP}
#ROSE_TOP="/home/liao6/daily-test-rose/svn-test-rose-alt/$$/"

# for regular build and tests
CONFIGURE_FLAGS=(--with-edg_source_code=true --enable-rosehpct --with-boost=${BOOST_ROOT} --enable-dq-developer-tests --with-ROSE_LONG_MAKE_CHECK_RULE=yes --with-CXX_DEBUG=-g --with-C_DEBUG=-g --with-CXX_WARNINGS=-Wall --prefix=${ROSE_TOP}/install "$@")

# for release with EDG binaries
BINARY_CONFIGURE_FLAGS=(--with-boost=${BOOST_ROOT} --enable-rosehpct --with-ROSE_LONG_MAKE_CHECK_RULE=yes --with-CXX_DEBUG=-g --with-C_DEBUG=-g --with-CXX_WARNINGS=-Wall --prefix=${ROSE_TOP}/install_binary "$@")

ROSE_SVNROOT=file:///usr/casc/overture/ROSE/svn/ROSE/trunk/ROSE
MAILADDRS="liaoch@llnl.gov"

# the right version of subversion is essential!!
SVN=/nfs/apps/subversion/1.4.5/bin/svn
SVNOP=checkout
# check out the revision of the past 11:00pm PDT (06:00am UTC)
SVNVERSIONOPTION="`date -d '06:00Z' -u '+%FT%H:%M:%SZ'`"
#SVNVERSIONOPTION=1303

# How many processes for make
MAKEFLAGS="-j16"

# my additional stuff
ENABLE_UPLOAD=yes
KEEP_TEST_DIR=yes
NORMAL_INSTALL_DIR=yes # no chmod 000 for 'install'

set -e
set -o pipefail
