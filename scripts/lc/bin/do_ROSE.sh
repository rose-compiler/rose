#!/bin/bash
# Runs a ROSE Tool against a source file.
# Parm $1 is a bash script to be sourced, relative to the current dir,
# that sets source_path and RUN_IT_ARGS

# Find ourselves:
rel_enclosing_dir=`dirname $0`
export TEST_SCRIPT_DIR=`(cd ${rel_enclosing_dir}; pwd)`
export ROSE_ROSE_SCRIPT_DIR=`(cd ${rel_enclosing_dir}/../ROSE; pwd)`

# Get parms:
parm_source_script=$1

# Declares and sets:
#   log_...
#   set_strict
#   use_latest_gcc_rose
#   use_latest_intel_rose
source ${ROSE_ROSE_SCRIPT_DIR}/declare_install_functions.sh
log_start

# Sets:
#   CC
#   CXX
#   ROSE_BACKEND_CXX
#   ROSE_HOME
#   ROSE_LD_LIBRARY_PATH
#   ROSE_TOOL
#use_specific_intel_rose 0.9.13.17
# use_latest_intel_rose
use_latest_intel_19_0_4_rose
#use_latest_gcc_rose_with_profiling
print_rose_vars

# Sets:
#   source_path
#   RUN_IT_ARGS
source ${parm_source_script}

source_dir=`dirname ${source_path}`
source_path_no_suffix=${source_path%.*}
object_path=${source_path_no_suffix}.o

# /bin/ is executable in ROSE install dir:
# /tutorial/ is libtool script in ROSE build dir:
export ROSE_TOOL_PATH=${ROSE_HOME}/bin/identityTranslator
# Needed to run a ROSE Tool:
#LD_LIBRARY_PATH=${ROSE_HOME}/src/.libs:${LD_LIBRARY_PATH}
#LD_LIBRARY_PATH=${ROSE_HOME}/src/3rdPartyLibraries/libharu-2.1.0/src/.libs:${LD_LIBRARY_PATH}

#export MPICH_CXX=${ROSE_TOOL_PATH}
#export CXX="/usr/tce/packages/mvapich2/mvapich2-2.2-gcc-4.9.3/bin/mpic++"
#export CXX="/usr/tce/packages/mvapich2/mvapich2-2.2-gcc-4.9.3/bin/mpic++ -echo"
#export CXX="/usr/tce/packages/mvapich2/mvapich2-2.2-gcc-4.9.3/bin/mpic++ -cxx=${ROSE_TOOL_PATH}"
#export CXX=/usr/dnta/kull/developers/tools/compilers/mvapich2-2.2/gcc-4.9.3p/mpicxx
#export ROSE_CXX=${CXX}
export ROSE_CXX=${ROSE_TOOL_PATH}

_show_source () {
  log "$1:"
  cat $1
  log "End $1."
  log_separator_1
}

show_sources () {
  log_separator_1
  log "Sources:"
  log_separator_1
  # List the original, and any extra .h or other files:
  for source in ${source_path_no_suffix}*.[ch]*
  do
    if [ -f ${source} ]
    then
      _show_source ${source}
    fi
  done
  log "End sources"
  log_separator_1
}

count_preprocessed_lines () {
  # Takes a little while to run, so the log function below is separate 
  # in case of multiple calls.
  #  Using the ROSE tool and not the compiler because compiler can get:
  # ... cannot open source file "mpi.h": is a directory
  #     #include <mpi.h>
  #  /usr/tce/packages/intel/intel-16.0.3/bin/icc 

  log "Counting preprocessed lines..."
  preprocessed_line_count=`run_it \
  ${ROSE_CXX} \
  -E \
  | wc -l`
  status=$?
  return ${status}
}

log_preprocessed_line_count () {
  log_separator_1
  log "Preprocessed line count: $preprocessed_line_count"
  status=$?
  log_separator_1
  return ${status}
}

run_tool () {
  log "RUNNING ROSE TOOL"
#  log "MPICH_CXX=${MPICH_CXX}"
  run_it \
  log_then_run \
  timeout 15m \
  ${ROSE_CXX} \
  ${ROSE_ARGS} \
  -c \
  -o ${object_path} 
  status=$?
  return ${status}
}

run_tool_with_valgrind () {
  log "RUNNING ROSE TOOL WITH PROFILING"
  run_it \
  log_then_run \
  valgrind --tool=callgrind \
  ${ROSE_CXX} \
  ${ROSE_ARGS} \
  -c \
  -o ${object_path} 
  status=$?
  return ${status}
}

# -H: print includes
run_compiler () {
  log "RUNNING COMPILER BEFORE RUNNING TOOL"
  run_it \
  log_then_run \
  ${ROSE_BACKEND_CXX} \
  -H \
  -c \
  -o ${object_path} 
  status=$?
  return ${status}
}

run_it () {
  last_err_status=0
  $@ \
  ${RUN_IT_ARGS} \
  -I${source_dir} \
  ${source_path}
  status=$?
  if [ ${status} -ne 0 ]
  then
    last_err_status=${status}
  fi
  return ${status}
}

set_memory_limit () {
  # rzgenie nodes have 36 cores and 128G memory.  Don't take more than our share:
  # Don't use log_then_run. so we don't get unwanted "status=" lines:
  limit_cmd="ulimit -v 3000000" # 3G
  log "Running ${limit_cmd}"
  ${limit_cmd}
  status=$?
  # Note:  Normally ulimit -v is "unlimited".  If you set it to a number, then try 
  # to set it to unlimited again, or to a larger number, you get (on rzgenie 2018/05/23):
  # "bash: ulimit: virtual memory: cannot modify limit: Operation not permitted"
  return ${status}
}

set_memory_limit
show_sources
count_preprocessed_lines
log_preprocessed_line_count
##run_tool
##run_tool_with_valgrind
run_compiler && log_preprocessed_line_count && run_tool
##run_compiler && run_tool_with_valgrind
# Doesn't get called when run_tool exits witn an error:
log_preprocessed_line_count

exit ${last_err_status}

