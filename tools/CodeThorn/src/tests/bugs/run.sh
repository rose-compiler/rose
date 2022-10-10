#!/bin/bash

echo "========================="
echo "BUG7: ROSE-1867 workaround"
codethorn bug7.C -I.

echo "========================="
echo "BUG8: multiple files."
codethorn -I. bug8a.c bug8b.c
echo "========================="

echo "========================="
echo "BUG9: ROSE-1879 workaround"
codethorn -I. bug9a.c bug9b.c
echo "========================="

echo "========================="
echo "BUG10: CODETHORN-30 normalization bug"
codethorn --normalize-all=yes bug10.C --out-of-bounds-analysis-file=bounds.csv
NUM=`wc -l bounds.csv`
if [ "$NUM" == "0" ]; then
    echo "Error: out of bounds detected (normalization wrong)."
    exit 1
fi
echo "========================="
echo "ALL TESTS PASSED."
