#!/bin/sh

find . -name Makefile.in | xargs rm
rm -rf aclocal.m4 autom4te.cache/ config.sub config.guess configure missing depcomp install-sh ltmain.sh

