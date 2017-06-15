#!/bin/bash
#
# Generates the corresponding Ada spec from a C++ header file.
#

# Defines log, log_and_run, etc.:
source `which utility_functions.sh`

# Make GNAT compiler and gprbuild available:
use_gnat () {
  # Needed for use:
  . /usr/local/tools/dotkit/init.sh
  use -q gnat
}

log_start
log_invocation "$@"
use_gnat
# -C extract comments from headers and attempt to generate corresponding Ada comments
# -fdump-ada-spec-slim: generate a single Ada file and not the transitive closure
# -D<macro>: define <macro>
log_and_run g++ -c -fdump-ada-spec $1 || exit $?
log_end

