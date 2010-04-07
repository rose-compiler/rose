#!/bin/sh

# This is a script to automatically release/upload a ROSE package to
# SciDAC Outreach Center
# The assumption is that the distribution package is already built
# and the corresponding source and build trees are there. 
# It will
#  a) do sanity check of the distribution candidate
#    as a distribution package. Makefile.in and other generated file should exist!!
#  b) automatically fill out a web form to upload the package and release it.
# by Liao
# 12/15/2009

if [ $# -ne 2 ]
then
  echo This script needs two arguments to run.
  echo Syntax: $0 ROSE_SOURCE_PATH ROSE_BUILD_PATH
  echo Example: $0 /home/aaa/rose /home/aaa/buildrose
  exit 1
fi

# grab and verify the input parameters
ROSE_SOURCE_PATH=$1
ROSE_BUILD_PATH=$2

if [ -d ${ROSE_SOURCE_PATH} ]; then
  if [ -f ${ROSE_SOURCE_PATH}/rose_config.h.in ] ;
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

if [ -d ${ROSE_BUILD_PATH} ]; then
  if [ -f ${ROSE_BUILD_PATH}/rose_config.h ] ;
   then
     echo ${ROSE_BUILD_PATH} is verified to be a rose build tree
   else
     echo Fatal error: ${ROSE_BUILD_PATH} does not seem to be a rose build tree!
     exit 3
  fi
else
  echo Fatal error: ${ROSE_BUILD_PATH} does not exist!
  exit 3
fi

# ---------find the distribution package from the build tree
# find the form of rose-version-rev.tar.gz
# find the full path and name for the package to be uploaded
cd ${ROSE_BUILD_PATH}
ROSE_DIST_NO_EDG_PACKAGE_NAME=$(find . -maxdepth 1  -name  rose-\*-\*.tar.gz)
if [ $? -ne 0 ]; then
     echo "Fatal error: cannot find the distribution package!"
     exit 1
fi

# we only need the base name
ROSE_DIST_NO_EDG_PACKAGE_BASE_NAME=`basename $ROSE_DIST_NO_EDG_PACKAGE_NAME .tar.gz`
test -f $ROSE_BUILD_PATH/$ROSE_DIST_NO_EDG_PACKAGE_NAME || exit 1

# How many types of platforms to support
# Let's try the lightweight release first!!
EDG_BIN_COUNT=0

#----------real execution ----------------------
#----------get revision no etc ----------------------
# x. grab package name, version no, and revision no from 
# the log of the head version
LOG_TEXT=$ROSE_DIST_NO_EDG_PACKAGE_BASE_NAME

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

echo "The detected version number is $VERSION_NO"
echo "The detected pseudo revision number is $REVISION_NO"

#---------- upload the distribution package------------
# find the distribution package from top/build

# in case we want to use this script on both internal and external repository
# they have different file names for the distributions
#ROSE_DISTRIBUTION=$(find build -name  \*source-with-EDG-binary\*.tar.gz)
#ROSE_DISTRIBUTION=$(find . -name rose-${VERSION_NO}-${REVISION_NO}.tar.gz)
#ROSE_DISTRIBUTION=$(find . -name rose-${VERSION_NO}-${REVISION_NO}.tar.gz)
ROSE_DISTRIBUTION=$ROSE_DIST_NO_EDG_PACKAGE_NAME
if [ $? -ne 0 ]; then
   echo "Fatal error: cannot find the distribution package!"
   exit 1
fi

#------------ sanity check here!!------------------------
#----------------------------------------------------
# a temp place for sanity check
UPLOAD_DIR=${ROSE_BUILD_PATH}/upload
echo "upload dir is $UPLOAD_DIR"

rm -rf ${UPLOAD_DIR}
mkdir -p ${UPLOAD_DIR}

# unpack the package to the work dir and get the root dir of the distribution
#----------------------------------------------------
tar xzvf ${ROSE_DISTRIBUTION} -C ${UPLOAD_DIR} &>/dev/null
if [ $? -ne 0 ]; then
   echo "Fatal error for: tar xzvf ${ROSE_DISTRIBUTION} -C ${UPLOAD_DIR}"
   exit 1
fi

cd ${UPLOAD_DIR}

# get the full path to the unpacked distribution
ROSE_DIST_DIR=$(ls ${UPLOAD_DIR})
echo "Unpacked ROSE distribution directory is: ${ROSE_DIST_DIR}"

# Find all unwanted directories and remove them
#----------------------------------------------------
find . -name .svn | xargs rm -rf

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

# re-pack the package after sanity check and post-processing
# final file name to be uploaded 
# make sure its name is formalized as something like 
# rose-0.9.4a-source-with-EDG-binary-2759.tar.gz
cd ${UPLOAD_DIR}

# -------------------- we no longer put EDG binaries into the external svn repository--------
rm ${ROSE_DIST_DIR}/src/frontend/CxxFrontend/roseBinaryEDG-*.tar.gz


#------------create a new tar ball based on the cleaned up upload directory's content
FILE_NAME=${PACKAGE_NAME}-${VERSION_NO}-without-EDG-${REVISION_NO}.tar.gz
#FILE_NAME=${PACKAGE_NAME}-${VERSION_NO}-source-with-EDG-binary-${REVISION_NO}.tar.gz

# the untarred package should be rose-0.9.5a-8286
if [ -d ${PACKAGE_NAME}-${VERSION_NO}-${REVISION_NO} ]; then
  tar czvf "${FILE_NAME}" ${PACKAGE_NAME}-${VERSION_NO}-${REVISION_NO} &>/dev/null || {echo "error in tar czvf ...tar.gz packge; exit 1;"}
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

#-------------------- the actual uploading!!!---------------------
# call the upload script 
# VERSION_NO, REVISION_NO, FULL_PACKAGE_NAME
${ROSE_SOURCE_PATH}/scripts/hudson/finalUploadFilePackages.sh ${VERSION_NO} ${REVISION_NO} ${FULL_PACKAGE_NAME} 
if [ $? -ne 0 ]; then
   echo "Fatal error: cannot upload the distribution package!"
   exit 1
fi
