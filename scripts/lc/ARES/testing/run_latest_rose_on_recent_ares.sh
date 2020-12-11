#!/bin/bash
# Runs comp_db_map.sh and render_text.sh 
# Uses srun_do
#
# USAGE: 
# run_and_log test_this_build.sh <first unit number> <last unit number>
# e.g. run_and_log test_this_build.sh 0001 0400
# 
# DEPENDENCIES:
#   ${BUILD_HOME}/ares (a softlink to the ARES repo dir, two directories up, e.g.:
#     ./ares -> /g/g17/charles/code/ARES/ares-develop-2019-03-14 or:
#     ./ares -> ../../ares-develop-2019-03-14
#   ${BUILD_HOME}/compile_commands.json
#   ${ROSE_HOME}/bin/identityTranslator
#   ${ROSE_ROSE_SCRIPT_DIR}/declare_install_functions.sh

# Don't actually run:
#export RUN_OR_NOT_EFFORT_ONLY=TRUE

# Find ourselves:
rel_enclosing_dir=`dirname $0`
export TEST_SCRIPT_DIR=`(cd ${rel_enclosing_dir}; pwd)`
export ROSE_ROSE_SCRIPT_DIR=`(cd ${rel_enclosing_dir}/../../ROSE; pwd)`

# Get parms:
if [ $# -eq 2 ]
then
  export FIRST_UNIT=$1
  export  LAST_UNIT=$2
else
  export FIRST_UNIT="0001"
  export  LAST_UNIT="1727"
fi

# Declares and sets:
#   log_then_run
#   set_strict
#   SRUN_DO
#   use_latest_gcc_rose
#   use_latest_intel_rose
source ${ROSE_ROSE_SCRIPT_DIR}/declare_install_functions.sh

# Sets:
#   COMMON_BUILD_BASE
#   COMP_DB_MAP
#   RENDER_TEXT
#   ROSE_HOME
#   ROSE_LD_LIBRARY_PATH
#   ROSE_TOOL
use_latest_intel_19_0_4_rose

# Overriding COMMON_BUILD_BASE to make it match the directory string in 
# compile_commands.json:
export COMMON_BUILD_BASE="/usr/WS2/charles/code"

export PROJECT_BASE=${COMMON_BUILD_BASE}/ARES
export PROJECT_BRANCH_VERSION="develop-2020-01-10"
export PROJECT_BUILD_DIR=${PROJECT_BASE}/ares-${PROJECT_BRANCH_VERSION}-build

export SOURCE_HOME=${PROJECT_BUILD_DIR}/ares
export BUILD_HOME=${PROJECT_BUILD_DIR}/TOSS3/build
export COMPILATION_DATABASE_PATH=${BUILD_HOME}/compile_commands.json

export REPORT_FILE_NAME_ROOT="latest_rose_recent_ares_report_${FIRST_UNIT}_${LAST_UNIT}"
export JSON_REPORT_FILE_NAME="${REPORT_FILE_NAME_ROOT}.json"
export TEXT_REPORT_FILE_NAME="${REPORT_FILE_NAME_ROOT}.txt"

# Run in this script's dir:
cd ${TEST_SCRIPT_DIR}

# Run ROSE on units (Expensive!  Use srun!):
log_then_run \
${SRUN_DO} -c36 \
${COMP_DB_MAP} \
${SOURCE_HOME} \
${BUILD_HOME} \
${ROSE_TOOL} \
--database=${COMPILATION_DATABASE_PATH} \
--report=${JSON_REPORT_FILE_NAME} \
--start_at=${FIRST_UNIT} \
--end_at=${LAST_UNIT} \
--nprocs=36 \
--filter r:-gxx-name=/usr/tce/packages/gcc/gcc-6.1.0/bin/g++:-gxx-name=/usr/tce/packages/gcc/gcc-4.9.3/bin/g++ \
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
