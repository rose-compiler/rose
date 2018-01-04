#!/bin/bash
# a script to make sure the test codes do not have unintentional errors
# To run: ./testAll.sh &>log.txt  
# then check log.txt

#CTESTS=`ls *.c`
#for test in $CTESTS; do
#    clang -c -fopenmp -Wall $test 
#done

CPPTESTS=`ls *.cpp`
for test in $CPPTESTS; do
    clang++ -c -fopenmp -Wall $test 
done


rm *.o
