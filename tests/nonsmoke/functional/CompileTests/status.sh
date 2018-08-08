#!/bin/bash -e

> status.csv

files=

for d in $@; do

  passed=$(echo $d/*.passed | tr ' ' '\n' | grep -v \* || true)
  if [ ! -z "$passed" ]; then
    files="$files  $(echo $passed | paste -sd\ )"
    for f in $(echo "$passed" | cut -d/ -f2- | rev | cut -d. -f2- | rev); do
      echo $d,$f,0 >> status.csv
    done
  fi

  failed=$(echo $d/*.failed | tr ' ' '\n' | grep -v \* || true)
  if [ ! -z "$failed" ]; then
    files="$files $(echo $failed | paste -sd\ )"
    for f in $(echo "$failed" | cut -d/ -f2- | rev | cut -d. -f2- | rev); do
      msg=$(tail -n2 $d/$f.failed | head -n1)
      echo "$d,$f,1,\"$msg\"" >> status.csv
    done
  fi

done

tar czf status.tgz $files
