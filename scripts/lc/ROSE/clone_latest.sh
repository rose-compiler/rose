#!/bin/bash
# Clones ROSE.
# Calls srun_do and run_and_log as needed.

# For debug:
#set -x

rel_enclosing_dir=`dirname $0`
export ROSE_ROSE_SCRIPT_DIR=`(cd ${rel_enclosing_dir}; pwd)`
source ${ROSE_ROSE_SCRIPT_DIR}/declare_install_functions.sh

# Don't actually run:
#export RUN_OR_NOT_EFFORT_ONLY=TRUE

# For manual testing:
#export ROSE_BUILD_BASE="${HOME}/code/ROSE"

# Uses ROSE_BUILD_BASE if set, else sets it:
set_main_vars
print_rose_vars
clone_latest_workspace
print_rose_vars

