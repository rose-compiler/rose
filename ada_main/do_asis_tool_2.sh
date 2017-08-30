#!/bin/bash
# Builds a dot_asis tool, and runs it on some test code.
#
# Parameters:
# -d  Turns on debug

# Prevents some informative error messages, so is usually commented out:
#set -o errexit
#trap "__exit_status=$?; echo Error - exiting do_asis_tool_2.sh with status ${__exit_status}; exit ${__exit_status}" ERR

# Defines log, log_and_run, etc.:
source utility_functions.sh

rel_script_dir=`dirname $0`
script_dir=`(cd ${rel_script_dir}; pwd)`
current_dir=`pwd`
tool_name=run_asis_tool_2
target_dir=../test_units
target_units="unit_2.adb"

use_gnat () {
  # Make GNAT compiler and gprbuild available:
  # Needed for use:
  . /usr/local/tools/dotkit/init.sh
  use -q gnat
}

build_asis_tool () {
  log "Building ${tool_name}"
  # -p       Create missing obj, lib and exec dirs
  # -P proj  Use Project File proj
  # -v       Verbose output
  # -vl      Verbose output (low verbosity)
  # -vm      Verbose output (medium verbosity)
  # -vh      Verbose output (high verbosity)
  # -vPx     Specify verbosity when parsing Project Files (x = 0/1/2)
  # -Xnm=val Specify an external reference for Project Files
  gprbuild -p -Pdot_asis.gpr -v -XLIBRARY_TYPE=static -XASIS_BUILD=default ${tool_name}
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


