#!/bin/bash
#
# 
# Builds ASIS templates executable, and runs it on some test code.

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
target_units="test_unit"

use_gnat () {
  # Make GNAT compiler and gprbuild available:
  # Needed for use:
  . /usr/local/tools/dotkit/init.sh
  use -q gnat
}

build_asis_tool () {
  log "Building ${tool_name}"
  gprbuild -p -Prose_asis.gpr -XLIBRARY_TYPE=static -XASIS_BUILD=default ${tool_name}.adb
}

generate_adt_files () {
  log "Generating .adt files for specified units in ${target_dir}"
  cd ${target_dir}
  for unit in ${units}
  do
    log "Generating ${target_unit}.adt" 
    gcc -c -gnatct ${target_unit}.adb
  done
  cd ${current_dir}
}

run_asis_tool () {
  log "Running ${tool_name} on all .adt files in ${target_dir}"
  cd ${target_dir}
  ${script_dir}/obj/${tool_name}
  cd ${current_dir}
}

log_start
log_invocation "$@"

use_gnat
log_and_run_or_exit build_asis_tool
log_and_run_or_exit generate_adt_files
log_and_run_or_exit run_asis_tool

log_end


