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
COMPASS_PROJECT=/home/dquinlan/ROSE/ROSE_CVS/ROSE/projects/compass/

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
# The updateCheckerList() function updates the file 
# projects/compass/CHECKER_LIST that contains a list of all checkers 'seen' by
# this script
#
###
updateCheckerList()
{
  local checker=$1
  local checker_list=$2
  local -i count=0

  local checker_exist=`grep "${checker}" $checker_list`

  if [[ $checker_exist == '' ]]; then
    echo "Updating ${checker} to $checker_list"
    echo "${checker}" >> $checker_list
  fi # We've got new checker!

} #updateCheckerList() <checker name> <checker list path>

###
#
# The generateCompassDocs() function automatically generates the file
# ${COMPASS_DOCS} that is to be included in projects/compass/compass.tex.in
# instead of the individual Compass Checker documentation
#
###
generateCompassDocs()
{
  local docs=$1

  if [[ -f $docs ]]; then
    echo "Removing old $docs..."
    rm -f $docs
  fi

  for dir in ${SUBDIRS[@]}
  do
    echo "" >> $docs
    echo -e "\\\newpage\n\\input{${dir}Docs}" >> $docs
  done
} #generateCompassDocs() <path to compass.tex.in>

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

###
#
# The generateCompassHeader() function generates the header file
# ${COMPASS_HEADER} that contains #include directives to the individual
# Compass checker header files
#
###
generateCompassHeader()
{
  local header=$1

  if [[ -f $header ]]; then
    echo "Removing old ${header}..."
    rm -f ${header}
  fi

  echo "// This is an automatically generated file" >> $header
  for dir in ${SUBDIRS[@]}
  do
    echo "#include \"${dir}.h\"" >> $header
  done
} #generateCompassHeader() <path to compass main header>

