#!/bin/bash
# RERS 2012 Solutions conversion, MS 2013.
# $1 is name of rers 2012 solution file
# $2 is start number of LTL solutions

SOLUTIONSFILE=$1
LTL_START=$2

function printresult {
  property=$1
  result=$2
  if [ $result -eq 0 ];
  then
    echo "$property,no,9"
   fi
  if [ $result -eq 1 ]; 
  then
    echo "$property,yes,9"
   fi
}

egrep 'error_??' $SOLUTIONSFILE > tmp_assert1.txt
egrep --only-matching '[0-9]+' tmp_assert1.txt > tmp_assert2.txt

#initialize array to 0
for (( i=0; i<LTL_START; i++));
do
  assert[$i]=0
done

exec<tmp_assert2.txt
while read line
do 
  assertnum=`echo $line`
  assert[$assertnum]=1
done

# output array
for (( i=0; i<LTL_START; i++));
do
  printresult $i ${assert[$i]}
done

# extract LTL solutions
egrep '(Formula\ )' $SOLUTIONSFILE > tmp_ltl1.txt
a=$LTL_START
exec<tmp_ltl1.txt
while read line
do 
  issatisfied=`echo $line | egrep --count 'Formula is satisfied'`
  printresult $a $issatisfied
  a=$(($a+1));
done

rm tmp_assert1.txt
rm tmp_assert2.txt
rm tmp_ltl1.txt

