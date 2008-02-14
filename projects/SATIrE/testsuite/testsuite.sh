#!/bin/sh 

# Basic test script
# (C) 2008 Adrian Prantl

#set -x

if [ x$1 = x ]; then 
    SUITE="."
else
    SUITE="$1"
fi

FILES=`find $SUITE -name \*.[cC]* 2>/dev/null`
CURRENTDIR=`pwd`
ANALYSIS="constprop/constprop"
OPTIONS="--wholeprogram --preinfo --postinfo --textoutput --termoutput --sourceoutput --no_anim"

expected_fails=0
fails_ok=0
expected_succs=0
succs_ok=0

fail_errors=""
succ_errors=""

for file in $FILES; do

  for analysis in $ANALYSIS; do

    if ! echo $file | grep -q ^$SUITE/dontrun; then

      echo "--- testing $ANALYSIS on file $file ---"

      $analysis $OPTIONS $file >/dev/null
      result=$?
      # Expected FAIL
      if echo $file | grep -q ^$SUITE/failure; then
	  expected_fails=$(( $expected_fails + 1 ))
	  if [ $result != 0 ]; then
	      fails_ok=$(( $fails_ok + 1 ))
	  else
	      echo "** ERROR: Expected failure succeeded $analysis $OPTIONS $file"
	      fail_errors="$fail_errors $analysis:$file"
	  fi
      fi
      # Expected SUCCESS
      if echo $file | grep -q ^$SUITE/success; then    
	  expected_succs=$(( $expected_succs + 1 ))
	  if [ $result == 0 ]; then
	      succs_ok=$(( $succs_ok + 1 ))
	  else
	      echo "** ERROR: Expected success failed $analysis $OPTIONS $file"
	      succ_errors="$succ_errors $analysis:$file"
	  fi
      fi
    fi
  done 
done

echo "########################################################################"
echo "# SATIrE automagic test report, `date`"
echo "########################################################################"
echo

echo "[$succs_ok/$expected_succs] expected successes"
echo "[$fails_ok/$expected_fails] expected failures"

if [ "x$fail_errors" != "x" ]; then
    echo "** ERROR: Expected failures succeeded"
    for i in $fail_errors; do 
	echo "  $i"
    done
fi
if [ "x$succ_errors" != "x" ]; then
    echo "** ERROR: Expected successes failed"
    for i in $succ_errors; do
	echo "  $i"
    done
fi
if [ "x$fail_errors$succ_errors" != "x" ]; then
    exit 1
fi
echo
echo "########################################################################"

# vim: ts=4 sts=4 sw=4:
