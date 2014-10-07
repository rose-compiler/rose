#!/bin/bash

EXECUTABLE=variableTypeTest
BASEDIR=$(dirname "$0")
for file in $BASEDIR/inputFiles/types_*.cpp
do
#  echo "$1"/"$EXECUTABLE" $file
  EXPECTED=$( head -n 1 "$file" | sed 's#//##')
#  echo "Expected value: " $EXPECTED
  "$1"/"$EXECUTABLE" "$file"
  RETVAL=$?
#  echo "Return value: " $RETVAL
  if [ "$RETVAL" -ne "$EXPECTED" ] ; then
    echo "Error occured in " $file
    exit -1
  fi
done
