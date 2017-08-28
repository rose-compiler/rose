#!/bin/bash
#
# 
# Builds ASIS templates executable, and runs it on some test code.
# 
# Parameters:
# -d  Turns on debug
#

# Prevents some informative error messages, so is usually commented out:
#set -o errexit
#trap "__exit_status=$?; echo Error - exiting do_templates.sh with status ${__exit_status}; exit ${__exit_status}" ERR

# Defines log, log_and_run, etc.:
source ../utility_functions.sh

use_gnat () {
  # Make GNAT compiler and gprbuild available:
  # Needed for use:
  . /usr/local/tools/dotkit/init.sh
  use -q gnat
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
  gprbuild -p -Pdot_asis_library.gpr -XLIBRARY_TYPE=static -XASIS_BUILD=default
}

log_start
log_invocation "$@"

use_gnat
log_and_run build_library || exit $?

log_end


