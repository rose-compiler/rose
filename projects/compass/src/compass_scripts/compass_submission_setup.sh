#!/bin/bash
################################################################################
#
# Author: Gary Yuan
# File: compass_submission_setup.sh
# Date: 7/19/2007
# Updated: 1/24/2008
# Purpose: Automatically build compass from checker submit directory 
#
################################################################################

### GLOBAL VARIABLES

BACK=`pwd`
SCRIPTDIR=`dirname $0`
CMDROOT=`sh -c "cd $SCRIPTDIR; pwd"`
CHECKIN_DIRECTORY=$1
COMPASS_PROJECT=$2
READ_ONLY=$3
CHECKERS_DIRECTORY=${CHECKIN_DIRECTORY}/../

NEW_CHECKERS=`find ${CHECKIN_DIRECTORY} -maxdepth 1 -type f`

COMPASS_LIST=${COMPASS_PROJECT}/CHECKER_LIST

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

updateRuleSelection()
{
  local checker=$1
  local rule_selection=$2

  local checker_exist=`grep "${checker}" $rule_selection`

  if [[ $checker_exist == '' ]]; then
    echo "Updating ${checker} to ${rule_selection}"
    echo "+:${checker}" >> ${rule_selection}
  fi
}

###############################################################################
extraDistMakefile()
{
  dir=$1

#  echo ${CHECKERS_DIRECTORY}/${dir}/${dir}.makefile

  cat > ${CHECKERS_DIRECTORY}/${dir}/${dir}.makefile <<END
# Set Variables ROSE_INSTALL to your ROSE_INSTALL installation and
# COMPASS_SUPPORT to your compassSupport directory like this:
#ROSE_INSTALL= ROSE-INSTALL-DIRECTORY
#COMPASS_PROJECT = COMPASS-PROJECT-DIRECTORY 
#COMPASS_SUPPORT = \${COMPASS_PROJECT}/compassSupport

CHECKER_NAME = ${dir}Test

LINKER_FLAGS = -L\$(ROSE_INSTALL)/lib -Wl,-rpath \$(ROSE_INSTALL)/lib -lrose \$(RT_LIBS) -L\`pwd\` -Wl,-rpath \`pwd\` -lcompass

all: ${dir}Test

libcompass.so: \$(COMPASS_SUPPORT)/compass.h \$(COMPASS_SUPPORT)/compass.C
	g++ -fPIC -Wall -shared -o \$@ \$(COMPASS_SUPPORT)/compass.C -I\$(ROSE_INSTALL)/include -I\$(COMPASS_SUPPORT)

${dir}Test: ${dir}.C ${dir}Main.C libcompass.so \$(COMPASS_SUPPORT)/compassTestMain.C
	g++ -fPIC -Wall -o \$@ ${dir}.C ${dir}Main.C -I\$(COMPASS_SUPPORT) -I\$(ROSE_INSTALL)/include \$(LINKER_FLAGS)

test: ${dir}Test ${dir}Test1.C
	./${dir}Test ${dir}Test1.C

clean:
	rm -f libcompass.so "${dir}Test"

END

}

extraDist()
{
  for dir in ${SUBDIRS[@]}
  do
    Dir=${CHECKERS_DIRECTORY}/${dir}
    rm -f ${Dir}/Makefile.am ${Dir}/compass.[Ch] ${Dir}/Makefile ${Dir}/compassTestMain.C
    extraDistMakefile ${dir}
  done
} # extraDist

################################################################################
################################################################################
# stuff needs to be done

unset GREP_OPTIONS

if(( $# < 2 )); then
  echo "Usage: `basename $0` <COMPASS SUBMIT DIR> <COMPASS PROJECT DIR> <regenerate>
	Summary of options:
	<COMPASS SUBMIT DIR> -- Directory where checker tarballs are located
	<COMPASS PROJECT DIR> -- compass directory under ROSE source tree
	<regenerate> -- optional, specify to regenerate current checkers from file CHECKER_LIST"
  exit 1
fi

if [[ ! -d $CHECKIN_DIRECTORY ]]; then
  echo "Error: $CHECKIN_DIRECTORY is not a valid directory"
  exit 1
fi

if [[ ! -d $COMPASS_PROJECT ]]; then
  echo "Error: $COMPASS_PROJECT is not a valid directory"
  exit 1
fi

if [[ ! -f ${COMPASS_LIST} ]]; then
  touch ${COMPASS_LIST}
elif [[ ${READ_ONLY} == "new" ]]; then
  rm -f ${COMPASS_LIST}
fi # if [[ ! -f ${COMPASS_LIST} ]]

if [[ ${READ_ONLY} == "read" ]]; then
  echo 'Please note, the "read" option is now "regenerate"'
  exit 1
fi

################################################################################

#if [[ ${READ_ONLY} != "regenerate" ]]; then
#  for checker in $NEW_CHECKERS
#  do
#      checker_directory=`tar ztf ${checker} | head -1`
#      tar -C ${CHECKERS_DIRECTORY} -zkxf ${checker} >& /dev/null
#  
#      if [[ -d ${COMPASS_PROJECT}/${checker_directory} ]]; then
#          echo "Updating ${COMPASS_PROJECT}/${checker_directory}..."
#          rm -rf ${COMPASS_PROJECT}/${checker_directory}
#      fi
  
#      echo "Moving ${checker_directory} to ${COMPASS_PROJECT}..."
#      mv -f /tmp/${checker_directory} ${COMPASS_PROJECT}
#  
#      updateCheckerList `basename ${checker_directory:0:((${#checker_directory}-1))}` ${COMPASS_LIST}
#  done
#fi # if [[ ${READ_ONLY} != "regenerate" ]]

echo "Sorting ${COMPASS_LIST}..."
sort ${COMPASS_LIST} -o ${COMPASS_LIST}

# for checker in $NEW_CHECKERS do; ...; done
# Updates the compass project directory specified by ${COMPASS_PROJECT} with
# the submitted checkers in ${CHECKIN_DIRECTORY}.
# The list of all checkers ${COMPASS_LIST} is also updated.
  
for checker in `cat ${COMPASS_LIST}`
do
  if [[ `echo $checker | grep -v "^#"` != '' ]]; then
    USUBDIRS[$((SUBDIR_COUNT))]=`echo ${checker:0:1} | gawk '{print toupper($0)}'`${checker:1}
    SUBDIRS[$((SUBDIR_COUNT++))]=$checker
  fi #if [[ `echo $checker | grep -v "^#"` != '' ]], # is comment symbol
done

for checker in ${USUBDIRS[@]}
do
  updateRuleSelection $checker ${COMPASS_PROJECT}/RULE_SELECTION 
done

echo "Sorting ${COMPASS_PROJECT}/RULE_SELECTION"
sort ${COMPASS_PROJECT}/RULE_SELECTION -o ${COMPASS_PROJECT}/RULE_SELECTION 

extraDist

################################################################################
