#!/bin/sh

# This is a script to automatically release a ROSE package.
# It will
#  a) make a distribution package off the head of the external repository
#     Choosing the external repository is safer compared using the internal one with EDG files
#     run a set of tests 
#  b) automatically fill out a web form to upload the package and release it.
# by Liao
# 10/29/2008
# set -e: will have problem with grep
#---------variable definition-------------
SVN=/nfs/apps/subversion/1.5.1/bin/svn

SOURCE_REPOS=https://outreach.scidac.gov/svn/rose/trunk
# top dir for rose's source and build directories 
# it follows our convention like: top/sourcetree  top/build top/install etc
# test on rev 2759 /home/liao6/daily-test-rose/20081029_050001
#
ROSE_TOP=/home/liao6/daily-test-rose/release
#ROSE_TOP=/home/liao6/daily-test-rose/20081029_050001

# How many types of platforms to support
EDG_BIN_COUNT=3
#The local directory with the scripts for our regression tests
SCRIPT_TOP=/home/liao6/rose/scripts

#----------real execution ----------------------
#----------get revision no etc ----------------------
# x. grab package name, version no, and revision no from 
# the log of the head version
# This will cause problem if a branch is the last changed thing. 
# The head log of main trunk will be empty
#LOG_TEXT=`$SVN log -r head $SOURCE_REPOS|grep '^Load rose'|cut -d' ' -f 2`
LOG_TEXT=`$SVN log -l 1 $SOURCE_REPOS|grep '^Load rose'|cut -d' ' -f 2`

if [ "x$LOG_TEXT" = "x" ]; then
  echo "Fatal error: cannot get the package-version-revision string!"
  exit 1
fi  

# LOG_TEXT should be something like: 
# rose-0.9.4a-2759 (package-version-revision) 
PACKAGE_NAME=`echo $LOG_TEXT| cut -d'-' -f 1`
if [ "x$PACKAGE_NAME" = "x" ]; then
  echo "Fatal error: cannot get  package string!"
  exit 1
fi  

VERSION_NO=`echo $LOG_TEXT| cut -d'-' -f 2`
if [ "x$VERSION_NO" = "x" ]; then
  echo "Fatal error: cannot get  version string!"
  exit 1
fi

REVISION_NO=`echo $LOG_TEXT| cut -d'-' -f 3`
if [ "x$REVISION_NO" = "x" ]; then
  echo "Fatal error: cannot get revision string!"
  exit 1
fi

#----------run a full round of test to generate the release package---------
# no need to transfer the revision number which is just obtained from the head
cd $SCRIPT_TOP
# TODO, real head later on, commented out for debugging
./roseFreshTest ./roseFreshTestStub-Leo-scidac-release.sh
if [ $? -ne 0 ]; then
   echo "Fatal error: cannot generate the distribution package!"
   exit 1
fi

#---------- upload the distribution package------------
# find the full path and name for the package to be uploaded
cd ${ROSE_TOP}
# find the distribution package from top/build

# in case we want to use this script on both internal and external repository
# they have different file names for the distributions
#ROSE_DISTRIBUTION=$(find build -name  \*source-with-EDG-binary\*.tar.gz)
ROSE_DISTRIBUTION=$(find build -name rose-${VERSION_NO}.tar.gz)

if [ ROSE_DISTRIBUTION[0] ]; then
    echo "Found rose distribution package:$ROSE_DISTRIBUTION"
else  
     echo "Fatal error: cannot find the distribution package!"
     exit 1
fi

#------------ sanity check here!!
# a temp place for sanity check
UPLOAD_DIR=${ROSE_TOP}/upload
rm -rf ${UPLOAD_DIR}
mkdir -p ${UPLOAD_DIR}

# unpack the package to the work dir and get the root dir of the distribution
#----------------------------------------------------
tar xzvf ${ROSE_DISTRIBUTION} -C ${UPLOAD_DIR} &>/dev/null

cd ${UPLOAD_DIR}

# Find all unwanted directories and remove them
#----------------------------------------------------
find . -name .svn | xargs rm -rf

# get the full path to the unpacked distribution
ROSE_DIST_DIR=$(ls ${UPLOAD_DIR})
echo "Unpacked ROSE distribution directory is: ${ROSE_DIST_DIR}"
# Make sure no EDG copyrighted files exist
#----------------------------------------------------

# We search for some representative source files of EDG
EDG_FILES=($(find . -name il_def.h -or -name cp_gen_be.c -or -name lower_il.h))
#EDG_FILES=($(find . -name Makefile.am))
if [ ${EDG_FILES[0]} ]; then
  echo Fatal Error: Found copyrighted EDG source files:${EDG_FILES[@]}
  exit 1
else
  echo "Made sure that there is No EDG source files.."
fi

# and the copyright string of EDG: "Proprietary information of Edison Design Group Inc."
pwd
EDG_COPYRIGHT_STRINGS=($(find . -name \*.C -or -name \*.h -or -name \*.c -or -name \*.cpp|xargs grep 'Proprietary information of Edison Design Group Inc.'))

if [ ${EDG_COPYRIGHT_STRINGS[0]} ]; then
  echo Fatal Error: Found copyrighted EDG text in source files:${EDG_COPYRIGHT_STRINGS[@]}
  exit 2
else
  echo "Double checked there is no copyrighted EDG text"
fi

# Make sure all binary EDG versions for three platforms exist:i686-apple, i686-redhat, 
# and x86_64-redhat, using a bash array (index starting from 0) to test it.
EDG_BINARIES=($(find . -name roseBinaryEDG-\*.tar.gz))
# array start from 0, so the third element is array[2]
if [ ${EDG_BINARIES[$EDG_BIN_COUNT-1]} ]; then
   echo Found the EDG binaries for $EDG_BIN_COUNT platfoms as expected. It is ${EDG_BINARIES[@]}. 
else 
  echo Fatal Error: Less than $EDG_BIN_COUNT platforms is supported. 
  exit 3
fi

# re-pack the package after sanity check and post-processing
# final file name to be uploaded 
# make sure its name is formalized as something like 
# rose-0.9.4a-source-with-EDG-binary-2759.tar.gz
cd ${UPLOAD_DIR}

FILE_NAME=${PACKAGE_NAME}-${VERSION_NO}-source-with-EDG-binary-${REVISION_NO}.tar.gz

if [ -d ${PACKAGE_NAME}-${VERSION_NO} ]; then
  tar czvf "${FILE_NAME}" ${PACKAGE_NAME}-${VERSION_NO} &>/dev/null || {echo "error in tar czvf ...tar.gz packge; exit 1;"}
else
  echo Fatal Error: No unpacked release package exists.
  exit 3
fi  

FULL_PACKAGE_NAME=$UPLOAD_DIR/$FILE_NAME
if [ -e $FULL_PACKAGE_NAME ] ; then  
  echo "Confirmed the existence of the ROSE distribution package: $FULL_PACKAGE_NAME"
else
   echo "Fatal error: cannot find the distribution package:$FULL_PACKAGE_NAME!"
   exit 1
fi  

# call the upload script 
# VERSION_NO, REVISION_NO, FULL_PACKAGE_NAME
${SCRIPT_TOP}/makeRelease.sh ${VERSION_NO} ${REVISION_NO} ${FULL_PACKAGE_NAME} 
if [ $? -ne 0 ]; then
   echo "Fatal error: cannot upload the distribution package!"
   exit 1
fi
