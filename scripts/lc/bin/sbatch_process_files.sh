#!/bin/bash
# Process each file (line, actually) listed in $1.${SLURM_ARRAY_TASK_ID}, using $2
# Use in an sbatch array job.
# Example usage:
# sbatch_process_files.sh source_files.txt do_one.sh

# Defines log, log_then_run, etc.:
source `which utility_functions.sh`
log_start
log_invocation "$@"

file_base_name=$1
file_name=${file_base_name}.${SLURM_ARRAY_TASK_ID}
process=$2

line_number=0
status=0

process_line () {
  if [ ${status} -eq 0 ]
  then
    let line_number=line_number+1
    log_then_run ${process} ${line}
  fi
}

process_file () {
  log "Processing ${file_name}"
  # Don't exit on error:
  set +o errexit
  while [ ${status} -eq 0 ]
  do 
    read line
    status=$?
    process_line
  done
  log "Total lines processed: ${line_number}"
}

log "This is task number ${SLURM_ARRAY_TASK_ID}"
log "Input file is ${file_name}"
cat ${file_name} | process_file

log_end
