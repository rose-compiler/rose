#!/bin/bash
# Builds a dot_asis tool with an Ada main program, and runs it on some test code.
#
# Parameters:
# -d  Turns on debug

# Prevents some informative error messages, so is usually commented out:
#set -o errexit
#trap "__exit_status=$?; echo Error - exiting do_asis_tool_2.sh with status ${__exit_status}; exit ${__exit_status}" ERR

# This script is in the base directory of this build:
rel_base_dir=`dirname $0`
base_dir=`(cd ${rel_base_dir}; pwd)`

# Defines log, log_and_run, etc.:
source ${base_dir}/../utility_functions.sh

tool_name=run_asis_tool_2
target_dir=${base_dir}/../test_units
target_units="unit_2.adb"

check_for_gnat () {
  log_separator_1
  log "Checking GNAT gprbuild version"
  gprbuild --version || exit -1
}

build_asis_tool () {
  status=0
  log_separator_1
  log "Building ${tool_name}"
  # -p       Create missing obj, lib and exec dirs
  # -P proj  Use Project File proj
  # -v       Verbose output
  # -vPx     Specify verbosity when parsing Project Files (x = 0/1/2)
  # -Xnm=val Specify an external reference for Project Files
  gprbuild \
  -p \
  -P${base_dir}/dot_asis.gpr \
  -v \
  -XLIBRARY_TYPE=static \
  -XASIS_BUILD=default \
  ${tool_name} || status=1
  
  return ${status}
}

# Keeps going.  Returns 1 if any failed, 0 if all succeeded:
process_units () {
  status=0  
  log_separator_1
  log "Processing specified files in ${target_dir} with ${tool_name}"
  cd ${target_dir}
  for target_unit in ${target_units}
  do
    log "Processing ${target_unit}" 
    log_and_run ${base_dir}/obj/${tool_name} -f ${target_unit} "$@" || status=1
  done
  return ${status}
}

log_start
log_invocation "$@"

check_for_gnat
log_and_run build_asis_tool    || exit $?
log_and_run process_units "$@" || exit $?

log_end

