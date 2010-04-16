#!/bin/bash
# Please run this under the top source tree of ROSE to find files with identical names
# Contributed by Jeremiah
# Achieved by Liao
# 9/29/2009

ls -R  | sort -f | uniq -cid | sort -rn