###
#
# The generateMakefile() function generates the file ${COMPASS_MAKEFILE}
# this is included into the Makefile.am of the projects/compass directory
#
###
generateMakefile()
{
  local makefile_am=$1
  local -i subbound=$((${#SUBDIRS[@]}-1))
  local libs=""
  local tests=""

  if [[ -f ${makefile_am} ]]; then
    echo "Removing old ${makefile_am}..."
    rm -f ${makefile_am}
  fi # if [[ -f ${makefile_am} ]]

  ## Comment block
  echo -e '# This is a generated file\n' >> ${makefile_am}

  ## COMPASS_INCLUDES
  echo -e 'COMPASS_INCLUDES = \' >> ${makefile_am}

  count=0
  for dir in ${SUBDIRS[@]}
  do
    if(( count < subbound )); then
      echo -e "\t-I\$(srcdir)/${dir} \\" >> ${makefile_am}
    else
      echo -e "\t-I\$(srcdir)/${dir}" >> ${makefile_am}
    fi
    ((count++))
  done

  ## Checker sources
  echo "" >> ${makefile_am}
  echo 'COMPASS_CHECKER_SRCS = \' >> ${makefile_am}
  count=0
  for dir in ${SUBDIRS[@]}
  do
    lib=${USUBDIRS[$((count))]}

    if(( count < subbound )); then
      echo -e "\t\$(srcdir)/${dir}/${dir}.C \\" >> ${makefile_am}
    else
      echo -e "\t\$(srcdir)/${dir}/${dir}.C" >> ${makefile_am}
    fi
    ((count++))
  done

  ## COMPASS_CHECKER_TESTS
  echo -e '\nCOMPASS_CHECKER_TESTS = \' >> ${makefile_am}
  count=0
  for dir in ${SUBDIRS[@]}
  do
    if(( count < subbound )); then
      echo -e "\t \$(srcdir)/${dir}/${dir}Test1.C \\" >> ${makefile_am}
    else
      echo -e "\t \$(srcdir)/${dir}/${dir}Test1.C" >> ${makefile_am}
    fi
    ((count++))
  done

  ## COMPASS_CHECKER_TEST_LABELS
  echo -e '\nCOMPASS_CHECKER_TEST_LABELS = \' >> ${makefile_am}

  count=0
  for dir in ${USUBDIRS[@]}
  do
    if(( count < subbound )); then
      echo -e "\ttest${dir} \\" >> ${makefile_am}
    else
      echo -e "\ttest${dir}" >> ${makefile_am}
    fi
    ((count++))
  done

  ## COMPASS_CHECKER_TEX_FILES
  echo -e '\nCOMPASS_CHECKER_TEX_FILES = \' >> ${makefile_am}

  count=0
  for dir in ${SUBDIRS[@]}
  do
    if(( count < subbound )); then
      echo -e "\t${dir}Docs.tex \\" >> ${makefile_am}
    else
      echo -e "\t${dir}Docs.tex" >> ${makefile_am}
    fi
    ((count++))
  done

  ## docs label
  echo '
docs: compass.tex
	test -f fixme.sty || ln -s $(top_srcdir)/docs/Rose/fixme.sty fixme.sty
	test -f compassCheckerDocs.tex || ln -s $(srcdir)/compassCheckerDocs.tex compassCheckerDocs.tex
	test -f acknowledgments.tex || ln -s $(srcdir)/acknowledgments.tex acknowledgments.tex
	test -f introduction.tex || ln -s $(srcdir)/introduction.tex introduction.tex
	test -f addYourOwnDetector.tex || ln -s $(srcdir)/addYourOwnDetector.tex addYourOwnDetector.tex' >> ${makefile_am}

  for dir in ${SUBDIRS[@]}
  do
    tex=${dir}Docs.tex
    echo -e "\ttest -f ${tex} || ln -s \$(srcdir)/${dir}/${tex}" >> ${makefile_am}
  done

  echo -e '\ttest -f compassDocs.tex || ln -s $(srcdir)/compassSupport/compassDocs.tex compassDocs.tex
	pdflatex compass.tex' >> ${makefile_am}

  echo "" >> ${makefile_am}

  ## test{CHECKER} label
  count=0
  for dir in ${SUBDIRS[@]}
  do
    echo -e "test${USUBDIRS[$((count))]}: compassMain" >> ${makefile_am}
    echo -e "\t cp \$(srcdir)/compass_parameters compass_parameters" >> ${makefile_am}
    echo -e "\t ./compassMain \$(srcdir)/${dir}/${dir}Test1.C\n" >> ${makefile_am}
    ((count++))
  done

  ## testAllCheckersSeparately label
  echo -e "\ntestAllCheckersSeparately: \$(COMPASS_CHECKER_TEST_LABELS)\n\t@echo \"\$@ done\"" >> ${makefile_am}

} #generateMakefileAM()

################################################################################
################################################################################
# stuff needs to be done

if [[ ! -f ${COMPASS_LIST} ]]; then
  touch ${COMPASS_LIST}
fi # if [[ ! -f ${COMPASS_LIST} ]]

################################################################################

for checker in $NEW_CHECKERS
do
    checker_directory=`tar ztf ${checker} | head -1`
    tar -C /tmp -zxf ${checker}

    if [[ -d ${COMPASS_PROJECT}/${checker_directory} ]]; then
        echo "Removing old copy of ${COMPASS_PROJECT}/${checker_directory}..."
        rm -rf ${COMPASS_PROJECT}/${checker_directory}
    fi

    echo "Moving ${checker_directory} to ${COMPASS_PROJECT}..."
    mv -f /tmp/${checker_directory} ${COMPASS_PROJECT}

    updateCheckerList ${checker_directory:0:((${#checker_directory}-1))} ${COMPASS_LIST}
done
# for checker in $NEW_CHECKERS do; ...; done
# Updates the compass project directory specified by ${COMPASS_PROJECT} with
# the submitted checkers in ${CHECKIN_DIRECTORY}.
# The list of all checkers ${COMPASS_LIST} is also updated.

for checker in `cat ${COMPASS_LIST}`
do
  USUBDIRS[$((SUBDIR_COUNT))]=`echo ${checker:0:1} | gawk '{print toupper($0)}'`${checker:1}
  SUBDIRS[$((SUBDIR_COUNT++))]=$checker
done

generateMakefile ${COMPASS_MAKEINC}
generateCompassHeader ${COMPASS_HEADER}
generateCompassSource ${COMPASS_SOURCE}
generateCompassDocs ${COMPASS_DOCS}
