#!/bin/bash
#
# 
# Builds ASIS templates executable, and runs it on some test code.
# 
# Parameters:
# -d  Turns on debug
#

# Prevents some informative error messages, so is usually commented out:
#set -o errexit
#trap "__exit_status=$?; echo Error - exiting do_templates.sh with status ${__exit_status}; exit ${__exit_status}" ERR

# Defines log, log_and_run, etc.:
source `which utility_functions.sh`

rel_script_dir=`dirname $0`
script_dir=`(cd ${rel_script_dir}; pwd)`
current_dir=`pwd`
tool_name=run_asis_tool_2
target_dir=./test_units
target_units="unit_2.adb"

use_gnat () {
  # Make GNAT compiler and gprbuild available:
  # Needed for use:
  . /usr/local/tools/dotkit/init.sh
  use -q gnat
}

build_asis_tool () {
  log "Building ${tool_name}"
  gprbuild -p -Pdot_asis.gpr -XLIBRARY_TYPE=static -XASIS_BUILD=default ${tool_name}.adb
}

# Keeps going.  Returns 1 if any failed, 0 if all succeeded:
process_units () {
  status=0  
  log "Processing specified files in ${target_dir} with ${tool_name}"
  cd ${target_dir}
  for target_unit in ${target_units}
  do
    log "Processing ${target_unit}" 
    log_and_run ${script_dir}/obj/${tool_name} -f ${target_unit} "$@" || status=1
  done
  return ${status}
}

log_start
log_invocation "$@"

use_gnat
log_and_run build_asis_tool    || exit $?
log_and_run process_units "$@" || exit $?

log_end


