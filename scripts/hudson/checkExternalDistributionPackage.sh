#!/bin/bash -vx
# Given an external distribution package of ROSE ,
# check if it contains anything unsuitable for public release, including
#  EDG source file
#  EDG-SAGE connection
#  and others
# It also adds some extra stuff there
#  
# Liao, 12/14/2009

if [ $# -ne 3 ]
then
  echo This script needs three arguments to run.
  echo  the distribution package, the source tree, the build tree generating the package
  echo Example: $0 ROSE_DIST_NO_EDG_PACKAGE ROSE_SOURCE_TREE ROSE_BUILD_TREE
exit 1
fi
ROSE_DIST_NO_EDG_PACKAGE=$1
ROSE_SOURCE_TREE=$2
ROSE_BUILD_TREE=$3

set -e
# the input package must exist!
test -f $ROSE_DIST_NO_EDG_PACKAGE 
test -d $ROSE_SOURCE_TREE 
test -d $ROSE_BUILD_TREE 

echo distribution package is $ROSE_DIST_NO_EDG_PACKAGE
echo rose source tree is $ROSE_SOURCE_TREE
echo rose build tree is $ROSE_BUILD_TREE

ROSE_DISTRIBUTION=$ROSE_DIST_NO_EDG_PACKAGE

UPLOAD_DIR=${ROSE_BUILD_TREE}/upload
echo upload dir is $UPLOAD_DIR

rm -rf ${UPLOAD_DIR}
mkdir -p ${UPLOAD_DIR}


# unpack the package to the work dir and get the root dir of the distribution
#----------------------------------------------------
tar xzvf ${ROSE_DISTRIBUTION} -C ${UPLOAD_DIR}

cd ${UPLOAD_DIR}
ROSE_DIST_DIR=$(ls ${UPLOAD_DIR})
echo "Unpacked ROSE distribution directory is: ${ROSE_DIST_DIR}"

# Find all unwanted directories and remove them
#----------------------------------------------------
find . -name .svn | xargs rm -rf
find . -name Makefile.in~ | xargs rm -rf
find . -name autom4te.cache | xargs rm -rf

# remove any EDG source tree: EDG_SAGE_Connection, EDG_3.10, EDG_3.3 EDG_4.0 etc.
echo "Current directory is .."
echo `pwd`
rm -rf ${ROSE_DIST_DIR}/src/frontend/CxxFrontend/EDG/EDG_*

# remove configure and Makefile.in
rm -rf ${ROSE_DIST_DIR}/configure
rm -rf ${ROSE_DIST_DIR}/aclocal.m4
rm -rf ${ROSE_DIST_DIR}/rose_config.h.in
rm -rf ${ROSE_DIST_DIR}/libltdl
find . -name Makefile.in | xargs rm -rf
# remove generated scanners and parsers available from the source distribution
# But should not be put into the external repository
rm -rf ${ROSE_DIST_DIR}/src/frontend/SageIII/ompparser.cc
rm -rf ${ROSE_DIST_DIR}/src/frontend/SageIII/ompparser.h
rm -rf ${ROSE_DIST_DIR}/src/frontend/SageIII/omplexer.cc
rm -rf ${ROSE_DIST_DIR}/src/frontend/OpenFortranParser_SAGE_Connection/preproc-fortran.cc
rm -rf ${ROSE_DIST_DIR}/src/frontend/OpenFortranParser_SAGE_Connection/preproc-fortran-fixed.cc
rm -rf ${ROSE_DIST_DIR}/src/frontend/SageIII/preproc.cc
# dot2gml is not yet in distribution, added them here as a precaution 
rm -rf ${ROSE_DIST_DIR}/src/roseIndependentSupport/dot2gml/parseDotGrammar.cc
rm -rf ${ROSE_DIST_DIR}/src/roseIndependentSupport/dot2gml/parseDotGrammar.h
rm -rf ${ROSE_DIST_DIR}/src/roseIndependentSupport/dot2gml/parseDot.cc
rm -rf ${ROSE_DIST_DIR}/projects/DocumentationGenerator/doxygenComment.C
rm -rf ${ROSE_DIST_DIR}/src/midend/programAnalysis/annotationLanguageParser/language-lexer.cc
rm -rf ${ROSE_DIST_DIR}/src/midend/programAnalysis/annotationLanguageParser/language-parser.cc
rm -rf ${ROSE_DIST_DIR}/src/midend/programAnalysis/annotationLanguageParser/language.tab.h

# restore a few third party Makefile.in which cannot yet been automatically built
#cp ${ROSE_SOURCE_TREE}/src/3rdPartyLibraries/libharu-2.1.0/script/Makefile.in ${ROSE_DIST_DIR}/src/3rdPartyLibraries/libharu-2.1.0/script/Makefile.in
cp ${ROSE_SOURCE_TREE}/src/3rdPartyLibraries/libharu-2.1.0/Makefile.in ${ROSE_DIST_DIR}/src/3rdPartyLibraries/libharu-2.1.0/Makefile.in
cp ${ROSE_SOURCE_TREE}/src/3rdPartyLibraries/libharu-2.1.0/src/Makefile.in ${ROSE_DIST_DIR}/src/3rdPartyLibraries/libharu-2.1.0/src/Makefile.in
cp ${ROSE_SOURCE_TREE}/src/3rdPartyLibraries/libharu-2.1.0/include/Makefile.in ${ROSE_DIST_DIR}/src/3rdPartyLibraries/libharu-2.1.0/include/Makefile.in

# Make sure no EDG copyrighted files exist
#----------------------------------------------------

# We search for some representative source files of EDG
EDG_FILES=($(find . -name il_def.h -or -name cp_gen_be.c -or -name lower_il.h))
#EDG_FILES=($(find . -name Makefile.am))
if [ ${EDG_FILES[0]} ]; then
  echo Fatal Error: Found copyrighted EDG source files:${EDG_FILES[@]}
  exit 1
fi

# and the copyright string of EDG: "Proprietary information of Edison Design Group Inc."
set +e
EDG_COPYRIGHT_STRINGS=($(find . -name \*.C -or -name \*.h -or -name \*.c -or -name \*.cpp| xargs grep 'Proprietary information of Edison Design Group Inc.'))
set -e
if [ ${EDG_COPYRIGHT_STRINGS[0]} ]; then
  echo Fatal Error: Found copyrighted EDG text in source files:${EDG_COPYRIGHT_STRINGS[@]}
  exit 2
fi

# -------------------- we no longer put EDG binaries into the external svn repository--------
rm -rf ${ROSE_DIST_DIR}/src/frontend/CxxFrontend/roseBinaryEDG-*.tar.gz

