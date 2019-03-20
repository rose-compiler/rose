#!/bin/bash

echo "========================="
echo "BUG7: ROSE-1867 workaround"
codethorn bug7.C -I.

echo "========================="
echo "BUG8: multiple files."
codethorn -I. bug8a.c bug8b.c
echo "========================="
