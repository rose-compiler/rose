#!/bin/bash -e

DIRS="C_tests C89_std_c89_tests C99_tests C11_tests Cxx_tests Cxx11_tests Cxx14_tests Cxx17_tests C_subset_of_Cxx_tests MicrosoftWindows_C_tests MicrosoftWindows_Cxx_tests"

[ -z $nprocs ] && nprocs=1

target=$1
[ -z $target ] && target="translator"

[ $target == "compiler" ]    && MAKE_OPTIONS="TEST_TRANSLATOR=testCompiler ROSE_FLAGS="
[ $target == "backend" ]     && MAKE_OPTIONS="TEST_TRANSLATOR=../../testBackend"
[ $target == "translator" ]  && MAKE_OPTIONS=""

fname=status-$target

rm -f $fname.csv $fname.log $fname.tgz

files=
anp=0
anf=0

for d in $DIRS; do

  pushd $d > /dev/null
  make --jobs $nprocs clean &> /dev/null
  ( make $MAKE_OPTIONS --jobs $nprocs --keep-going status &> $fname.log ) || true
  popd > /dev/null

  passed=$(echo $d/*.passed | tr ' ' '\n' | grep -v \* || true)
  if [ ! -z "$passed" ]; then
    files="$files  $(echo $passed | paste -sd\ )"
    for f in $(echo "$passed" | cut -d/ -f2- | rev | cut -d. -f2- | rev); do
      echo $d,$f,0 >> $fname.csv
    done
  fi
  np=$(echo $passed | tr ' ' '\n' | wc -l)
  anp=$((anp+np))

  failed=$(echo $d/*.failed | tr ' ' '\n' | grep -v \* || true)
  if [ ! -z "$failed" ]; then
    files="$files $(echo $failed | paste -sd\ )"
    for f in $(echo "$failed" | cut -d/ -f2- | rev | cut -d. -f2- | rev); do
      echo "$d,$f,1" >> $fname.csv
    done
  fi
  nf=$(echo $failed | tr ' ' '\n' | wc -l)
  anf=$((anf+nf))

  echo "[STATUS:$target:summary] $d $np $nf"

done

echo "[STATUS:$target:summary] ALL $anp $anf"

cat $fname.csv | sed 's/^/[STATUS:'$target':details]/'
tar czf $fname.tgz $files &> /dev/null

