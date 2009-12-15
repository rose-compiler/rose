#!/bin/bash -vx
# A hudson-friendly script to 
# 1) build a distribution package off the internal git repository's
# master branch, 
# 2) import the package to the external svn repository
# 3) update rosecompiler.org website
# 
# Liao 12/15/2009

# set up 32-bit environment
#--------------------------------------
set +e
source /usr/apps/qt/4.5.1/setup.sh  # this causes the script to abort if set +e is not used

export JAVA_HOME=/usr/apps/java/jdk1.6.0_11
export BOOST_ROOT="/home/liao6/opt/boost_1_35_0"
export LD_LIBRARY_PATH="/usr/apps/java/jdk1.6.0_11/lib:${JAVA_HOME}/jre/lib/i386/server:$LD_LIBRARY_PATH"
export LD_LIBRARY_PATH="${BOOST_ROOT}/lib:${LD_LIBRARY_PATH}"
export PATH="$JAVA_HOME/bin:/home/liao6/opt/git-1.6.5.2/bin:/usr/apps/automake/1.9.6/bin:$PATH:/home/liao6/opt/ghc-6.10.4/bin"
PROCESS_NUM=4

# prepare the source tree 
#--------------------------------------
# git pull # this will cause problem since hudson test may run on a ghost branch

set -e

PSEUDO_REV_NUM=`git log --pretty=oneline |wc -l`

ROSE_SRC_PATH=`pwd`
echo `pwd`
# always rebuild
./build

# build tree:  configure and build
#--------------------------------------
# always build fresh
if test -e ROSE-build ; then chmod -R u+w ROSE-build ; fi
rm -rf ROSE-build
mkdir ROSE-build

cd ROSE-build
ROSE_BUILD_PATH=`pwd`

# only reconfigure as needed
../configure --with-boost=${BOOST_ROOT} --with-CXX_DEBUG=-g --with-CXX_WARNINGS=-Wall --enable-dq-developer-tests --with-ROSE_LONG_MAKE_CHECK_RULE=yes --with-qt=/usr/apps/qt/4.5.1 --with-roseQt --with-haskell=/home/liao6/opt/ghc-6.10.4/bin --with-pch --with-gomp_omp_runtime_library=/home/liao6/opt/gcc-svn/lib/ --prefix=$HOME/.hudson/tempInstall

make -j${PROCESS_NUM} && \
make check -j${PROCESS_NUM} && \
make docs -j${PROCESS_NUM} &&  \
make dist -j${PROCESS_NUM} DOT_SVNREV=-$PSEUDO_REV_NUM && \
make distcheck -j${PROCESS_NUM}

# --------- update external svn repository
/home/liao6/rose/scripts/hudson/importRoseGitDistributionToSVN $ROSE_SOURCE_PATH $ROSE_BUILD_PATH

# --------- update external web site
make -C docs/Rose copyWebPages

# ---------update external file packages
# SKIPPED here 
# this is done by another weekly hudson job
# /home/liao6/rose/scripts/hudson/releaseRoseFilePackages.sh $ROSE_SOURCE_PATH $ROSE_BUILD_PATH

