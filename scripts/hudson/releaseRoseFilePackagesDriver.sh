#!/bin/bash
# A hudson-friendly script to 
# x) build a distribution package off the internal git repository's
# master branch, 
# x) tag the internal git with a numeric release number
# x) release a file package for ROSe
# 
# Liao 12/18/2009

# set up 64-bit environment
# So this script should be tied to a 64-bit machine
#--------------------------------------
set +e
source /usr/apps/qt/4.5.1/setup.sh  # this causes the script to abort if set +e is not used

export JAVA_HOME=/usr/apps/java/jdk1.6.0_11
export BOOST_ROOT=/export/tmp.hudson-rose/opt/boost_1_40_0-inst
export LD_LIBRARY_PATH="/usr/apps/java/jdk1.6.0_11/lib:${JAVA_HOME}/jre/lib/amd64/server:$LD_LIBRARY_PATH"
export LD_LIBRARY_PATH="${BOOST_ROOT}/lib:${LD_LIBRARY_PATH}"
export PATH="$JAVA_HOME/bin:/export/tmp.hudson-rose/opt/git-1.6.5.2-inst/bin:/usr/apps/automake/1.9.6/bin:$PATH"
PROCESS_NUM=8

# sometimes, we want to skip the time consuming part
# and reuse previous build to test this script
SKIP_COMPILATION=0

# prepare the source tree 
#--------------------------------------
# git pull # this will cause problem since hudson test may run on a ghost branch

set -e

PSEUDO_REV_NUM=`git log --pretty=oneline |wc -l`

## tag the repository
EXIST_TAG=`git tag -l $PSEUDO_REV_NUM`

if [ "x$EXIST_TAG" != "x$PSEUDO_REV_NUM" ]; then
  git tag -a -m "pseudo release revision number" $PSEUDO_REV_NUM
fi
# verify the tag match the head hash
TAGGED_HASH=`git show $PSEUDO_REV_NUM | grep ^commit | cut -f 2 -d \ `
HEAD_HASH=`git rev-parse HEAD`
if [ "x$TAGGED_HASH" != "x$HEAD_HASH" ] ; then
  echo fatal error: the tag is associated with a different commit from the current head!!
  echo tagged hash is $TAGGED_HASH
  echo head hash is $HEAD_HASH 
  exit 3
fi 

## push to the remote repository
#git push --tags
git push origin $PSEUDO_REV_NUM

ROSE_SOURCE_PATH=`pwd`
echo `pwd`

if [ -d ${ROSE_SOURCE_PATH} ]; then
  if [ -f ${ROSE_SOURCE_PATH}/build ] ;
   then
     echo ${ROSE_SOURCE_PATH} is verified to be a rose source tree
   else
     echo Fatal error: ${ROSE_SOURCE_PATH} does not seem to be a rose source tree!
     exit 3
  fi
else
  echo Fatal error: ${ROSE_SOURCE_PATH} does not exist!
  exit 3
fi

# always rebuild
if [ $SKIP_COMPILATION -ne 1 ]; then
./build
fi


# build tree:  configure and build
#--------------------------------------
# always build fresh
if [ $SKIP_COMPILATION -ne 1 ]; then
  if test -e ROSE-build ; then chmod -R u+wx ROSE-build ; fi
  rm -rf ROSE-build
  mkdir ROSE-build
fi

cd ROSE-build
ROSE_BUILD_PATH=`pwd`

if [ $SKIP_COMPILATION -ne 1 ]; then
  # only reconfigure as needed
#  ../configure --with-boost=${BOOST_ROOT} --with-CXX_DEBUG=-g --with-qt=/usr/apps/qt/4.5.1 --with-roseQt --with-haskell=/home/liao6/opt/ghc-6.10.4/bin --prefix=$HOME/.hudson/tempInstall
  ../configure --with-boost=${BOOST_ROOT} --with-CXX_DEBUG=-g --with-qt=/usr/apps/qt/4.5.1 --with-roseQt --prefix=$HOME/.hudson/tempInstall
  
  make -j${PROCESS_NUM} && \
  make check -j${PROCESS_NUM} && \
  make dist -j${PROCESS_NUM} DOT_SVNREV=-$PSEUDO_REV_NUM && \
  make distcheck -j${PROCESS_NUM}
fi

if [ $? -ne 0  ]  ; then
  echo  "Fatal error during make and make dist, aborting ..."
  exit 3
fi

# ---------update external file packages
${ROSE_SOURCE_PATH}/scripts/hudson/releaseRoseFilePackages.sh $ROSE_SOURCE_PATH $ROSE_BUILD_PATH

# --------- update external svn repository
# this script can tolerate redundant update (trying to update the same package more than once is OK)
#/home/liao6/rose/scripts/hudson/importRoseGitDistributionToSVN $ROSE_SOURCE_PATH $ROSE_BUILD_PATH
# --------- update external web site
#if [ $SKIP_COMPILATION -ne 1 ]; then
# make -C docs/Rose copyWebPages
#fi
# build tree should be clean
#if [ -d ${ROSE_BUILD_PATH} ]; then
#  if [ -f ${ROSE_BUILD_PATH}/rose_config.h ] ;
#   then
#     echo ${ROSE_BUILD_PATH} is verified to be a rose build tree
#   else
#     echo Fatal error: ${ROSE_BUILD_PATH} does not seem to be a rose build tree!
#     exit 3
#  fi
#else
#  echo Fatal error: ${ROSE_BUILD_PATH} does not exist!
#  exit 3
#fi



