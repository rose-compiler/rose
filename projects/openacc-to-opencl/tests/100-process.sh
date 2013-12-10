#!/bin/bash

source $1/../test-vars.rc
source $1/../100-test-vars.rc
source $1/$3-test-vars.rc

for file in `ls $2/$3*.dat | sed s/\.dat$//`
do
  cat $file.dat  | awk -v reps=$LIBACC_TEST_SAMPLING_SIZE '\
                             BEGIN {\
                               i=0;\
                               s1=0;\
                               s2=0;\
                             }\
                             {\
                               s1=s1+$1;\
                               s2=s2+$2;\
                               i++;\
                               if (i==reps) {\
                                 print s1 "," s2 "," $3 "," $4 "," $5 "," $6 "," $7 "," $8;\
                                 s1=0
                                 s2=0
                                 i=0;\
                               }\
                             }' > $file.tmp

  
  echo "comp-speedup,comp-comm-speedup,comp-time,comm-time,gang,worker,vector,size" > $file.csv
  for size in $LIBACC_TEST_SIZE_LIST
  do
    cat $file.tmp | grep "^$size" | awk -v tref=`cat $1/$3.csv | grep "$size$" | awk 'BEGIN{FS=","}{print $1}'` '\
                                                    BEGIN{FS=","}\
                                                    {\
                                                      print tref/$1 "," tref/$2 "," $1 "," $2-$1 "," $3 "," $4 "," $5 "," $6 "," $7 "," $8;\
                                                    }' >> $file.csv
  done

  for gang in $LIBACC_TEST_GANG_LIST
  do
    for worker in $LIBACC_TEST_WORKER_LIST
    do
      for vector in $LIBACC_TEST_VECTOR_LIST
      do
        echo "comp-speedup,comp-comm-speedup,comp-time,comm-time,gang,worker,vector,size" > $file-by_size-gang_$gang-worker_$worker-vector_$vector.csv
        cat $file.csv | grep "$gang,$worker,$vector" >> $file-by_size-gang_$gang-worker_$worker-vector_$vector.csv
      done
    done
  done
done

# TODO By   size: one data point per gang/worker/vector tested + mean + stddev

# TODO By   gang/worker: 


#for size in $LIBACC_TEST_SIZE_LIST
#do
#  for gang in $LIBACC_TEST_GANG_LIST
#  do
#    for worker in $LIBACC_TEST_WORKER_LIST
#    do
#      for vector in $LIBACC_TEST_VECTOR_LIST
#      do
#      done
#    done
#  done
#done

