#!/bin/bash -vx
#
# By Chunhua Liao, August 7,2005
set -e
  time ./bt.W|grep " SUCCESSFUL"
  time ./cg.W|grep " SUCCESSFUL"
  time ./ep.W|grep " SUCCESSFUL"
  time ./ft.W|grep " SUCCESSFUL"
  time ./is.W|grep " SUCCESSFUL"
  time ./lu.W|grep " SUCCESSFUL"
  time ./mg.W|grep " SUCCESSFUL"
  time ./sp.W|grep " SUCCESSFUL"

