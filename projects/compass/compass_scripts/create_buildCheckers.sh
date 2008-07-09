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
    echo "#include \"${dir}/${dir}.h\"" >> $header
  done
} #generateCompassHeader() <path to compass main header>

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

//AS(1/18/08) Read in the selecting of rules
std::map<std::string, bool > 
readFile( std::string filename){
  std::map<std::string, bool > checkersToUse;
  std::ifstream* file_op = Compass::openFile( filename );

  std::string current_word;
  bool is_first_word = true;
  //read file
  char dummyString[2000];

  while((*file_op) >> current_word){
	//First word denotes what the regular expression should operate
	//upon. Second word denotes the regular expression

	if(  current_word.substr(0,1) == std::string("#") ){
	  //Skip rest of the line if a "#" character is found. This denotes a 
	  //comment
	  file_op->getline(dummyString,2000);

	}else{
	  if(current_word.substr(0,2) == "+:" ){
		checkersToUse[current_word.substr(2)] = true;
	  }else if(current_word.substr(0,2) == "-:" ){
		checkersToUse[current_word.substr(2)] = false;
	  }
	}
  }

  return checkersToUse;
}



void
buildCheckers( std::vector<Compass::TraversalBase*> &retVal, Compass::Parameters &params, Compass::OutputObject &output, SgProject* pr )
{
' >> $source

for dir in ${USUBDIRS[@]}
do
  echo -e "
    try {
        CompassAnalyses::${dir}::Traversal *traversal;
        traversal = new CompassAnalyses::${dir}::Traversal(params, &output);
        retVal.push_back(traversal);
    } catch (const std::exception &e) {
        std::cerr << \"error initializing checker ${dir}: \" << e.what() << std::endl;
    }" >> $source
done

echo -e '

  //AS(1/18/2008) Remove the rules that has been deselected from the retVal
  std::string ruleFile = Compass::parseString(params["Compass.RuleSelection"]);

  std::map<std::string, bool > ruleSelection = readFile(ruleFile);

  std::vector<Compass::TraversalBase*> ruleDeselected;
  for( std::vector<Compass::TraversalBase*>::reverse_iterator iItr = retVal.rbegin();
	  iItr != retVal.rend(); iItr++ )
  {
	std::map<std::string, bool >::iterator isRuleSelected = ruleSelection.find((*iItr)->getName() );   
      
	if( isRuleSelected == ruleSelection.end() ){
         std::cerr << "Error: It has not been selected if " + (*iItr)->getName() + " should be run." 
		           << std::endl;
		 exit(1);
	}
	if( isRuleSelected->second == false  )
	  ruleDeselected.push_back(*iItr);
  }

   for( std::vector<Compass::TraversalBase*>::iterator iItr = ruleDeselected.begin();
	  iItr != ruleDeselected.end(); iItr++ )
  {
	retVal.erase(std::find(retVal.begin(),retVal.end(),*iItr));
  }



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
generateCompassHeader ${COMPASS_HEADER}
generateCompassSource ${COMPASS_SOURCE}
# generateCompassDocs ${COMPASS_DOCS}
