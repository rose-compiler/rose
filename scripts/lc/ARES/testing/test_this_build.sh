#!/bin/bash
# Run comp_db_map.sh and render_text.sh in THIS ARES build directory.
# 
# = Expensive! Run with srun_do -c36!
# - Not needed: run_and_log
#
# - Must be run in a directory such as:
#   /g/g17/charles/code/ARES/ares-develop-2019-03-14-build/TOSS3/build
# 
# - There must be a file called compile_commands.json in this directory.
#
# - There must be a softlink called "ares" to the repo dir, two directories up, e.g.:
#   /g/g17/charles/code/ARES/ares-develop-2019-03-14-build/ares ->
#   /g/g17/charles/code/ARES/ares-develop-2019-03-14

# Exit if error or undef variable:
set -eu

# Changeable variables:
export ROSE_VERSION="0.9.10.230"
export FIRST_UNIT="0001"
export  LAST_UNIT="1704"

# Calculated variables:
export CODE_BASE="${HOME}/code"
export ROSE_BASE="${CODE_BASE}/ROSE"
export AUTOMATION_HOME="${ROSE_BASE}/rose-reynolds12-automation/scripts/lc"
export ARES_BASE="${CODE_BASE}/ARES"
export ROSE_HOME="${ROSE_BASE}/rose-master-${ROSE_VERSION}-intel-18.0.1.mpi-intel-18.0.1.mpi"
export REPORT_FILE_NAME_ROOT="report_${FIRST_UNIT}_${LAST_UNIT}"
export JSON_REPORT_FILE_NAME="${REPORT_FILE_NAME_ROOT}.json"
export TEXT_REPORT_FILE_NAME="${REPORT_FILE_NAME_ROOT}.txt"

# Run ROSE on units:
${AUTOMATION_HOME}/bin/run_and_log \
${AUTOMATION_HOME}/compdb/comp_db_map.py \
../../ares \
. \
${ROSE_HOME}/bin/identityTranslator \
--database=compile_commands.json \
--report=${JSON_REPORT_FILE_NAME} \
--start_at=${FIRST_UNIT} \
--end_at=${LAST_UNIT} \
--nprocs=36 \
-- \
-rose:no_optimize_flag_for_frontend \
-rose:skipAstConsistancyTests \


# Make text report:
${AUTOMATION_HOME}/bin/run_and_log \
${AUTOMATION_HOME}/compdb/render_text.py \
--in_file=${JSON_REPORT_FILE_NAME} \
--out_file=${TEXT_REPORT_FILE_NAME} \
--debug \


