#!/bin/sh
# START WITH SRUN!

# Runs Ares autobuild and make, using a ROSE tool instead of the CXX compiler

# Sets strict mode, defines log, log_then_run, etc.:
source `which utility_functions.sh`

log_start

rel_test_dir=`dirname $0`
test_dir=`(cd ${rel_test_dir}; pwd)`
project_dir=`dirname ${test_dir}`
source_dir=${project_dir}/ares
build_dir=${project_dir}/ares_build

# Sets ROSE_HOME, ROSE_LD_LIBRARY_PATH:
. ${test_dir}/set_ROSE_HOME

# ARES makefile variables:

# Use the MPI wrapper, not the tool directly:
#export ROSE_TOOL_PATH=${test_dir}/rose_mpicxx
# Use the tool directly (assumes it was built to be MPI-aware):
export ROSE_TOOL_PATH="${ROSE_HOME}/tutorial/identityTranslator"
# Use the pass-fail script, which controls the ROSE tool and version:
#export ROSE_TOOL_PATH="${project_dir}/ares_testing/run_rose_tool.py"
# Use the compiler instead:
export ROSE_TOOL_PATH=/usr/tce/packages/mvapich2/mvapich2-2.2-intel-18.0.1/bin/mpic++

export ROSE_FLAGS="--edg:no_warnings -rose:keep_going"
#export ROSE_FLAGS='"--edg:no_warnings -rose:keep_going --debug"'
# Makes make verbose:
export AT=""
# Turns off colorized output in make - good for making log files more readable:
export COLOR=0

log "SYS_TYPE:             $SYS_TYPE"
log "ROSE_HOME:            $ROSE_HOME"
log "ROSE_TOOL_PATH:       $ROSE_TOOL_PATH"
log "ROSE_FLAGS:           $ROSE_FLAGS"
log "ROSE_LD_LIBRARY_PATH: $ROSE_LD_LIBRARY_PATH"

# Must be run in parent of ares dir or gets error:
# "Bunker at path ./ares not found"
cd ${project_dir}
# Run autobuild to bring in cloned changes and to clear out results of 
# previous make (may also create version file):
#Preserve compiled rgst_types.o for now:
#log_then_run ${source_dir}/scripts/bin/ares_autobuild ${build_dir}

# Run make with Rose tool:
#############################
cd ${build_dir}/build
#log_then_run make 
#log_then_run make -k -j 36
log_then_run make -k -j 36 rosetest
#############################

log_end

