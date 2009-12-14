#!/bin/sh

# /bin/sh -x

# This script builds a ROSE workspace suitable for building projects that use an installed version of ROSE.

if [ $# -ne 2 ]
then
  echo "This script needs two arguments"
  echo "   Usage: $0 <sourcedir for ROSE> <proposed ROSE workspace path>"
  echo ""
  echo "   Example: $0 <ROSE srcdir> /home/dquinlan/ROSE/myWorkSpace"
  echo ""
  exit 1
fi

srcdir=$1
echo "srcdir = $srcdir"

newWorkspacePath=$2
echo "newWorkspacePath = $newWorkspacePath"

mkdir -p $newWorkspacePath
echo "current directory = $PWD"
cp -r $srcdir/scripts/buildExampleRoseWorkspaceDirectory $newWorkspacePath

echo "Exiting after setting up new workspace at $newWorkspacePath"

