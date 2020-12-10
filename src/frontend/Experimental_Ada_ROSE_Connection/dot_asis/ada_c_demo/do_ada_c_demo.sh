#!/bin/bash
#
# Builds Ada/C++ executable and runs it.
# 

# Prevents some informative error messages, so is usually commented out:
#set -o errexit
#trap "__exit_status=$?; echo Error - exiting do_templates.sh with status ${__exit_status}; exit ${__exit_status}" ERR

# Defines log, log_then_run, etc.:
source `which utility_functions.sh`

rel_script_dir=`dirname $0`
script_dir=`(cd ${rel_script_dir}; pwd)`
current_dir=`pwd`

# Make GNAT compiler and gprbuild available:
use_gnat () {
   module unload gnat
export PATH="${PATH}:/collab/usr/global/tools/rose/toss_3_x86_64_ib/GNAT/2019/bin"

}

log_start
log_invocation "$@"
use_gnat

log_then_run gprbuild -p -Pada_c_demo.gpr || exit $?
log_then_run ${script_dir}/obj/ada_main "$@" 
log_then_run ${script_dir}/obj/c_main "$@"
log_then_run ${script_dir}/obj/c_ada_c_main "$@" 

log_end

