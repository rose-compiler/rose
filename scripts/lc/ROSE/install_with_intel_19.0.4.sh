#!/bin/bash
# Configure, make, and install ROSE.  Run after clone_and_preconfigure.sh.
# Calls srun_do and run_and_log as needed.
# This is for Intel 19.0.4, needed by 2020-02-10 ARES

# For debug:
#set -x

enclosing_directory=`dirname $0`
export ROSE_SCRIPT_DIR=`(cd ${enclosing_directory}; pwd)`
source ${ROSE_SCRIPT_DIR}/declare_install_functions.sh

# Don't actually run:
#export RUN_OR_NOT_EFFORT_ONLY=TRUE

# For manual testing:
#export ROSE_BUILD_BASE="${HOME}/code/ROSE"

# Uses ROSE_BUILD_BASE if set, else sets it:
set_main_vars
#clone_latest_workspace
#do_preconfigure
use_latest_existing_workspace
setup_intel_19_0_4_compiler
setup_boost
print_rose_vars
do_intel_configure
make_and_install

