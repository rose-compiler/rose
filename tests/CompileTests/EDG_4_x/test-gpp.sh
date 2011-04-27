#!/bin/sh

cp $2/../Cxx_tests/$1 $1
g++ -I$2/../Cxx_tests -c $1 &> /dev/null
if [ $? -ne 0 ]; then \
  echo $1 "has failed."; \
fi
rm $1
