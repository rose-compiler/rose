#!/bin/bash
# Runs comp_db_map.sh and render_text.sh using srun_do
#
# USAGE: 
# run_and_log test_this_build.sh <first unit number> <last unit number>
# e.g. run_and_log test_thiis_build.sh 0001 0400
# 
# - Dependencies:
#   ./set_ROSE_HOME
#   ${BUILD_HOME}/compile_commands.json

# Exit if error or undef variable:
set -eu

if [ $# -eq 2 ]
then
  export FIRST_UNIT=$1
  export  LAST_UNIT=$2
else
  export FIRST_UNIT="0001"
  export  LAST_UNIT="2237"
fi

# Set ROSE version.  Sets:
#   CODE_BASE
#   ROSE_PROJECT_BASE
#   ROSE_HOME
#   AUTOMATION_HOME (for scripts that may be different than those in ROSE_HOME)
source set_ROSE_HOME
# Defines log_then_run:
source ${AUTOMATION_HOME}/bin/utility_functions.sh
export ROSE_TOOL=${ROSE_HOME}/bin/identityTranslator

export SOURCE_HOME="${CODE_BASE}/KULL/kull-master-2019-02-11"
export BUILD_HOME="${CODE_BASE}/KULL/kull-master-2019-02-11-gnu-4.9.3.mvapich2.2.2"

export REPORT_FILE_NAME_ROOT="report_${FIRST_UNIT}_${LAST_UNIT}"
export JSON_REPORT_FILE_NAME="${REPORT_FILE_NAME_ROOT}.json"
export TEXT_REPORT_FILE_NAME="${REPORT_FILE_NAME_ROOT}.txt"

# Run ROSE on units (Expensive!  Use srun!):
srun_do -c36 \
${AUTOMATION_HOME}/compdb/comp_db_map.py \
${SOURCE_HOME} \
${BUILD_HOME} \
/usr/bin/timeout \
--database=${BUILD_HOME}/compile_commands.json \
--report=${JSON_REPORT_FILE_NAME} \
--start_at=${FIRST_UNIT} \
--end_at=${LAST_UNIT} \
--nprocs=36 \
--filter f:-cxx=/usr/tce/packages/gcc/gcc-4.9.3/bin/g++ \
-- \
300s \
${ROSE_TOOL} \
-rose:no_optimize_flag_for_frontend \
-rose:skipAstConsistancyTests \

# Make text report:
log_then_run \
${AUTOMATION_HOME}/compdb/render_text.py \
--in_file=${JSON_REPORT_FILE_NAME} \
--out_file=${TEXT_REPORT_FILE_NAME} \
--debug \

# Look at the output:
log_then_run \
tail -n 20 ${TEXT_REPORT_FILE_NAME}
