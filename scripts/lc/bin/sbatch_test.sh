#!/bin/bash
# This script tells how it was started by srun
# Note: SLURM_ARRAY_TASK_ID is only defined when this script is run via 
# "sbatch --array=1-[N]", as in do_sbatch

# Defines log, log_then_run, etc.:
source `which utility_functions.sh`
log_start
log_invocation "$@"

file_base_name=$1
file_name=${file_base_name}.${SLURM_ARRAY_TASK_ID}
echo "This is task number ${SLURM_ARRAY_TASK_ID}"
echo "Input file is ${file_name}"
echo "Contents are:"
cat ${file_name}

log_end
