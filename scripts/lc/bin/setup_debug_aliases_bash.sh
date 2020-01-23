#!/bin/echo ERROR: Source, do not execute, this script:

# Bash aliases and functions that support script debugging, especially startup 
# and sourced scripts

# Alias caveats:
#
# - Be sure not to refer to another alias in an alias definition, because Bash 
#   does not do nested alias lookup.  
# - There must be a trailing space in the alias definition for the shell to 
#   expand any alias following it on the command line.
# - An alias does not carry to a subshell, so source this script from .bashrc  
#   as well as from .profile.

# Testing whether a variable is set:
#
# To return true when VAR is set and not get "-bash: VAR: unbound variable"  
# when VAR is unset and "set -u" (unset is an error) is in effect:
# [ -n "${VAR+x}" ]
# If VAR is set and not null, -n "${VAR}" is true.
# If VAR is set and null, -n "x" is true.
# If VAR is not set, -n "" is false.
# See POSIX shell, 2.6.2 Parameter Expansion in:
# http://pubs.opengroup.org/onlinepubs/9699919799/utilities/V3_chap02.html#tag_18_06_02

# Expand aliases even in a non-interactive shell, so this script doesn't get errors:
shopt -s expand_aliases

if [ -f $HOME/.debug_setup ]; then
  # Print out debug messages during .*shrc and others:
  export DEBUG_SETUP="TRUE"
else
  unset DEBUG_SETUP
fi

alias _test_debug_setup='[ -n "${DEBUG_SETUP+x}" ]'

alias echo_dashes='echo "----------------------------------------------------------------------"'
alias echo_equals='echo "======================================================================"'

alias time_24='date +%H:%M:%S'
alias date_time_24='date +"%Y-%m-%d_%H:%M:%S"'
alias date_time_24_nano='date +"%Y-%m-%d_%H:%M:%S.%N"'
alias _echo_timestamp='echo `date_time_24`'

# Names of sourced file(s):
alias _echo_source_path_and_line='echo ${BASH_SOURCE[0]}:${LINENO}'
# File name only:
alias _echo_source_file_and_line='echo ${BASH_SOURCE[0]##*/}:${LINENO}'
# Call stack, file names only:
alias _echo_source_file_and_callers='echo "${BASH_SOURCE[*]##*/}'
alias _echo_caller_source_file='echo ${BASH_SOURCE[1]}'
alias _echo_caller_source_file_stack='echo ${BASH_SOURCE[*]:1}'

#-------------------------------------------------------------------------------
# Each of these aliases may be followed by something to be echoed when called, 
# e.g.: 
# echo_error "File foo not found."
#
alias echo_line='echo "`_echo_timestamp` --- `_echo_source_file_and_line`: "'
#echo_line test_line
alias echo_error='echo "`_echo_timestamp` *** ERROR `_echo_source_path_and_line`: "' 
# Like .cshrc:
alias echo_err='echo_error'
#echo_error test_error
alias echo_warning='echo "`_echo_timestamp` !!! WARNING `_echo_source_path_and_line`: "'
#echo_warning test_warning
# Like .cshrc:
alias echo_warn='echo_warning'
alias debug_line='`_test_debug_setup` && echo "`_echo_timestamp` ### - `_echo_source_path_and_line`:" '
#debug_line test_debug

# Log items of interest:
log_setup_items() {
  echo_var -
  echo_var PS1
}

# Log current script (and its sourcers):
begin_setup_script() {
  _test_debug_setup && echo_equals
  # Use _echo_caller... because source is this file:
  _test_debug_setup && echo "`_echo_timestamp` ### BEGIN -->  `_echo_caller_source_file_stack`"
  _test_debug_setup && log_setup_items
  _test_debug_setup && echo_dashes
}

end_setup_script() {
  _test_debug_setup && echo_dashes
  _test_debug_setup && log_setup_items
  # Use _echo_caller... because source is this file:
  _test_debug_setup && echo "`_echo_timestamp` ### END <<---  `_echo_caller_source_file_stack`"
  _test_debug_setup && echo_equals
}

echo_var() {
  eval echo "\#\#\# - $1 = \${$1:-\<not set\>}"
}
alias show_var='echo_var '

debug_var() {
  _test_debug_setup && show_var $1
}

echo_path() {
  echo_dashes
  echo "-- $1" 
  echo_dashes
  eval [ \$$1 ]
  if [ $? == 0 ]; then
    eval echo "\$$1" | tr ":" "\012"
  else 
    echo "<not set>"
  fi
  echo_dashes
}
# Like .cshrc:
alias show_path='echo_path '

echo_paths() {
  echo_equals
  echo "Selected PATH variables"
  echo_equals
  echo_path PATH
  echo_path LD_LIBRARY_PATH
  echo_path MANPATH
  echo_equals
}
# Like .cshrc:
alias show_paths='echo_paths'

echo_all_paths() {
  echo_equals
  echo "All PATH variables $*"
  echo_equals
  # Set POSIX mode in a subshell so that set does not list functions:  
  (set -o posix ; set) | grep PATH= | sort | tr ":=" "\012\012"
  echo_equals
}
# Like .cshrc:
alias show_all_paths='echo_all_paths'

debug_paths() {
  if _test_debug_setup; then
    echo_all_paths $*
  fi    
}

# exit -1 only exits the current script - the sourcer keeps going.
# TODO: Not translated from csh to bash yet:
#alias exit_all_sourced_scripts 'echo "*** (${scriptname}) Generating error to stop all sourced scripts"; e:r:r:o:r: e:r:r:o:r:'

# Parameterized version of: echo_warn "JAVA_BIN: ${JAVA_BIN} does not exist or is not a directory.  JAVA_BIN not added to PATH."
# TODO: Not translated from csh to bash yet:
#alias not_exist_not_added 'echo_warn "\!^ ${\!^} does not exist or is not a directory.  \!^ not added to \!\!:2."'

