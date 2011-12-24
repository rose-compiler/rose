#!/bin/bash

if [ $# -ne 3 ]; then
    echo
    echo "Usage: $0 compass2_executable testcode matchstring"
    echo
    exit 1
else
    EXE="$1"
    TESTCODE="$2"
    MATCHSTRING="$3"
fi

# ------------------------------------------------------------------------------
#  Test !
# ------------------------------------------------------------------------------

# expectation
if ! expected="$(head -n 1 "$TESTCODE" | awk '{print $2}')"; then
  echo
  echo "Error: could not retrieve the Expected matches from"
  echo
  echo "    $testcode"
  echo
  echo "error:output=$expected"
  echo
  exit 1
fi

# run test
cmd="$EXE $TESTCODE"
log="$($cmd 2>&1)"
if [ "$?" -ne 0 ]; then
    echo
    echo "!! Error !!"
    echo
    echo "\$ $cmd"
    echo
    echo "$log"
    echo
    exit 1
else
    if ! matches="$(echo "$log" | grep  "$MATCHSTRING" | wc -l)"; then
        echo
        echo "Error: could not compute the Actual matches from"
        echo
        echo "    $TESTCODE"
        echo
        echo "error:output=$matches"
        echo
        exit 1
    else
        if [ "$matches" -ne "$expected" ]; then
          echo
          echo "-------------------------------------------------------------------------"
          echo "- !! ERROR !!"
          echo "-------------------------------------------------------------------------"
          echo
          echo "-- $(basename "$TESTCODE")"
          echo "-- ($expected) matches expected"
          echo "-- ($matches) matches detected"
          echo
          echo "-------------------------------------------------------------------------"
          echo "- \$ $cmd"
          echo "-------------------------------------------------------------------------"
          echo
          echo "$log"
          echo
          echo "-------------------------------------------------------------------------"
          echo "- $TESTCODE:"
          echo "-------------------------------------------------------------------------"
          cat -n "$TESTCODE"
          echo "-------------------------------------------------------------------------"
          exit 1
        fi
    fi
fi

