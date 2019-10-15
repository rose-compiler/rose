#!/bin/bash
# Runs comp_db_map.sh and render_text.sh 
# Uses srun_do
#
# USAGE: 
# run_and_log test_this_build.sh <first unit number> <last unit number>
# e.g. run_and_log test_this_build.sh 0001 0400
# 
# DEPENDENCIES:
#   ./set_ROSE_HOME
#   ${BUILD_HOME}/compile_commands_no_fortran.json

# Exit if error or undef variable:
set -eu

# Find ourselves:
rel_script_dir=`dirname $0`
export CONTAINING_DIR=`(cd ${rel_script_dir}; pwd)`

# Run in this script's dir:
cd ${CONTAINING_DIR}

# Get parms:
if [ $# -eq 2 ]
then
  export FIRST_UNIT=$1
  export  LAST_UNIT=$2
else
  export FIRST_UNIT="0001"
  export  LAST_UNIT="0429"
fi

# Set ROSE version.  Sets:
#   CODE_BASE
#   ROSE_PROJECT_BASE
#   ROSE_HOME
#   AUTOMATION_HOME (for automation scripts - may not be in ROSE_HOME)
source set_ROSE_HOME

# Sets strict mode, defines set_strict, unset_strict, log_*,
# log_then_run, run_or_not, etc.:
source ${AUTOMATION_HOME}/bin/utility_functions.sh

export SOURCE_HOME="${CODE_BASE}/ARDRA/ardra/transport3d"
export BUILD_HOME="${CODE_BASE}/ARDRA/ardra/ardra-toss_3_x86_64_ib"
export COMPILATION_DATABASE_PATH="${BUILD_HOME}/compile_commands_no_fortran.json"

export REPORT_FILE_NAME_ROOT="report_${FIRST_UNIT}_${LAST_UNIT}"
export JSON_REPORT_FILE_NAME="${REPORT_FILE_NAME_ROOT}.json"
export TEXT_REPORT_FILE_NAME="${REPORT_FILE_NAME_ROOT}.txt"

export SRUN_DO="${AUTOMATION_HOME}/bin/srun_do"
export COMP_DB_MAP="${AUTOMATION_HOME}/compdb/comp_db_map.py"
export RENDER_TEXT="${AUTOMATION_HOME}/compdb/render_text.py"
export ROSE_TOOL="${ROSE_HOME}/bin/identityTranslator"


# Run ROSE on units (Expensive!  Use srun!):
log_then_run ${SRUN_DO} -c36 \
${COMP_DB_MAP} \
${SOURCE_HOME} \
${BUILD_HOME} \
${ROSE_TOOL} \
--database=${COMPILATION_DATABASE_PATH} \
--report=${JSON_REPORT_FILE_NAME} \
--start_at=${FIRST_UNIT} \
--end_at=${LAST_UNIT} \
--nprocs=36 \
-- \
-rose:no_optimize_flag_for_frontend \
-rose:skipAstConsistancyTests \

# Make text report (Cheap. srun not needed):
log_then_run \
${RENDER_TEXT} \
--in_file=${JSON_REPORT_FILE_NAME} \
--out_file=${TEXT_REPORT_FILE_NAME} \
--debug \

# Look at the output:
log_then_run \
tail -n 20 ${TEXT_REPORT_FILE_NAME}
