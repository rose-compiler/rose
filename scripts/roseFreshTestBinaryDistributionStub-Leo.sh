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

# DQ (7/19/2008): Permissions were not reset after my 3rd striped disk was added! So use another directory for now.
ROSE_TOP="/home/liao6/daily-test-rose/svn-test-rose-alt/$$/"

CONFIGURE_FLAGS=(--with-edg_source_code=true --with-boost=${BOOST_ROOT} --enable-dq-developer-tests --with-ROSE_LONG_MAKE_CHECK_RULE=yes --with-CXX_DEBUG=-g --with-C_DEBUG=-g --with-CXX_WARNINGS=-Wall --prefix=${ROSE_TOP}/install "$@")
BINARY_CONFIGURE_FLAGS=(--with-boost=${BOOST_ROOT} --with-ROSE_LONG_MAKE_CHECK_RULE=yes --with-CXX_DEBUG=-g --with-C_DEBUG=-g --with-CXX_WARNINGS=-Wall --prefix=${ROSE_TOP}/install_binary "$@")

ROSE_SVNROOT=file:///usr/casc/overture/ROSE/svn/ROSE/trunk/ROSE
MAILADDRS="liaoch@llnl.gov"

# the right version of subversion is essential!!
SVN=/nfs/apps/subversion/1.4.5/bin/svn
SVNOP=checkout

MAKEFLAGS="-j16"

set -e
set -o pipefail
