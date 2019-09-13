#!/bin/bash

#======================
# To get latest remote version number:
export ROSE_REMOTE_PROJECT="rose-dev@rosecompiler1.llnl.gov:rose/scratch"
export ROSE_REPO_NAME="rose"
# e.g. "rose-dev@rosecompiler1.llnl.gov:rose/scratch/rose":
export ROSE_REMOTE_REPO="${ROSE_REMOTE_PROJECT}/${ROSE_REPO_NAME}"
export LATEST_ROSE_VERSION=`\
git ls-remote --tags ${ROSE_REMOTE_REPO} | \
sed 's/^[[:alnum:]]*\trefs\/tags\/v//' | \
grep -v '\^{}' | \
sort -t. -k 1,1n -k 2,2n -k 3,3n -k 4,4n | \
tail -n1`
echo ${LATEST_ROSE_VERSION}
#======================

# Jenkins page for merge and new version info
# https://hudson-rose-44.llnl.gov:9443/jenkins/job/development-merger/


