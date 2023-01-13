#!/bin/bash -e

DIRS="C_tests C89_std_c89_tests C99_tests C11_tests Cxx_tests Cxx11_tests Cxx14_tests Cxx17_tests C_subset_of_Cxx_tests MicrosoftWindows_C_tests MicrosoftWindows_Cxx_tests"

TEST_TRANSLATOR=../../echoCompiler
for p in $DIRS; do
  make -C $p clean &> /dev/null
  $BEAR -o $p.json --use-cc $TEST_TRANSLATOR \
                   --use-c++ $TEST_TRANSLATOR \
       make TEST_TRANSLATOR=$TEST_TRANSLATOR -k -j24 -C $p status || true
done
