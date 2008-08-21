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

ROSE_SVNROOT=file:///usr/casc/overture/ROSE/svn/ROSE/trunk/ROSE
MAILADDRS="yuangm@gmail.com"

#source /home/yuan5/tmp/09/ROSE_Regression/scripts/rosetestrc
export ROSE_TOP=${ROSE_TEST_REGRESSION_ROOT}/rose/fresh/`date +%F-%H%M`

MYCC="${ROSE_TEST_REGRESSION_ROOT}/scripts/utils/gcc"
MYCXX="${ROSE_TEST_REGRESSION_ROOT}/scripts/utils/g++"
#UTILS=${ROSE_TEST_REGRESSION_ROOT}/scripts/utils
MAKEFLAGS="CC=${MYCC} CXX=${MYCXX} CCLD=${MYCC} CXXLD=${MYCXX}"

KEEP_TEST_DIR=yes
NORMAL_INSTALL_DIR=yes

SVNOP=checkout
SVN=/nfs/apps/subversion/1.4.5/bin/svn

CONFIGURE_FLAGS=( --with-edg_source_code=true --with-CXX_DEBUG=-g --with-C_DEBUG=-g --with-CXX_WARNINGS=-Wall --with-CXXFLAGS=-fPIC -with-CFLAGS=-fPIC --enable-ltdl-convenience --enable-dq-developer-tests --with-ROSE_LONG_MAKE_CHECK_RULE=yes --with-boost=${BOOST_ROOTX} --prefix=${ROSE_TOP}/install "$@")

set -e
set -o pipefail
