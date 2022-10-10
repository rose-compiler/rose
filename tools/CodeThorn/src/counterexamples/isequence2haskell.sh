#!/bin/bash
# transform a list of inputs into a haskell datatype
set -e
echo "inputsequences = \"$2\""
awk -f int2rers.awk $1 >$2
