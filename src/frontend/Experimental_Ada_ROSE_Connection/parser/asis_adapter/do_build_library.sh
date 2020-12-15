#!/bin/bash
# Builds the dot_asis library

# Prevents some informative error messages, so is usually commented out:
#set -o errexit
#trap "__exit_status=$?; echo Error - exiting do_build_library.sh with status ${__exit_status}; exit ${__exit_status}" ERR

# This script is in the base directory of this build:
rel_base_dir=`dirname $0`
base_dir=`(cd ${rel_base_dir}; pwd)`
# Defines log, log_and_run, etc.:
source ${base_dir}/../utility_functions.sh

check_for_gnat () {
  log_separator_1
  log "Checking GNAT gprbuild version"
  gprbuild --version || exit -1
}

build_library () {
  # -p       Create missing obj, lib and exec dirs
  # -P proj  Use Project File proj
  # -v       Verbose output
  # -vl      Verbose output (low verbosity)
  # -vm      Verbose output (medium verbosity)
  # -vh      Verbose output (high verbosity)
  # -vPx     Specify verbosity when parsing Project Files (x = 0/1/2)
  # -Xnm=val Specify an external reference for Project Files
  log_then_run \
  gprbuild \
  -p \
  -P ${base_dir}/asis_adapter.gpr \
  -v || exit $?
}

log_start
log_invocation "$@"

check_for_gnat
build_library

log_end

