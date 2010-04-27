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
COMPASS_PROJECT=$2
READ_ONLY=$3
#CHECKERS_DIRECTORY=${CHECKIN_DIRECTORY}/../
CHECKERS_DIRECTORY=$1

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

  test -f ${CHECKERS_DIRECTORY}/${dir}/${dir}.compass.external.makefile || cat > ${CHECKERS_DIRECTORY}/${dir}/${dir}.compass.external.makefile <<END
# Set Variables ROSE_INSTALL to your ROSE_INSTALL installation and
# COMPASS_SUPPORT to your compassSupport directory like this:
#ROSE_INSTALL=ROSE-INSTALL-DIRECTORY
#COMPASS_PROJECT=COMPASS-PROJECT-DIRECTORY
#COMPASS_SUPPORT=\${COMPASS_PROJECT}/src/compassSupport

CHECKER_NAME = ${dir}Test

LINKER_FLAGS = -L\$(ROSE_INSTALL)/lib -Wl,-rpath \$(ROSE_INSTALL)/lib -lrose \$(RT_LIBS) -L\`pwd\` -Wl,-rpath \`pwd\` -lcompass

all: prerequisites.h instantiate_prerequisites.h ${dir}Test

prerequisites.h: \$(COMPASS_PROJECT)/extensions/prerequisites
	find \$(COMPASS_PROJECT)/extensions/prerequisites -name "*.h" | awk -F/ '{print "#include \"" \$\$NF "\""}' > \$@

instantiate_prerequisites.h: \$(COMPASS_PROJECT)/extensions/prerequisites
	cat \$(COMPASS_PROJECT)/extensions/prerequisites/*.h | grep "^extern" | sed -e 's@extern[\\t\\ ]*@Compass::@g' | awk '{print \$\$1 " Compass::" \$\$2}' > \$@


libcompass.so: \$(COMPASS_PROJECT)/src/compassSupport/compass.h \$(COMPASS_PROJECT)/src/compassSupport/compass.C prerequisites.h instantiate_prerequisites.h
	g++ -fPIC -Wall -shared -o \$@ \$(COMPASS_PROJECT)/src/compassSupport/compass.C -I\$(ROSE_INSTALL)/include -I\$(COMPASS_PROJECT)/src/compassSupport -I\$(COMPASS_PROJECT)/extensions/prerequisites -I.

# GMY: The order of source files during compilation is important due to the use
# of extern. The checker source code must follow the main source or else the
# executable is likely to seg. fault.
${dir}Test: ${dir}.C ${dir}Main.C libcompass.so \$(COMPASS_PROJECT)/src/compassSupport/compassTestMain.C
	g++ -fPIC -Wall -o \$@ ${dir}Main.C ${dir}.C -I\$(COMPASS_PROJECT)/src/compassSupport -I\$(ROSE_INSTALL)/include -I\$(COMPASS_PROJECT)/extensions/prerequisites -I. \$(LINKER_FLAGS)

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
    rm -f ${Dir}/Makefile.am ${Dir}/compass.[Ch] ${Dir}/Makefile ${Dir}/compassTestMain.C ${Dir}/prerequisites.h ${Dir}/instantiate_prerequisites.h
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

if [[ ! -d $CHECKERS_DIRECTORY ]]; then
  echo "Error: $CHECKERS_DIRECTORY is not a valid directory"
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

echo "Sorting ${COMPASS_LIST}..."
env LC_ALL=C sort ${COMPASS_LIST} -o ${COMPASS_LIST}

for checker in `cat ${COMPASS_LIST}`
do
  if [[ `echo $checker | grep -v "^#"` != '' ]]; then
    USUBDIRS[$((SUBDIR_COUNT))]=`echo ${checker:0:1} | awk '{print toupper($0)}'`${checker:1}
    SUBDIRS[$((SUBDIR_COUNT++))]=$checker
  fi #if [[ `echo $checker | grep -v "^#"` != '' ]], # is comment symbol
done

for checker in ${USUBDIRS[@]}
do
  updateRuleSelection $checker ${COMPASS_PROJECT}/RULE_SELECTION.in
done

echo "Sorting ${COMPASS_PROJECT}/RULE_SELECTION.in"
# Using GNU sort's in-place sorting capability
env LC_ALL=C sort -o ${COMPASS_PROJECT}/RULE_SELECTION.in ${COMPASS_PROJECT}/RULE_SELECTION.in

extraDist

################################################################################
