#!/bin/bash

if [ "$#" -ne 2 ]; then
    exit 73 # some meaningless magic number
fi

expected=$1
code=$2

$code

actual="$?"

if [[ "$expected" -ne "$actual" ]]; then
  exit 73 # some meaningless magic number
fi
