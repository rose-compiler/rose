#!/bin/bash
#Push internal repo to external repo on Hopper (nersc)

cd /home/panas2/c++workspace/nersc_rose/ROSE-nersc
echo "Checking out Internal-rose-rc"
git checkout internal-rose-rc
echo "Pull from master"
git pull
echo "Checkout remote master"
git checkout master
echo "Merge with rose-llnl"
git merge internal-rose-rc
echo "Push to Hopper"
git push


