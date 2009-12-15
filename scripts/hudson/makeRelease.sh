#!/bin/bash
# Liao 12/8/2009
# Initial script to make external release packages
# it assume 
# Part of the work will be phased out once the external git repository is ready.

# set up 32-bit environment
#--------------------------------------
# source /usr/apps/qt/4.5.1/setup.sh  # this causes the script to abort

export JAVA_HOME=/usr/apps/java/jdk1.6.0_11
export BOOST_ROOT="/home/liao6/opt/boost_1_35_0"
export LD_LIBRARY_PATH="/usr/apps/java/jdk1.6.0_11/lib:${JAVA_HOME}/jre/lib/i386/server:$LD_LIBRARY_PATH"
export LD_LIBRARY_PATH="${BOOST_ROOT}/lib:${LD_LIBRARY_PATH}"
export PATH="$JAVA_HOME/bin:/home/liao6/opt/git-1.6.5.2/bin:/usr/apps/automake/1.9.6/bin:$PATH"
PROCESS_NUM=4

# prepare the source tree 
#--------------------------------------
# git pull # this will cause problem since hudson test may run on a branch

echo `pwd`
# always rebuild
./build

# build tree:  configure and build
#--------------------------------------
# always build fresh
rm -rf ROSE-build 
mkdir ROSE-build
cd ROSE-build
# only reconfigure as needed
../configure --with-boost=${BOOST_ROOT} --with-CXX_DEBUG=-g --with-CXX_WARNINGS=-Wall

make -j${PROCESS_NUM} && \
make check -j${PROCESS_NUM} && \
make docs -j${PROCESS_NUM} && \
make dist -j${PROCESS_NUM} && \
make distcheck -j${PROCESS_NUM}

