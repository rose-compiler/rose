#!/bin/bash
#
# 
# Builds ASIS templates executable, and runs it on some test code.

# Prevents some informative error messages, so is usually commented out:
# set -o errexit

# Defines log, log_and_run, etc.:
source `which utility_functions.sh`

rel_script_dir=`dirname $0`
script_dir=`(cd ${rel_script_dir}; pwd)`
tool_name=run_asis_tool_2
target_dir=./test_units

use_gnat () {
  # Make GNAT compiler and gprbuild available:
  # Needed for use:
  . /usr/local/tools/dotkit/init.sh
  use -q gnat
}

build_asis_tool () {
  log "Building ${tool_name}"
  gprbuild -d -Prose_asis.gpr -XLIBRARY_TYPE=static -XASIS_BUILD=default ${tool_name}.adb
}

generate_adt_files () {
  log "Generating adt files for units in ${target_dir}"
  (cd ${target_dir}; gcc -c -gnatct test_unit.adb)
}

run_asis_tool () {
  log "Running ${tool_name} in ${target_dir}"
  (cd ${target_dir}; ${script_dir}/obj/${tool_name})
}

log_start
log_invocation "$@"

use_gnat
log_and_run build_asis_tool
log_and_run generate_adt_files
log_and_run run_asis_tool

log_end


