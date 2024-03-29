#!/bin/echo ERROR: Source, do not execute, this script:
#
# Sets strict mode, defines set_strict, unset_strict, log_*,
# log_then_run, run_or_not, etc.:
#
# set_strict
# unset_strict 
# do_time_log 
# do_time_file 
# do_date_log 
# do_date_file 
# do_date 
# do_env 
# log 
# log_w_time
# log_w_date
# log_w_line
# log_w_line_w_time
# log_w_line_w_date
# log_no_prefix 
# log_blank 
# log_blank_no_prefix 
# log_separator_0 
# log_separator_1 
# log_separator_2 
# log_separator_3 
# log_start 
# log_invocation 
# log_end
# run_or_not
# log_then_run 
# log_then_run_or_exit 
#
# This file should have execute permission so it can be found by "which", but 
# it should be sourced, not executed.  For example:
#   utility_functions=`which utility_functions.sh`
#   source ${utility_functions}
#
# Note:
# $*, $@ - all args
# "$*"   - all args quoted in one string. 
#          Useful to avoid losing white space (more than one space at a time)
# "$@"   - all args quoted individually 

###############################################################################
# BEGIN bash strict mode setup
script_name=$0
finish () {
  __exit_status=$? 
  if [[ ${__exit_status} -ne 0 ]]
  then 
    echo "finish: ERROR: Stack fields below are: BASH_SOURCE, FUNCNAME, BASH_LINENO:"
    for ((x=0; x < ${#FUNCNAME[*]}; x += 1 ))
    do
      echo "finish: ${BASH_SOURCE[x]}, ${FUNCNAME[x]}, ${BASH_LINENO[x]}"
    done
    echo "finish: ERROR: Exiting ${script_name} with status ${__exit_status}"
  fi
  exit ${__exit_status}
}

# Exit on nonzero status.  Unset variable is err.  Use status of rightmost 
# command in pipeline.  Echo exit status.  Echo trace if status not 0:
set_strict () {
  # See http://redsymbol.net/articles/unofficial-bash-strict-mode/
  # -e, -o errexit -
  #      Exit if a command yields a nonzero exit status.  The ERR trap executes
  #      before the shell exits.
  # -u, -o nounset -
  #      Treat unset variables as errors.
  # -o pipefail -
  #      Change exit status of a pipeline to be that of the rightmost command that
  #      failed, or zero if all exited successfully.
  # Changes behavior of commands chained together with || or &&.
  # Makes script exit if grep fails:
  set -euo pipefail
  trap finish EXIT
}

unset_strict () {
  set +euo pipefail
  # Turn trap off:
  trap EXIT
}

# END bash strict mode setup
###############################################################################

###############################################################################
# Functions to push and pop the current state of set.  

push_set_state () {
  # Using set +u in a subshell so that detection of an unset variable does not 
  # itself trigger an error:
  if [ `(set +u; echo ${SET_STACK:-not_set})` == "not_set" ]
  then
    export SET_STACK_SIZE=0
  else  
    SET_STACK_SIZE=${#SET_STACK[*]}
  fi
  let "SET_STACK_SIZE = SET_STACK_SIZE + 1"
#  echo "SET_STACK[SET_STACK_SIZE - 1]=$-"
  export SET_STACK; SET_STACK[SET_STACK_SIZE - 1]=$-
  # Variable array index is zero-based, so do increment after:
}

pop_set_state () {
  # Using set +u in a subshell so that detection of an unset variable does not 
  # itself trigger an error:
  if [ `(set +u; echo ${SET_STACK_SIZE-not_set})` == "not_set" ]
  then
    echo "pop_set_state: ERROR: SET_STACK_SIZE may not be unset or empty. Calling exit 1."
    exit 1
  elif [ ${SET_STACK_SIZE} -le 0 ]
  then  
    echo "pop_set_state: ERROR: SET_STACK_SIZE may not be 0. Calling exit 1."
    exit 1
  else
    set -${SET_STACK[SET_STACK_SIZE - 1]}
    SET_STACK[SET_STACK_SIZE - 1]=""
    # When SET_STACK_SIZE is one, this fails:
    # let "SET_STACK_SIZE = SET_STACK_SIZE - 1"
    # Therefore this if:
    if [ ${SET_STACK_SIZE} == "1" ]
    then
      SET_STACK_SIZE=0
  else
      let "SET_STACK_SIZE = SET_STACK_SIZE - 1"
    fi
  fi
}

test_push_pop_u () {
  echo $-
  push_set_state
  set +u
  echo $-
  pop_set_state
  echo $-
}

test_nested_push_pops () {
  echo "testing test_nested_push_pops"
#  set -x
  set -eu
  echo $-
  push_set_state
  set +u
  echo $-
  push_set_state
  set +e  
  echo $-
  pop_set_state
  echo $-
  pop_set_state
  echo $-
}

test_pop_never_push () {
  echo "testing test_pop_no_push: Should fail with SET_STACK_SIZE unset."  
  # Using set +u in a subshell so that detection of an unset variable does not 
  # itself trigger an error:
  (set +u; echo "SET_STACK_SIZE: ${SET_STACK_SIZE-not_set}")
#  set -x
  pop_set_state
}

test_too_many_pops () {
  echo "testing test_too_many_pops: Should fail with SET_STACK_SIZE = 0."  
  # Using set +u in a subshell so that detection of an unset variable does not 
  # itself trigger an error:
#  set -x
  push_set_state
  pop_set_state

  (set +u; echo "SET_STACK_SIZE: ${SET_STACK_SIZE-not_set}")
  pop_set_state
}

#test_nested_push_pops
#test_pop_never_push
#test_too_many_pops

# END Functions to push and pop the current state of set.
###############################################################################

do_time_log () {
  date '+%H:%M:%S'
}

do_time_file () {
  date '+%H.%M.%S'
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

Utility_functions_script_file=`basename $0`
log () {
  echo ${Utility_functions_script_file}: "$*"
}

log_w_time() {
  log `do_time_log`: "$*"
}

log_w_date() {
  log `do_date_log`: "$*"
}

log_w_line() {
  log ${BASH_LINENO[0]}: "$*"
}

# Not calling log_w_line from inside this because it adds one to the BASH_LINENO stack:
log_w_line_w_time() {
  log ${BASH_LINENO[0]}: `do_time_log`: "$*"
}

log_w_line_w_date() {
  log ${BASH_LINENO[0]}: `do_date_log`: "$*"
}

# Ignore the intermediate calls in utility_functions.sh.
# Works for two levels of calls (call from _log...internal below)
_log_w_line_internal() {
  echo `basename ${BASH_SOURCE[3]}` ${BASH_LINENO[2]}: "$*"
}

# Works for one level of calls (call directly from run_or_not or log_then_run)
_log_w_line_w_time_internal() {
  _log_w_line_internal `do_time_log`: "$*"
}

# Works for one level of calls (call directly from run_or_not or log_then_run)
_log_w_line_w_date_internal() {
   _log_w_line_internal `do_date_log`: "$*"
}

log_no_prefix () {
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
  log_w_date "== START =="
}

log_invocation () {
  log "command line: \"$0 $*\""
  log "directory:    \"`\pwd -L`\""
  log "host:         \"`hostname`\""
}

# prints the exit code if supplied and exits with it:
log_end () {
  if [ $# -eq 1 ]
  then
    log_w_date "== END == (exiting with status $1)"
    log_separator_2
    # Turn off trap:
    trap "" EXIT
    exit $1
  else
    log_w_date "== END =="
    log_separator_2
  fi
}

# Check RUN_OR_NOT_EFFORT_ONLY and return 1 if it is
# set to FALSE, or not set:
_should_run () {
  if [ -z "${RUN_OR_NOT_EFFORT_ONLY+var_is_set}" ]
  then
    # Not set or set to null
    true; return $?
  else
    # Set to something...
    if [ "${RUN_OR_NOT_EFFORT_ONLY}" == "FALSE" ]
    then
      true; return $?
    else
      false; return $?
    fi
  fi
}

# If RUN_OR_NOT_EFFORT_ONLY is set, does not actually run the command:
run_or_not () {
  if _should_run
  then
    # Temporarily turn off error exit so the exit status can be logged:
    push_set_state
    set -e
    "$@"
    command_status=$?
    pop_set_state
  else
    _log_w_line_w_time_internal "NOT running \"$*\" (RUN_OR_NOT_EFFORT_ONLY is set)"
    command_status=0
  fi
  # Exit with the command status:
  return ${command_status}
}

# If RUN_OR_NOT_EFFORT_ONLY is set, does not actually run the command:
log_then_run () {
  # Caller source file and line:
  caller_info="${BASH_SOURCE[*]:1}:${LINENO}"
  log_separator_0
  log_separator_1
  _log_w_line_w_time_internal "Running: \"$*\""
  log_blank_no_prefix
  # Clone of run_or_not instead of calling it to make using _log_w_line_w_date_internal easier:
  if _should_run
  then
    # Temporarily turn off error exit so the exit status can be logged:
    push_set_state
    set -e
    "$@"
    command_status=$?
    pop_set_state
  else
    log "RUN_OR_NOT_EFFORT_ONLY is set. Not running."
    command_status=0
  fi
  log_blank_no_prefix
  _log_w_line_w_time_internal "Done. (status ${command_status})"
  log_separator_1
  log_separator_0
  # Exit with the command status:
  return ${command_status}
}

# Confusing name (too much like run_and_log), deprecated:
log_and_run () {
  log_separator_3
  log "log_and_run is DEPRECATED.  Use log_then_run.  Calling log_then_run \"$*\""
  log_separator_3
  log_then_run "$@"
}
# Exit the script with the command's status if the command fails:
log_then_run_or_exit () {
  log_then_run "$@" || exit $?
}

