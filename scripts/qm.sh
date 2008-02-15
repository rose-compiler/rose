#!/bin/bash
#
# Author: Gary Yuan
# Date: 6/25/2007
# File: qmsh
# Purpose: wrapper for the shell to automatically generate QMTEST test files
#
################################################################################

source ~/.bashrc
# Globals

# Variables YOU must set
  MYCC=gcc
  MYCXX=g++
  MYLD=g++

  TESTCC=rose_regression_cc
  TESTCXX=rose_regression_cxx
  TESTLD=rose_regression_cxx
# Variables YOU must set stop here

ROOT=$ROSE_TEST_REGRESSION_ROOT
UPLEVEL=2
REGRESSION_ROOT=`echo ${ROOT} | sed -e 's/\//./g'`

PATH=`echo $PATH | sed -e "s/$REGRESSION_ROOT\/scripts://g"`

TESTNAME="badtest.qmt"
OBJECT=""
OUTPUT=""
TESTCLASS="command.ExecTest"
PROGRAM=""

declare -a SOURCE=""
declare -a TEST_ARGUMENTS

setUplevel()
{
  local uplevel=""

  while((UPLEVEL--))
  do
    uplevel="${uplevel}\.\.\/"
  done

  UPLEVEL=${uplevel}
} #setUplevel()

setSourceArray()
{
  local count=0

  for arg in $@
  do
    case $arg in
      *.csv) ;;
      *.[cC]* | *.[fF])
        TESTCLASS="rose.RoseTest" 
        SOURCE[$((count++))]=$arg 
      ;; 
    esac
  done

  local obj=`basename ${SOURCE[0]}`
  OBJECT=${obj%%.*}.o
  OUTPUT=`basename $OBJECT`
} # setSourceName()

setObjectName()
{
  declare -i objectFlag=0

  for arg in $@
  do
    if((objectFlag > 0)); then
      OUTPUT=$arg
      OBJECT=$arg
      break
    elif [[ $arg == "-o" ]]; then
      objectFlag=1
    fi
  done

  if [[ $OBJECT == "" ]]; then
    OBJECT=$1
  fi
} # findObjectName()

setQMTestName()
{
  # set the local directory name w/o the regression root 
  local directory=`echo ${PWD} | sed -e "s/$REGRESSION_ROOT\///g"` 

  # replace instances of '.' with '_' in object name
  local object=`echo ${OBJECT} | sed -e "s/\./_/g"`

  # name test after object file
  TESTNAME=${object}

  # prepend the directory to the test name
  TESTNAME=${directory}_${TESTNAME}

  # replace all instances of '/' with '_' in test name
  TESTNAME=`echo ${TESTNAME} | sed -e 's/\//_/g'`

  # replace all instances of '.' with '_dot_' in test name
  TESTNAME=`echo ${TESTNAME} | sed -e 's/\./_dot_/g'`

  # replace all instances of '+' with 'plus' in test name
  TESTNAME=`echo ${TESTNAME} | sed -e 's/+/plus/g'`

  # prepend the unique time of test creation
  TESTNAME=`date +%H%M%S%N`_${TESTNAME}

  # shift the test name to all lower case letters required for QMTEST!
  TESTNAME=`echo ${TESTNAME} | gawk '{print tolower($0)}'`

  # append .qmt extension to test name
  TESTNAME="${TESTNAME}.qmt"
} #setQMTestName()

setPath()
{
  local option=$1	# the flag option
  local directory=""	# the path of the directory
  local back=`pwd`	# where am I now?
  local path="$arg"	# the corrected option + directory path

  # check that the option exists
  if [[ $option != '' ]]; then
    directory=`echo $arg | sed -e "s/$option//g"`
  else
    directory=$arg
  fi

  # special case, argument of option is `pwd`
  if [[ $directory == '`pwd`' ]]; then
    directory=${PWD}
  fi

  # check for absolute path
  if [[ ${directory:0:1} == '/' ]]; then
    path=$option`echo ${directory}/ | sed -e "s/$REGRESSION_ROOT/$UPLEVEL/g"`
  # elif, relative path append pwd
  elif [[ -d $directory ]]; then
    cd $directory
    path=$option`echo ${PWD}/ | sed -e "s/$REGRESSION_ROOT/$UPLEVEL/g"`
  # else if is file 
  elif [[ -a $directory ]]; then
    cd `dirname $directory`
    path=$option`echo ${PWD}/ | sed -e "s/$REGRESSION_ROOT/$UPLEVEL/g"`/`basename $directory`
  fi

  cd $back
  arg=$path
} # setPath()

setPath2()
{
  local back=`pwd`
  cd `dirname $arg`
  arg=$option`echo ${PWD}/ | sed -e "s/$REGRESSION_ROOT/$UPLEVEL/g"`/`basename $arg`
  cd $back
} #setPath2()

