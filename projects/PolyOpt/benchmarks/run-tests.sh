#!/bin/bash

CXX=gcc
PLUTOROSE=`pwd`/../plutorose
DIFF=diff

TOTAL_TESTS=0
TOTAL_SUCCESS=0
TOTAL_FAIL=0

trap "" SEGV

function logEcho
{
  echo $1
  echo $1 >> $DETAIL_LOG
}

function cleanup
{
  rm $1/$2.original 2> /dev/null
  rm $1/$2.plutorose 2> /dev/null
  rm $1/$2.test-diff 2> /dev/null
  rm $1/$2.original.output 2> /dev/null
  rm $1/$2.plutorose.output 2> /dev/null
}

function runBenchmark
{
  local RESTORE_DIR=`pwd`
  cd $1

  cleanup $1 $2

  local COMPILE_OPTIONS=""
  local RUN_OPTIONS=""
  local ALL_FILES=""

  if [ ! -e "test-compile-options" ]
  then
    logEcho "WARNING:  No test-compile-options file found!"
  else
    local COMPILE_OPTIONS="`cat test-compile-options`"
  fi

  if [ ! -e "test-run-options" ]
  then
    logEcho "WARNING:  No test-run-options file found!"
  else
    local RUN_OPTIONS="`cat test-run-options`"
  fi

  if [ ! -e "test-source-files" ]
  then
    logEcho "ERROR:  No test-source-files descriptor found!"
    logEcho "RESULT:  Fail"
    let TOTAL_FAIL++
    cleanup $1 $2
    cd $RESTORE_DIR
    return
  else
    local ALL_FILES="`cat test-source-files`"
  fi


  $CXX $COMPILE_OPTIONS $ALL_FILES -o $2.original >> $DETAIL_LOG 2>&1
  if [ $? -ne 0 ]
  then
    logEcho "ERROR:  Build failed for original source."
    logEcho "RESULT:  Fail"
    let TOTAL_FAIL++
    cleanup $1 $2
    cd $RESTORE_DIR
    return
  fi

  if [ -e "test-input-file" ]
  then
    ./$2.original $RUN_OPTIONS < $(cat test-input-file) > $2.original.output 2>&1
  else
    ./$2.original $RUN_OPTIONS > $2.original.output 2>&1
  fi

  # We cannot check return codes here because some benchmarks are lacking "return" statements,
  # but we can check for signals (i.e. SIGSEGV).
  local RET=$?
  if [ $RET -gt 127 ]
  then
    logEcho "ERROR:  Execution of original program failed with signal $((RET - 128))."
    logEcho "RESULT:  Fail"
    cat $2.original.output >> $DETAIL_LOG
    let TOTAL_FAIL++
    cleanup $1 $2
    return
  fi

  if [ -e "test-output-file" ]
  then
    cp $(cat test-output-file) $2.original.output
  fi

  # Build list of Rose-generated sources.
  local ROSE_SOURCES=""
  for file in $ALL_FILES
  do
    ROSE_SOURCES="rose_$file $ROSE_SOURCES"
  done
  

  $PLUTOROSE $COMPILE_OPTIONS $ALL_FILES --edg:no_warnings -rose:skipfinalCompileStep >> $DETAIL_LOG 2>&1
  local RET=$?
  if [ $RET -gt 127 ]
  then
    logEcho "ERROR:  PlutoRose invocation failed with signal $((RET - 128))."
    logEcho "RESULT:  Fail"
    let TOTAL_FAIL++
    cleanup $1 $2
    cd $RESTORE_DIR
    return
  elif [ $RET -ne 0 ]
  then
    logEcho "ERROR:  PlutoRose invocation failed  (Error Code: $?)."
    logEcho "RESULT:  Fail"
    let TOTAL_FAIL++
    cleanup $1 $2
    cd $RESTORE_DIR
    return
  fi

  $CXX $COMPILE_OPTIONS $ROSE_SOURCES -o $2.plutorose >> $DETAIL_LOG 2>&1
  if [ $? -ne 0 -o ! -e $2.plutorose ]
  then
    logEcho "ERROR:  Build failed for PlutoRose-transformed sources."
    logEcho "RESULT:  Fail"
    let TOTAL_FAIL++
    cleanup $1 $2
    cd $RESTORE_DIR
    return
  fi

  if [ -e "test-input-file" ]
  then
    ./$2.plutorose $RUN_OPTIONS < $(cat test-input-file) > $2.plutorose.output 2>&1
  else
    ./$2.plutorose  $RUN_OPTIONS > $2.plutorose.output 2>&1
  fi

  local RET=$?
  if [ $RET -gt 127 ]
  then
    logEcho "ERROR:  Execution of PlutoRose-transformed program failed with signal $((RET - 128))."
    logEcho "RESULT:  Fail"
    cat $2.plutorose.output >> $DETAIL_LOG
    let TOTAL_FAIL++
    cleanup $1 $2
    cd $RESTORE_DIR
    return
  fi

  if [ -e "test-output-file" ]
  then
    cp $(cat test-output-file) $2.plutorose.output
  fi
  
  $DIFF -q $2.original.output $2.plutorose.output
  if [ $? -ne 0 ]
  then
    logEcho "ERROR:  Diff failed between original and transformed runs!"
    logEcho "RESULT:  Fail"
    let TOTAL_FAIL++
    cleanup $1 $2
    cd $RESTORE_DIR
    return
  fi

  logEcho "RESULT:  Success"
  logEcho "Results Size: `du -h $2.plutorose.output | awk '{ print $1 }'`"

  let TOTAL_SUCCESS++

  cleanup $1 $2

  cd $RESTORE_DIR
}


