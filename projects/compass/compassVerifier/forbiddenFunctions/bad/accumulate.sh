#!/bin/bash

declare -i NUM=0

for file in `ls ./*.txt`
do
  for func in `sort $file | grep -v "#"`
  do
    echo $func >> compass_parameters.unfinished1
  done
done

sort compass_parameters.unfinished1 | uniq > compass_parameters.unfinished2

echo "### This is a generated file" > compass_parameters

for func in `cat compass_parameters.unfinished2`
do    
  echo "ForbiddenFunctions.Function$((NUM++))=${func},This function is forbidden for use in compass" >> compass_parameters
done

rm -f compass_parameters.unfinished1 compass_parameters.unfinished2

exit 0
