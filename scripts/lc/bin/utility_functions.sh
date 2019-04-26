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
  trap finish EXIT
}

unset_strict () {
  set +euo pipefail
  # Turn trap off:
  trap "" EXIT
}

set_strict
# END bash strict mode setup
###############################################################################

# Changes all \"this that\" to "this that"
#         all this\ that to this that
#         all "this that" to '"this that"'
#         all ''" to '" (in case of recursive calls via shell functions)
#         all "'' to "' (same)
# Uses greedy matching, so only deals with the outermost pair of \"
fix_escapes() {
  # Parameters need special quoting in case one is a parameter to sed, like 
  # "-n".  Two (#4, #5) sed patterns get rid of the single quotes added below: 
  echo \'"$@"\' | sed \
  -e 's/^"//g' \
  -e 's/"$//g' \
  -e 's/\\"/"/g' \
  -e 's/\\ / /g' \
  -e 's/"\(.*\)"/\x27"\1"\x27/g' \
  -e 's/^\x27//g' \
  -e 's/\x27$//g' \
  -e 's/\x27\x27"/\x27"/g' \
  -e 's/"\x27\x27/"\x27/g' \
  -
}

# Calls fix_escapes on each parm.  If a parm has quotes (escaped or not), the 
#   shell may have split it into multiple parms.  Escapes each quote inside 
#   quotes.  Assumes all ending quotes are at the end of a parm.
# Can't turn off greedy matching in sed in fix_escapes.  Feeds it one (posibly 
#   space-concatenated) parameter at a time, otherwise it just qoutes the most 
#   outer quotes.
fix_escapes_in_each_parm() {
  local new_parm=''
  local is_front_new_parm='true'
  local new_parm_line=''
  local is_front_new_parm_line='true'
  local is_in_quotes='false'
  for parm in "$@"; do
    if [[ ${parm} =~ .*\\\".* ]]; then
      if [[ ${is_in_quotes} = 'true' ]]; then 
        is_in_quotes='false'
      else 
        is_in_quotes='true'
      fi
    fi
    if [[ ${is_front_new_parm} = 'true' ]]; then
      new_parm="${parm}"
      is_front_new_parm='false'
    else
      new_parm="${new_parm} ${parm}"
    fi
#echo "NEW_PARM: >$new_parm<"
    # Done building new_parm if we are not in quotes now.  Process it and add 
    # it to new_parm_line:
    if [[ ${is_in_quotes} = 'false' ]]; then
      new_parm=`fix_escapes ${new_parm}`
#echo "NEW_PARM_FIXED: >$new_parm<"
      if [[ ${is_front_new_parm_line} = 'true' ]]; then
        new_parm_line="${new_parm}"
        is_front_new_parm_line='false'
      else
        new_parm_line="${new_parm_line} ${new_parm}"
      fi
    new_parm=''
    is_front_new_parm='true'
    fi
  done
  echo ${new_parm_line}
}


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

script_file=`basename $0`
log () {
  echo ${script_file}: "$*"
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

# One more stack level deep:
_log_w_line_internal() {
  log ${BASH_LINENO[1]}: "$*"
}

_log_w_line_w_time_internal() {
  log ${BASH_LINENO[1]}: `do_time_log`: "$*"
}

_log_w_line_w_date_internal() {
  log ${BASH_LINENO[1]}: `do_date_log`: "$*"
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

log_then_run () {
  # Caller source file and line:
  caller_info="${BASH_SOURCE[*]:1}:${LINENO}"
#  local command_line
#log_w_line "$@"
#  command_line=`fix_escapes_in_each_parm "$@"`
#log_w_line ${command_line}
  log_separator_0
  log_separator_1
  _log_w_line_w_time_internal "Running: \"$*\""
#  _log_w_line_internal "Running: ${command_line}"
  log_blank_no_prefix
  # Temporarily turn off error exit so the exit status can be logged:
  unset_strict
  "$@"
#  ${command_line}
  log_then_run_status=$?
  # Turn err exit and trap again:
#  set_strict
  log_blank_no_prefix
  _log_w_line_w_time_internal "Done. (status ${log_then_run_status})"
  log_separator_1
  log_separator_0
  # Exit with the right run status:
  return ${log_then_run_status}
}

# Confusing name (too much like run_and_log), deprecated:
log_and_run () {
#  local command_line
#  command_line=`fix_escapes_in_each_parm "$@"`
  log_separator_3
  log "log_and_run is DEPRECATED.  Use log_then_run.  Calling log_then_run \"$*\""
#  log "log_and_run is DEPRECATED.  Use log_then_run.  Calling log_then_run ${command_line}"
  log_separator_3
  log_then_run "$@"
#  log_then_run ${command_line}
}
# Exit the script with the command's status if the command fails:
log_then_run_or_exit () {
#  local command_line
#  command_line=`fix_escapes_in_each_parm "$@"`
#  log_then_run ${command_line} || exit $?
  log_then_run "$@" || exit $?
}