setArgumentsPath()
{
  local count=${#TEST_ARGUMENTS[@]}
  local outputflag=0

  for arg in $ARGUMENTS
  do
    if(( outputflag == 1 )); then
      outputflag=0
      TEST_ARGUMENTS[((count++))]=$arg
      continue
    fi

    case $arg in
      -o) outputflag=1 ;;
      *.o) ;;
      '`pwd`') setPath '' ;;
      .*/* | ..*/*  ) setPath2 ;;
      -I*) setPath '-I' ;;
      -L*) setPath '-L' ;;
      -WL*) setPath '-WL,' ;;
      -Wl*) setPath '-Wl,' ;;
      *.[cC] | *.lo | *.so | *.so.* | *.a) setPath2 ;;
      $MYCC) arg=$TESTCC ;;
      $MYCXX) arg=$TESTCXX ;;
      $MYLD) arg=$TESTLD ;;
    esac

    arg=`echo $arg | sed -e "s/$REGRESSION_ROOT/$UPLEVEL/g"`
    TEST_ARGUMENTS[((count++))]=$arg
  done
} # set full path

generateQMTest()
{
  local qmtestarguments="["
  local command=$1
  local arg=""
  declare -a arguments
  declare -i count=0

  for arg in $@
  do
    if((count>0)); then
      arguments[$count]=$arg
    fi

    ((count++))
  done

  # for arguments 0 to 1 before the last one
  # the call to sed changes any native instances of ' character to \'
  for arg in ${arguments[@]:0:$((${#arguments[@]}-1))}
  do
    arg=`echo $arg | sed -e "s/'/\\\\\'/g"`
    qmtestarguments="${qmtestarguments}'$arg', "
  done
  # append the last argument 
  arg=`echo ${arguments[ ${#arguments[@]} ]} | sed -e "s/'/\\\\\'/g"`
  qmtestarguments="${qmtestarguments}'$arg']"

  echo ""
  echo "qmtest argument line: ${qmtestarguments[@]}"
  qmtest create -o "$TESTNAME" -a program=$PROGRAM -a arguments="${qmtestarguments}" test $TESTCLASS

  if(($? != 0)); then
    exit $?
  fi

} #generateQMTest

################################################################################

#echo "$@" >> /home/yuan5/testing/temp.txt
#echo "shell: $SHELL" >> /home/yuan5/testing/temp.txt

setUplevel

declare -i ARGUMENT_COUNT=0
declare -a COMMANDLINE

# preprocessing that needs to be done for all arguments
for arg in $@
do
  case $arg in
    "$SHELL") arg=/bin/sh ;;
    *=*) arg=`echo $arg | sed -e 's/"/\\\"/g'` ;;
  esac

  COMMANDLINE[$((ARGUMENT_COUNT++))]=$arg
done

# determine if command line is run as shell command
if [[ ${COMMANDLINE[0]} != '-c' ]]; then
  COMMAND=${COMMANDLINE[0]}
  ARGUMENTS=${COMMANDLINE[@]:1:$ARGUMENT_COUNT}
else
  COMMAND=${COMMANDLINE[1]}
  ARGUMENTS=${COMMANDLINE[@]:2:$ARGUMENT_COUNT}
fi

# preprocessing done on command
case $COMMAND in

  # this case checks for relative path commands and makes the relative
  # to the regression root directory
  ./* | ../* )
    previousdir=`pwd`
    cd `dirname $arg`
    arg="${PWD}/`basename $arg`"
    arg=`echo $arg | sed -e "s/$REGRESSION_ROOT/$UPLEVEL/g"`
    cd $previousdir
  ;;

  # this case checks for compile or link commands and includes the option
  # link/include the current directory
  $MYCC | $MYCXX | $MYLD )
    TESTCLASS="rose.RoseTest"
    setSourceArray ${ARGUMENTS[@]}
    setObjectName ${ARGUMENTS[@]}
    TEST_ARGUMENTS[0]="-I."	# to include testing directory
    TEST_ARGUMENTS[1]="-L."	# to link testing directory
  ;;
esac

case $COMMAND in
  $MYCC)
    PROGRAM=$TESTCC
    ;; #case MYCC
  $MYCXX)
    PROGRAM=$TESTCXX
    ;; #case MYCXX
  $MYLD)
    PROGRAM=$TESTLD
    ;; #case MYLD

  *make*) PROGRAM='' ;;

  *) PROGRAM=$COMMAND 
    setSourceArray ${ARGUMENTS[@]}
    setObjectName ${ARGUMENTS[@]}
    OUTPUT=""
  ;;
esac

if [[ $PROGRAM != '' ]]; then
  setArgumentsPath
  setQMTestName $OBJECT
  generateQMTest $PROGRAM ${TEST_ARGUMENTS[@]}
fi

# Execute Backend
/bin/bash -c "$COMMAND ${ARGUMENTS[*]}"
exit $?
