#!/bin/bash -vx
# A simplest script to test the scidac external svn repository
# a given configuration: machine type and gcc version
# this is supposed to run from the top level rose source tree
# and called by Hudson

# Liao, 12/3/2009
# set up environment for each different platforms

# top rose git working copy, for hudson, it should be the hudson root directory?
# ROSE_TOP=/home/liao6/daily-test-rose/rose.git
MACHINE=`uname -m`
echo Found a $MACHINE ....
PROCESSE_NUM=4
# 32-bit machine--------
if  [ "$MACHINE" = "i686" ]; then
    # gcc version as an axis
    if [ "$GCC_VERSION" == "3.4" ]; then
      source /home/liao6/set.gcc3.4
    fi  
    if [ "$GCC_VERSION" == "4.0" ]; then
      source /usr/apps/gcc/4.0.4/setup.sh
    fi  
    if [ "$GCC_VERSION" == "4.1" ]; then
       # nothing to set for 4.1, the default gcc compilera
        echo "using the default gcc"
    fi
    if [ "$GCC_VERSION" == "4.2"]; then
      source /usr/apps/gcc/4.2.4/setup.sh
    fi  
    if [ "$GCC_VERSION" == "4.3" ]; then
      source /usr/apps/gcc/4.3.2/setup.sh
    fi  
    if [ "$GCC_VERSION" == "4.4" ]; then
      source /home/liao6/set.gcc4.4.1
    fi  
    echo `which gcc`
    source /usr/apps/qt/4.5.1/setup.sh
    export JAVA_HOME=/usr/apps/java/jdk1.6.0_11
    export BOOST_ROOT="/home/liao6/opt/boost_1_35_0"
    export LD_LIBRARY_PATH="/usr/apps/java/jdk1.6.0_11/lib:${JAVA_HOME}/jre/lib/i386/server:$LD_LIBRARY_PATH"
    export LD_LIBRARY_PATH="${BOOST_ROOT}/lib:${LD_LIBRARY_PATH}"
    export PATH="$JAVA_HOME/bin:/home/liao6/opt/git-1.6.5.2/bin:/usr/apps/automake/1.9.6/bin:$PATH"
    PROCESS_NUM=16
fi
# 64-bit machine--------
if  [ "$MACHINE" = "x86_64" ]; then
    export BOOST_ROOT=${OPT}/boost-1.36.0
    # gcc version as an axis
    if [ "$GCC_VERSION" == "3.4" ]; then
      source /home/liao6/set.gcc3.4
# gcc 3.4 needs a compatible boost library compiled with gcc 3.4
      export BOOST_ROOT=/nfs/casc/overture/ROSE/svn/apps/rh5-64/boost/1.35/gcc3.4
    fi  
    if [ "$GCC_VERSION" == "4.0" ]; then
      source /usr/apps/gcc/4.0.4/setup.sh
    fi  
    if [ "$GCC_VERSION" == "4.1" ]; then
       # nothing to set for 4.1, the default gcc compilera
        echo "using the default gcc"
    fi
    if [ "$GCC_VERSION" == "4.2"]; then
      source /usr/apps/gcc/4.2.4/setup.sh
    fi  
    if [ "$GCC_VERSION" == "4.3" ]; then
      source /usr/apps/gcc/4.3.2/setup.sh
    fi  
    if [ "$GCC_VERSION" == "4.4" ]; then
      source /usr/apps/gcc/4.4.1/setup.sh
    fi  
    echo `which gcc`
    OPT=/home/liao6/64home/opt
    export OPT
    export JAVA_HOME=${OPT}/jdk1.6.0_07
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:${JAVA_HOME}/jre/lib/amd64/server:${BOOST_ROOT}/lib
    export PATH=$JAVA_HOME/bin:/home/liao6/64home/opt/git-1.6.5.2/bin:$PATH
    PROCESS_NUM=4
fi

# TODO mac os machine--------

# prepare and 
# build source tree
test -d trunk && cd trunk || (echo "expecting trunk, but not found..." && echo `pwd`&& exit 1)
#test -f ChangeLog2
#if [ $? -ne 0 ]; then
#    echo `pwd`
#    echo "Error: the current directory does not seem to be a ROSE top source directory!!!"
#    exit 1
#fi 

#GIT_REPO_URL=`grep url .git/config | cut -f 3 -d' '`
#if [ "$GIT_REPO_URL" != "file:///nfs/casc/overture/ROSE/git/ROSE.git" ]; then
#    echo $GIT_REPO_URL
#    echo "Error: the git URL is not file:///nfs/casc/overture/ROSE/git/ROSE.git "
#    exit 1
#fi  
test -f configure
if [ $? -ne 0 ]; then
  echo "source tree need to be bootstrapped ..."
  ./build
fi

# minimum configuration options to build binaries
if test -e ROSE-build ; then chmod -R u+wx ROSE-build ; fi
rm -rf ROSE-build
mkdir ROSE-build

cd ROSE-build

../configure --with-boost=${BOOST_ROOT} --with-CXX_DEBUG=-g --with-CXX_WARNINGS=-Wall

make -j${PROCESS_NUM}
make check -j${PROCESS_NUM}

