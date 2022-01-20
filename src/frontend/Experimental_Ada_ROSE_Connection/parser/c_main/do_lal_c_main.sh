#!/bin/bash
# Builds a C parser_adapter demonstration program, and runs it on some test
# code.
#
# If there are file name parameters, uses those in ${target_dir}, otherwise 
# uses all in ${target_dir}
#
# Parameters:
# -d  Turns on debug
# [file name(s)] 
#     Test code file(s)

# Prevents some informative error messages, so is usually commented out:
#set -o errexit
#trap "__exit_status=$?; echo Error - exiting do_asis_tool_2.sh with status ${__exit_status}; exit ${__exit_status}" ERR

# This script is in the base directory of this build:
rel_base_dir=`dirname $0`
base_dir=`(cd ${rel_base_dir}; pwd)`
current_dir=`pwd`
# Defines log, log_and_run, etc.:
source ${base_dir}/../utility_functions.sh

obj_dir=${base_dir}/obj

gprbuild_path=`which gprbuild` || exit -1
gnat_bin=`dirname ${gprbuild_path}`
gnat_home=`dirname ${gnat_bin}`

# The base dir is at [repo base]/src/frontend/Experimental_Ada_ROSE_Connection/parser/ada_main:
repo_base_dir=`(cd ${base_dir}/../../../../..; pwd)`
test_base_dir="${repo_base_dir}/tests/nonsmoke/functional/CompileTests/experimental_ada_tests"
tests_dir="${test_base_dir}/tests"
reference_dot_file_dir="${tests_dir}/reference/parser_adapter/lal_dot_graphs"

target_dir="${tests_dir}"
output_dir="${reference_dot_file_dir}"
target_project="default.gpr"
#tool_name="main_2"
#tool_project="lal_test.gpr"
tool_name="run_lal_adapter"
tool_project="lal_c_main.gpr"

if [[ ${#*} -ge 1 ]] 
then
  target_units="$@"
else
  target_units=`(cd ${target_dir}; ls *.ad[bs])`
fi

gcc_home="${gnat_home}"
# Override the default gcc if needed (e.g. when GNAT gcc is not wanted):
# For Charles on LC:
# gcc_home=/usr/tce/packages/gcc/gcc-8.1.0
# For Dan on NFS:
# gcc_home=/nfs/casc/overture/ROSE/opt/rhel7/x86_64/gcc/4.8.4/mpc/1.0/mpfr/3.1.2/gmp/5.1.2
export CC=${gcc_home}/bin/gcc

show_compiler_version () {
  log_separator_1
  log "Compiler version:"
  ${CC} --version || exit -1
}

build_tool () {
  log_separator_1
  log "Building ${tool_name}"
  # -p       Create missing obj, lib and exec dirs
  # -P proj  Use Project File proj
  # -v       Verbose output
  # -vPx     Specify verbosity when parsing Project Files (x = 0/1/2)
  # -ws      Ignore builder warnings
  # -Xnm=val Specify an external reference for Project Files
  log_then_run gprbuild \
  -p \
  -P ${base_dir}/${tool_project} \
  ${tool_name} || exit $?
}

build_asis_tool_cc () {
  log_separator_1
  log "Building ${tool_name}"
  asis_adapter_home=${base_dir}/../asis_adapter
  asis_adapter_lib_dir=${asis_adapter_home}/lib
  asis_lib_dir=${gnat_home}/lib/asis/asislib
  gnat_lib_dir=${gnat_home}/lib/gcc/x86_64-pc-linux-gnu/6.3.1/adalib
  if [ ! -d ${obj_dir} ]; then
    mkdir ${obj_dir} || exit $?
  fi
  cd ${obj_dir}

  log_then_run ${asis_adapter_home}/do_build_library.sh || exit $?

  log_then_run ${CC} -c -x c -MMD -MF ${tool_name}.d \
  -I${asis_adapter_home}/include \
  ${base_dir}/source/${tool_name}.c || exit $?
#/collab/usr/global/tools/rose/toss_3_x86_64_ib/GNAT/2019/lib/asis/asislib

  log_then_run ${CC} \
  ${tool_name}.o \
  ${asis_adapter_lib_dir}/libasis_adapter.a \
  -lpthread \
  -lrt \
  -ldl \
  -L
  -o ${tool_name} || exit $?
}

# Keeps going.  Returns 1 if any failed, 0 if all succeeded:
process_units () {
  status=0  
  cd ${current_dir}
  log_separator_1
  log "Processing specified files in ${target_dir} with ${tool_name}."
  log "Writing dot files to ${output_dir}."
  for target_unit in ${target_units}
  do
    log "Processing ${target_unit}" 
    # Parameters:
    # Project file
    # Input file
    # Output directory
    log_then_run ${obj_dir}/${tool_name} \
       ${target_dir}/${target_project} \
       ${target_dir}/${target_unit} \
       ${output_dir} \
       || status=1
  done
  return ${status}
}

log_start
log_invocation "$@"

show_compiler_version
build_tool
process_units || exit $?

log_end

