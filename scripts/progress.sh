#!/usr/bin/env bash

while ((1));
do
  echo ""
  find . -name "*.qmt" | wc ; find . -name "*.o" | wc
  sleep 4
done
