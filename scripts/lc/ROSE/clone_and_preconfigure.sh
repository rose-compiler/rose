#!/bin/bash
# Clones and preconfigures (runs build) ROSE.  Run this before Configure.
# Calls srun_do and run_and_log as needed.

# For debug:
#set -x

rel_script_dir=`dirname $0`
export ROSE_SCRIPT_DIR=`(cd ${rel_script_dir}; pwd)`

# For manual testing:
# export ROSE_PROJECT_BASE="/usr/workspace/wsrzd/charles/code/ROSE"
# Uses ROSE_PROJECT_BASE if set, else sets it:
source ${ROSE_SCRIPT_DIR}/declare_install_functions.sh

# Don't actually run:
#export RUN_OR_NOT_EFFORT_ONLY=TRUE

set_main_vars
clone_latest_workspace
do_preconfigure

