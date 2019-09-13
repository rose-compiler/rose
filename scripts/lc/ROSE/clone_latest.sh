#!/bin/bash
# Just clone ROSE 0.9.11.XX:

# This script calls srun_do and run_and_log as needed, so no need to call it with them.
# For debug:
#set -x

export REL_SCRIPT_DIR=`dirname $0`
export ROSE_SCRIPT_DIR=`(cd ${REL_SCRIPT_DIR}; pwd)`
export ROSE_PROJECT_BASE="${HOME}/code/ROSE"

# Depends on ROSE_PROJECT_BASE:
source ${ROSE_SCRIPT_DIR}/declare_install_functions.sh

set_main_vars
clone_latest_workspace