function runBenchmarkSuite
{
  local RESTORE_DIR=`pwd`
  cd $1

  local BENCHMARKS=`ls -d */ | awk -F[/] '{print $1}'`
  local BENCHMARKS=${BENCHMARKS//CVS/}

  
  for benchmark in $BENCHMARKS
  do
    if [[ "x$BENCHMARK_INCLUDE" == "x" || $BENCHMARK_INCLUDE == *$benchmark* ]]
    then
      logEcho "-------- Starting $benchmark --------"
      runBenchmark `pwd`/$benchmark $benchmark
      logEcho "-------- Finishing $benchmark --------"
      let TOTAL_TESTS++
    fi
  done

  cd $RESTORE_DIR
}


BENCHMARK_SUITES=`ls -d */ | awk -F[/] '{print $1}'`
BENCHMARK_SUITES=${BENCHMARK_SUITES//CVS/}

RESULTS_FILE="`pwd`/test-results.txt"
DETAIL_LOG="`pwd`/detail.log"

BENCHMARK_SUITE_INCLUDE=""
BENCHMARK_INCLUDE=""

echo "" > $DETAIL_LOG

while getopts "s:b:h" flag
do

  case "$flag" in

    "s")
      BENCHMARK_SUITE_INCLUDE="$OPTARG $BENCHMARK_SUITE_INCLUDE"
      ;;
    "b")
      BENCHMARK_INCLUDE="$OPTARG $BENCHMARK_INCLUDE"
      ;;
    "h")
      echo "./run-tests [-s suite] [-b benchmark]"
      echo ""
      echo "  -s suite        Only process the named benchmark suites."
      echo "  -b benchmark    Only process the named benchmarks."
      exit 1
      ;;
    "?")
      exit 1
      ;;
  esac

#  if [ x"$flag" == "x?" ]
#  then
#    exit 1
#  elif [ x"$flag" == "xs" ]
#  then
#    BENCHMARK_SUITES_INCLUDE="$OPTARG $BENCHMARK_SUITES_INCLUDE"
#  fi
done

for suite in $BENCHMARK_SUITES
do
  if [[ "x$BENCHMARK_SUITE_INCLUDE" == "x" || $BENCHMARK_SUITE_INCLUDE == *$suite* ]]
  then
    logEcho "======== Starting $suite ========"
    runBenchmarkSuite `pwd`/$suite $suite
    logEcho "======== Finishing $suite ========"
  fi
done


echo ""
echo "Results of $TOTAL_TESTS Test(s):"
echo "Success:  $TOTAL_SUCCESS"
echo "Fail:     $TOTAL_FAIL"

