#!/bin/bash

FILES=`find .`

for FILE in $FILES
do

  QMTEST=`basename $FILE`

  if [[ ${QMTEST#*.} != 'qmt' && $FILE != './QMTest'* ]]; then
    rm -r -f $FILE >& /dev/null
  fi

done
