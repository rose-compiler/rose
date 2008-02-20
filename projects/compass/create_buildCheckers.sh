#!/bin/bash
################################################################################
#
# Author: Gary Yuan
# File: compass_submission_setup.sh
# Date: 7/19/2007
# Purpose: Automatically build compass from checker submit directory 
#
################################################################################

### GLOBAL VARIABLES

BACK=`pwd`
CHECKIN_DIRECTORY=~dquinlan/COMPASS_SUBMIT/
NEW_CHECKERS=`find ${CHECKIN_DIRECTORY} -maxdepth 1 -type f`
#COMPASS_PROJECT=/home/yuan5/ROSE/ROSE/projects/compass/
#COMPASS_PROJECT=/home/yuan5/new_compass/projects/compass/

# Location on Laptop
COMPASS_PROJECT=.

COMPASS_LIST=${COMPASS_PROJECT}/CHECKER_LIST
COMPASS_MAKEINC=${COMPASS_PROJECT}/compass_makefile.inc
COMPASS_HEADER=${COMPASS_PROJECT}/checkers.h
COMPASS_SOURCE=${COMPASS_PROJECT}/buildCheckers.C
COMPASS_DOCS=${COMPASS_PROJECT}/compassCheckerDocs.tex

declare -a SUBDIRS
declare -a USUBDIRS
declare -a SUBDIR_COUNT=0

################################################################################

###
#
# The generateCompassSource() function generates the .C file containing the
# function call to generate all Compass Checker Traversals. This is compiled
# with compassMain.C
#
###
generateCompassSource()
{
  local source=$1
  local -i num=0
  local -i i=0

  if [[ -f $source ]]; then
    echo "Removing old ${source}..."
    rm -f $source
  fi

echo -e '// This is an automatically generated file
#include <iostream>
#include <rose.h>
#include <vector>
#include "compass.h"
#include "checkers.h"

void
buildCheckers( std::vector<Compass::TraversalBase*> &retVal, Compass::Parameters &params, Compass::OutputObject &output )
{
' >> $source

for dir in ${USUBDIRS[@]}
do
  echo -e "  retVal.push_back( new CompassAnalyses::${dir}::Traversal(params, &output) );\n" >> $source
done

echo -e '
  return;
} //buildCheckers()
' >> $source

} #generateCompassSource() <path to compass main source>

for checker in `cat ${COMPASS_LIST}`
do
  USUBDIRS[$((SUBDIR_COUNT))]=`echo ${checker:0:1} | gawk '{print toupper($0)}'`${checker:1}
  SUBDIRS[$((SUBDIR_COUNT++))]=$checker
done

# generateMakefile ${COMPASS_MAKEINC}
# generateCompassHeader ${COMPASS_HEADER}
generateCompassSource ${COMPASS_SOURCE}
# generateCompassDocs ${COMPASS_DOCS}
