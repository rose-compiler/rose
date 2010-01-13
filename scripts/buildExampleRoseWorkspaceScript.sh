#!/bin/sh

# /bin/sh -x

# This script builds a ROSE workspace suitable for building projects that use an installed version of ROSE.

if [ $# -ne 2 ]
then
  echo ""
  echo "Error: this script needs two arguments"
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

# Note that the "-p" option is not always portable.
mkdir -p $newWorkspacePath
echo "current directory = $PWD"

# DQ (1/12/2010): Use the name of the specified directory.
# cp -r $srcdir/scripts/buildExampleRoseWorkspaceDirectory $newWorkspacePath
cp -r $srcdir/scripts/buildExampleRoseWorkspaceDirectory/* $newWorkspacePath || exit 1

# Run chmod so that the config directory will be able to be deleted 
# by the make distclean rule in the automated ROSE tests.
chmod -R +w $newWorkspacePath/config

echo "Exiting after setting up new workspace at $newWorkspacePath"

