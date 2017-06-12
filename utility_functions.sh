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

log_and_run () {
  log_separator_0
  log_separator_1
  log "running:      \"$*\""
  log_blank_no_prefix
  "$@"
  log_and_run_status=$?
  log_blank_no_prefix
  log "done running: \"$*\" (status ${log_and_run_status})"
  log_separator_1
  # Exit with the right run status:
  return ${log_and_run_status}
}

# Exit the script with the command's status if the command fails:
log_and_run_or_exit () {
  log_and_run "$@" || exit $?
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


