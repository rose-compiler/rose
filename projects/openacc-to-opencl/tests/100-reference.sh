#!/bin/bash

source $1/test-vars.rc

mkdir -p $1/datas
rm -f $1/datas/$2.log

for size in $LIBACC_TEST_SIZE_LIST
do

  echo "size = $size"

  for i in `seq 1 $LIBACC_TEST_SAMPLING_SIZE`
  do
    echo -n "."
    ./$2 $(($size*1024*1024)) >> $1/datas/$2.dat
  done

  echo;echo "----------------------------"
done

