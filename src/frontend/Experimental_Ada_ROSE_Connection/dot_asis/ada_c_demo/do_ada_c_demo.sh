#!/bin/bash
#
# Builds Ada/C++ executables and runs them.
# 

# Prevents some informative error messages, so is usually commented out:
#set -o errexit
#trap "__exit_status=$?; echo Error - exiting do_templates.sh with status ${__exit_status}; exit ${__exit_status}" ERR

rel_script_dir=`dirname $0`
script_dir=`(cd ${rel_script_dir}; pwd)`
current_dir=`pwd`

# Defines log, log_then_run, etc.:
source ${script_dir}/../utility_functions.sh

# Make GNAT compiler and gprbuild available:
use_gnat () {
  export PATH="${PATH}:/collab/usr/global/tools/rose/toss_3_x86_64_ib/GNAT/2019/bin"
}

log_start
log_invocation "$@"
use_gnat

log_then_run gprbuild -p -Pada_c_demo.gpr || exit $?
log_then_run ${script_dir}/obj/ada_main "$@" 
log_then_run ${script_dir}/obj/c_main "$@"
log_then_run ${script_dir}/obj/ada_c_main "$@"
log_then_run ${script_dir}/obj/c_ada_main "$@" 
log_then_run ${script_dir}/obj/c_ada_c_main "$@" 

# incomplete:
# log_then_run ${script_dir}/obj/ada_c_ada_main "$@" 
# log_then_run ${script_dir}/obj/c_c_ada_main "$@" 

log_end

