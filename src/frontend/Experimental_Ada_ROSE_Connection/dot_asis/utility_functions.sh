#!/bin/echo ERROR: Source, do not execute, this script:
#
# These are utility functions for use by demo_srun, runnee, do_srun, etc.
#
# This file should have execute permission so it can be found by "which", but 
# it should be sourced, not executed.  For example:
#   utility_functions=`which utility_functions.sh`
#   source ${utility_functions}

# Note:
# $*, $@ - all args
# "$*"   - all args quoted in one string
# "$@"   - all args quoted individually

# BEGIN bash strict mode setup
script_name=$0
finish () {
  __exit_status=$? 
  if [[ ${__exit_status} -ne 0 ]]
  then 
    echo "ERROR: Exiting ${script_name} with status ${__exit_status}"
  fi
  exit ${__exit_status}
}

set_strict () {
  # See http://redsymbol.net/articles/unofficial-bash-strict-mode/
  # -e, -o errexit -
  #      Exit if a command yields a nonzero exit status.  The ERR trap executes
  #      before the shell exits.
  # -u, -o nounset -
  #      Treat unset variables as errors.
  # -o pipefail -
  #      Change exit status of a pipeline to be that of the rightmost commad that
  #      failed, or zero if all exited successfully.
  # Changes behavior of commands chained together with || or &&.
  # Makes script exit if grep fails:
  set -euo pipefail
}

unset_strict () {
  set +euo pipefail
}

trap finish EXIT
set_strict
# END bash strict mode setup

script_file=`basename $0`

log () {
  # Quote args to avoid losing white space:
  echo ${script_file}: "$*"
}

log_no_prefix () {
  # Quote args to avoid losing white space:
  echo "$*"
}

log_blank () {
  log ""
}

log_blank_no_prefix () {
  log_no_prefix ""
}

log_separator_0 () {
  log_blank
}

log_separator_1 () {
  log "------------------------------------------------------------"
}

log_separator_2 () {
  log "============================================================"
}

log_separator_3 () {
  log "############################################################"
}

log_start () {
  log_separator_2
  log "start time:   `do_date_log`"
}

log_invocation () {
  log "command line: \"$0 $*\""
  log "directory:    \"`\pwd -L`\""
  log "host:         \"`hostname`\""
}

log_end () {
  log_separator_0
  log "end time:     `do_date_log`"
  log_separator_2
}

log_then_run () {
  log_separator_0
  log_separator_1
  log "running:      \"$*\""
  log_blank_no_prefix
  "$@"
  log_then_run_status=$?
  log_blank_no_prefix
  log "done running: \"$*\" (status ${log_then_run_status})"
  log_separator_1
  # Exit with the right run status:
  return ${log_then_run_status}
}

# Confusing name (too much like run_and_log), deprecated:
log_and_run () {
  log_separator_3
  log "log_and_run is deprecated.  Use log_then_run.  Calling log_then_run \"$*\""
  log_separator_3
  log_then_run "$@"
}
# Exit the script with the command's status if the command fails:
log_then_run_or_exit () {
  log_then_run "$@" || exit $?
}

do_date_log () {
  date '+%Y/%m/%d %H:%M:%S'
}

do_date_file () {
  date '+%Y-%m-%d_%H.%M.%S'
}

do_date () {
  do_date_log
}

do_env () {
  env | sort
}


