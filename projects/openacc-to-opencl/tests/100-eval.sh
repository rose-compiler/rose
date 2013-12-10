#!/bin/bash

# Usage: ./eval.sh src_dir data_dir test_cat test_id test_case
#     * src_dir   : Directory where the source for this test are located
#     * data_dir  : Directory where the resulting data-file will be stored
#     * test_id   : Test's unique ID
#     * test_case : Name of the binary to use. Enable to try multiple versions.

source $1/../test-vars.rc
source $1/../100-test-vars.rc
source $1/$3-test-vars.rc

TEST_SRCDIR=$1 make -f $1/Makefile $3-$4

mkdir -p $2
rm -f $2/$3-$4.dat

# TODO if NUMBER_PARAMS != 1 ...

for size in $SIZE_LIST
do
  PARAMS_LIST=$PARAMS_LIST $(($size*1024*1024))
done

for params in $PARAMS_LIST
do
  for gang in $GANG_LIST
  do
    for worker in $WORKER_LIST
    do
      for vector in $VECTOR_LIST
      do
        echo "size = $size"
        echo "gang = $gang"
        echo "worker = $worker"
        echo "vector = $vector"

        for i in `seq 1 $LIBACC_TEST_SAMPLING_SIZE`
        do
          echo -n "."
          ./$3-$4 $gang $worker $vector $params >> $2/$3-$4.dat
          echo $gang $worker $vector $params >> $2/$3-$4.dat
        done

        echo;echo "----------------------------"
      done
    done
  done
done

