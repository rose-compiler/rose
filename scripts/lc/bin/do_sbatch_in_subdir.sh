#!/bin/bash
# Parameters:
# <file_to_slice> <number of slices> <number of tasks> <command for each line>
# Description:
#   Creates a new directory for this run, copies <file to slice> to it, does the rest in there.
#   Slices <file_to_slice> into <number of slices>.
#   Does: do_sbatch <number of slices> sbatch_process_files.sh <file to slice> <command for each line>.
#   <command for each line> is run inside the created directory, so it must be built in, on the path, an absolute path, or start with "../".
#   <number of tasks> may be smaller than <number of slices>, e.g. 2 for testing before a full run.

# Example usage:
#   run_and_log do_sbatch_in_subdir.sh source_files.txt 144 2 ../do_one.sh

# Sets strict, defines log, log_then_run, etc.:
source `which utility_functions.sh`

file_to_slice=$1
number_of_slices=$2
number_of_tasks=$3
command_for_each_line=$4

start_time=`do_date_file`
short_command_name=`basename ${command_for_each_line}`
short_file_name=`basename ${file_to_slice}`
subdir_name="${short_command_name}__${short_file_name}__${number_of_slices}__${number_of_tasks}__${start_time}"

log_invocation "$@"

log "Making new directory ${subdir_name}"
mkdir ${subdir_name}
cd ${subdir_name}
cp ../${file_to_slice} .
slice_file ${file_to_slice} ${number_of_slices}

# e.g. do_sbatch 144 sbatch_process_files.sh source_files.txt do_one.sh
log_then_run do_sbatch ${number_of_tasks} sbatch_process_files.sh ${file_to_slice} ${command_for_each_line}

log_end
