#!/usr/bin/env bash

# Execution wrapper for binary files executed by perf.
# Purpose: perf does not return the proper error code when a binary
#          segfaults. 
#          https://stackoverflow.com/questions/34787205/how-can-i-get-exit-code-of-program-run-with-perf-stat
#
#          This script is invoked from the script that runs perf,
#          i.e., rose-execution-monitor-linux-perf-intel-x86_64.sh
#
# Usage: rose-execution-wrapper outfile specimen arguments..
#        outfile     is the file where the actual exit code will be stored
#        specimen    the executed specimen
#        arguments.. arbitrary long argument list to specimen

outfile="$1"
shift

eval "$@"

errorcode=$?

echo "$errorcode" >"$outfile"
exit $errorcode

