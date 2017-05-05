#!/bin/bash
#
# Builds Ada/C++ executable and runs it.
# 

# Prevents some informative error messages, so is usually commented out:
#set -o errexit
#trap "__exit_status=$?; echo Error - exiting do_templates.sh with status ${__exit_status}; exit ${__exit_status}" ERR

# Defines log, log_and_run, etc.:
source `which utility_functions.sh`

rel_script_dir=`dirname $0`
script_dir=`(cd ${rel_script_dir}; pwd)`
current_dir=`pwd`
tool_name='bar'

use_gnat () {
  # Make GNAT compiler and gprbuild available:
  # Needed for use:
  . /usr/local/tools/dotkit/init.sh
  use -q gnat
}

build_it () {
  log "Building ${tool_name}"
  gprbuild -p -Pdefault.gpr
}

run_it () {
  log "Running ${tool_name}"
  log_and_run ${script_dir}/obj/${tool_name}
}

log_start
log_invocation "$@"
use_gnat

log_and_run build_it    || exit $?
log_and_run run_it "$@" || exit $?

log_end


