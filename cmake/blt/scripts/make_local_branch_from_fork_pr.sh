#!/usr/bin/env bash

###############################################################################
# Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
# other BLT Project Developers. See the top-level LICENSE file for details
# 
# SPDX-License-Identifier: (BSD-3-Clause)
###############################################################################

###############################################################################
# Help                                                                         
###############################################################################
Help()
{
   # Display Help
   echo
   echo "This script will make a branch in a local git repo for a PR from a "
   echo "branch in a forked repo. The script must be run inside the local repo."
   echo 
   echo "Syntax: make_local_branch_from_fork_pr [-h | -c num | num]"
   echo "options:"
   echo "-h       Print this help usage message."
   echo "-c <#>   Check whether there is a PR with given number."
   echo "-b <#>   Make local branch from branch associated with given PR number."
   echo
}

###############################################################################
# Process the input args.
###############################################################################
if [ "$1" == "" ]; then
  echo
  echo "You must pass an arg to script. Pass -h to see correct usage."
  exit
fi 

while [ "$1" != "" ]; do
  case $1 in
    -h) # display help message
        Help
        exit;;
    -c) # check PR exists
        shift 
        num=$1
        echo
        echo "Running git ls-remote on origin to see if PR ${num} exists..."
        echo "If you don't see it in the output, it does not exist." 
        echo
        git ls-remote --refs origin | grep "pull/${num}"
        exit;;
    -b) # make local branch for given PR number
        shift
        num=$1
        echo
        echo "Attempting to make local branch from branch on fork for PR ${num}"
        echo
        echo "If successful, you will be on local branch pr-from-fork/${num}"
        echo "when script exits."
        echo
        echo "Run 'git branch' to be sure the new branch exists."
        echo
        echo "You can then push the new branch to the main repo; e.g.,"
        echo "   git push <remote> <branch>"
        echo
        echo "If you make a PR for the new branch, it is a good idea to "
        echo "reference the original PR from the forked repo to track the "
        echo "original PR discussion."
        echo
        git fetch origin pull/${num}/head:pr-from-fork/${num} && git checkout pr-from-fork/${num}
        echo
        
        exit;;
    \?) # incorrect option
        echo
        echo "Error: $1 is an invalid option"
        exit;;
  esac
done
