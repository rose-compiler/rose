#!/bin/bash
# Defines log, log_then_run, etc.:
source `which utility_functions.sh`
log_start
log_invocation "$@"

# Local variables:
rel_script_dir=`dirname $0`
script_dir=`(cd ${rel_script_dir}; pwd)`
project_dir=`dirname ${script_dir}`
build_dir=${project_dir}/kull_build

. ${rel_script_dir}/set_ROSE_HOME
export LD_LIBRARY_PATH=${ROSE_HOME}/src/.libs:${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH=${ROSE_HOME}/src/3rdPartyLibraries/libharu-2.1.0/src/.libs:${LD_LIBRARY_PATH}
export ROSE_TOOL=${ROSE_HOME}/bin/identityTranslator
# Don't know how to get flags into make thru mpic++ yet:
export ROSE_FLAGS=
#export ROSE_FLAGS='"--edg:no_warnings -rose:keep_going"'
#export ROSE_FLAGS='"--edg:no_warnings -rose:keep_going --debug"'

log "SYS_TYPE:     $SYS_TYPE"
log "ROSE_TOOL:    $ROSE_TOOL"
log "ROSE_FLAGS:   $ROSE_FLAGS"

# Default cmake is too old.  Need at least 3.2:
module load cmake/3.5.2

#############################
#export MPICH_CXX=${ROSE_TOOL}
#############################

# Makes make verbose:
#export AT=""

# Run make with Rose tool:
#############################
# Version fails in regular make, so do it explicitly here:
log_then_run ${script_dir}/do_version.sh
cd ${build_dir}
make_line="make MPICH_CXX=${ROSE_TOOL} -k"
#make_line="make -k"
# Parallel:
log_then_run ${make_line} -j 36 "$@"
# Last time I used -exclusive, srun never started...
# log_then_run srun_do --exclusive -c 36 ${make_line} -j 36 "$@"
# Serial:
# log_then_run srun_do ${make_line} "$@"
#############################

log_end

