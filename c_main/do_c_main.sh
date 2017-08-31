#!/bin/bash
# Builds a dot_asis tool with a C main program, and runs it on some test code.

# Prevents some informative error messages, so is usually commented out:
#set -o errexit
#trap "__exit_status=$?; echo Error - exiting do_asis_tool_2.sh with status ${__exit_status}; exit ${__exit_status}" ERR

# This script is in the base directory of this build:
rel_base_dir=`dirname $0`
base_dir=`(cd ${rel_base_dir}; pwd)`

# Defines log, log_and_run, etc.:
source ${base_dir}/../utility_functions.sh

gnat=`which gnat` || exit -1
gnat_bin=`dirname ${gnat}`
gnat_home=`dirname ${gnat_bin}`
asis_lib_dir=${gnat_home}/lib/asis/asislib
gnat_lib_dir=${gnat_home}/lib/gcc/x86_64-pc-linux-gnu/6.3.1/adalib

gcc=`which gcc` || exit -1
gcc_bin=`dirname ${gcc}`
gcc_home=`dirname ${gcc_bin}`
# Override the default gcc if needed:
# gcc_home=/usr/apps/gnu/4.9.3
gcc_home=/nfs/casc/overture/ROSE/opt/rhel7/x86_64/gcc/4.8.4/mpc/1.0/mpfr/3.1.2/gmp/5.1.2

dot_asis_home=${base_dir}/../dot_asis_library
dot_asis_lib_dir=${dot_asis_home}/lib

obj_dir=${base_dir}/obj
target_dir=${base_dir}/../test_units
target_units="test_unit.adb"
tool_name=call_asis_tool_2

build_asis_tool () {
  status=0
  log ""
  log "Building ${tool_name}"
  if [ ! -d ${obj_dir} ]; then
    mkdir ${obj_dir} || status=1
  fi
  cd ${obj_dir}
  
  ${gcc_home}/bin/gcc -c -x c -MMD -MF ${tool_name}.d \
  -I${dot_asis_home}/include \
  ${base_dir}/${tool_name}.c

  ${gcc_home}/bin/gcc \
  ${tool_name}.o \
  ${dot_asis_lib_dir}/libdot_asis.a \
  ${asis_lib_dir}/libasis.a \
  ${gnat_lib_dir}/libgnarl.a \
  ${gnat_lib_dir}/libgnat.a \
  -static-libgcc \
  -lpthread \
  -lrt \
  -ldl \
  -o ${tool_name} || status=1
  
  return ${status}
}

# Keeps going.  Returns 1 if any failed, 0 if all succeeded:
process_units () {
  status=0  
  log "Processing specified files in ${target_dir} with ${tool_name}"
  cd ${target_dir}
  for target_unit in ${target_units}
  do
    log "Processing ${target_unit}" 
    log_and_run ${obj_dir}/${tool_name} -f ${target_unit} "$@" || status=1
  done
  return ${status}
}

log_start
log_invocation "$@"

build_asis_tool    || exit $?
process_units "$@" || exit $?

log_end


