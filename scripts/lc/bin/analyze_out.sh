#!/bin/bash
# Analyze multiple *.out files from e.g. 
#   run_and_log do_sbatch 144 sbatch_process_files.sh source_files.txt ../do_one.sh
# Expects all slurm_*.out files in this directory to be from the same batch.

# Sets strict, defines log, log_then_run, etc.:
source `which utility_functions.sh`

this_script=$0
script_dir=`dirname ${this_script}`

out_file=batch.out

log "Removing any old ${out_file} file"
rm -f ${out_file}

log "Concatenating all slurm-*_*.out files into ${out_file}"
cat slurm-*_*.out > ${out_file}

${script_dir}/analyze_log.sh ${out_file}

