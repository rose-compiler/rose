#!/bin/bash -e

> status.csv

files=
anp=0
anf=0

for d in $@; do

  passed=$(echo $d/*.passed | tr ' ' '\n' | grep -v \* || true)
  if [ ! -z "$passed" ]; then
    files="$files  $(echo $passed | paste -sd\ )"
    for f in $(echo "$passed" | cut -d/ -f2- | rev | cut -d. -f2- | rev); do
      echo $d,$f,0 >> status.csv
    done
  fi
  np=$(echo $passed | tr ' ' '\n' | wc -l)
  anp=$((anp+np))

  failed=$(echo $d/*.failed | tr ' ' '\n' | grep -v \* || true)
  if [ ! -z "$failed" ]; then
    files="$files $(echo $failed | paste -sd\ )"
    for f in $(echo "$failed" | cut -d/ -f2- | rev | cut -d. -f2- | rev); do
      echo "$d,$f,1" >> status.csv
    done
  fi
  nf=$(echo $failed | tr ' ' '\n' | wc -l)
  anf=$((anf+nf))

  echo "[STATUS:pass/fail] CompileTests/$d $np $nf"

done

echo "[STATUS:pass/fail] CompileTests $anp $anf"

#tar czf status.tgz $files

