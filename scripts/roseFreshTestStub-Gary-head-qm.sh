#!/bin/bash

unalias -a

source /usr/apps/mpich2/1.0.5/setup.sh
source /usr/apps/subversion/1.4.5/setup.sh
source /usr/apps/automake/1.9.6/setup.sh
source /usr/apps/autoconf/latest/setup.sh
source /usr/apps/graphviz/2.6/setup.sh
source /usr/apps/gcc/4.2.2/setup.sh

export JAVA_HOME=/usr/apps/java/jdk1.5.0_11

export LD_LIBRARY_PATH="/home/dquinlan/ROSE/OpenFortranParser?/ofp_install/lib:/usr/apps/java/jdk1.5.0_11/lib:/usr/apps/java/jdk1.5.0_11/jre/lib/i386/client:/home/yuan5/lib/boost_1_35_0/install/lib:${LD_LIBRARY_PATH}"

export PATH="/home/yuan5/lib/swig-1.3.36/install/bin:/usr/apps/java/jdk1.5.0_11/bin:${PATH}"

export BOOST_ROOTX="/home/yuan5/lib/boost_1_35_0/install"

export ROSE_TOP=/home/yuan5/tmp/fresh/`date +%F-%H%M`
#export ROSE_TOP=/home/yuan5/tmp/fresh/2008-09-14-0030
export LBL_RRQM_ROOT=liaoch@web-dev.nersc.gov:/www/host/rosecompiler/rrqm
source /home/yuan5/tmp/fresh/NIGHTLY_ROSE_Regression/scripts/rosetestrc

ROSE_SVNROOT=file:///usr/casc/overture/ROSE/svn/ROSE/trunk/ROSE
MAILADDRS="yuangm@gmail.com"

KEEP_TEST_DIR=1
NORMAL_INSTALL_DIR=1

SVNOP=checkout
SVN=/nfs/apps/subversion/1.4.5/bin/svn

CONFIGURE_FLAGS=( --with-edg_source_code=true --with-CXX_DEBUG=-g --with-C_DEBUG=-g --with-CXX_WARNINGS=-Wall --with-CXXFLAGS=-fPIC -with-CFLAGS=-fPIC --enable-ltdl-convenience --with-boost=${BOOST_ROOTX} --prefix=${ROSE_TOP}/install "$@")

KEEP_TEST_DIR=1
NORMAL_INSTALL_DIR=1 # no chmod 000 for 'install'
ENABLE_RRQM_PLUMHALL=1
ENABLE_RRQM_MOZILLA=1
#ENABLE_RRQM_PUBLISH=1
SKIP_DIST_TEST=1

MAKEFLAGS="-j2"

set -e
set -o pipefail
