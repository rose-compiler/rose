#!/bin/bash
# Builds a dot_asis tool with a C main program, and runs it on some test code.
#
# Parameters:
# -d  Turns on debug

# Prevents some informative error messages, so is usually commented out:
#set -o errexit
#trap "__exit_status=$?; echo Error - exiting do_asis_tool_2.sh with status ${__exit_status}; exit ${__exit_status}" ERR

# Defines log, log_and_run, etc.:
source ../utility_functions.sh

rel_script_dir=`dirname $0`
script_dir=`(cd ${rel_script_dir}; pwd)`
source_dir=`pwd`

dot_asis_home=/usr/workspace/wsb/charles/code/ROSE/dot_asis/dot_asis_library
gnat_home=/usr/workspace/wsb/charles/bin/adacore/gnat-gpl-2017-x86_64-linux
target_dir=../test_units
target_units="test_unit.adb"
tool_name=call_asis_tool_2

build_asis_tool () {
  log "Building ${tool_name}"
  if [ ! -d obj ]; then
    mkdir obj
  fi
  cd obj
  
  ${gnat_home}/bin/gcc -c -x c -MMD -MF ${tool_name}.d \
  -I${dot_asis_home}/include \
  ${source_dir}/${tool_name}.c

  ${gnat_home}/bin/gcc \
  ${tool_name}.o \
  ${dot_asis_home}/lib/libdot_asis.a \
  ${gnat_home}/lib/asis/asislib/libasis.a \
  ${gnat_home}/lib/gcc/x86_64-pc-linux-gnu/6.3.1/adalib/libgnarl.a \
  ${gnat_home}/lib/gcc/x86_64-pc-linux-gnu/6.3.1/adalib/libgnat.a \
  -static-libgcc \
  -lpthread \
  -lrt \
  -ldl \
  -o ${tool_name}
  cd ${source_dir}
}

# Keeps going.  Returns 1 if any failed, 0 if all succeeded:
process_units () {
  status=0  
  log "Processing specified files in ${target_dir} with ${tool_name}"
  cd ${target_dir}
  for target_unit in ${target_units}
  do
    log "Processing ${target_unit}" 
    log_and_run ${script_dir}/obj/${tool_name} -f ${target_unit} "$@" || status=1
  done
  return ${status}
}

log_start
log_invocation "$@"

log_and_run build_asis_tool    || exit $?
log_and_run process_units "$@" || exit $?

log_end


