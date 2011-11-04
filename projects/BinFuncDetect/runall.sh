#!/bin/bash
#
# Shell script to run the analysis on a bunch of executables.  The executables have the following directory structure
# relative to the CWD:
#
#   MSVC<version>/EXE/<name>.*                 -- the binary specimen (executables and/or libraries)
#   MSVC<version>/PDB/<name>.pdb               -- debugging information
#   MSVC<version>/functions-6.1-nopdb.csv      -- IDA Pro results without using debugging information
#   MSVC<version>/functions-6.1.csv            -- IDA Pro results using debugging information
#
# The *.csv files have one line per function, with the following comma-separated fields:
#   0: MD5 hash of the entire specimen
#   1: Function entry point virtual address
#   2: Name of function if known (not used by this analysis)
#   3: Hash of function bytes (MD5?) (not used)
#   4: A "position independent" hash of the function bytes (MD5?) (not used)
#   5: The bytes used to calculate the hash in field 3 (used only for its size)
#   6: The bytes used to calculate the hash in field 4 (not used)
#   7: Words representing some kind of IDA Pro flags (not used)

results="results-$(date +%Y%m%d%H%M%S%z)"
mkdir $results || exit 1
echo "Results are being saved in $results"

for specimen in $(find MSVC*/EXE -follow -type f |sort); do
    msvc="${specimen%%/*}"
    base="${specimen##*/}"
    noext="${base%.*}"

    mkdir -p "$results/$msvc" || exit 1
    output="$results/$msvc/$noext"

    echo
    echo "=============================================================================="
    echo "Analysing $base ($msvc)"
    echo "=============================================================================="

    ./analyzer $msvc/functions-6.1{,-nopdb}.csv $specimen 2>&1 >$output.lst |tee $output.out
done

echo "Results were saved in $results"
