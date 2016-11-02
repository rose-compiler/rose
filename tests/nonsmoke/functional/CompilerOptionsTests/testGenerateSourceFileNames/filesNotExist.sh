#!/bin/bash -norc
for FILE in "$@"
do
  if [ -f $FILE ]; then
      echo "ERROR: expected output $FILE from ROSE (does not exist or is no regular file)"  ;
      exit 1;
  fi
done

