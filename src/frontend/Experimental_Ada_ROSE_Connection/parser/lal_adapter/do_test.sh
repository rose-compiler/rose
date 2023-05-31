#!/bin/bash
# Builds an Ada parser_adapter demonstration program, and runs it on some test 
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
test_dir="${test_base_dir}/tests"

target_dir="${test_dir}"
target_project="default.gpr"

if [[ ${#*} -ge 1 ]] 
then
  target_units="$@"
else
  target_units=`(cd ${target_dir}; ls *.ad[bs])`
fi

tool_name="main"
tool_project_file="lal_test.gpr"

show_compiler_version () {
  log_separator_1
  log "Compiler version:"
  gnat --version || exit -1
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
  -P ${base_dir}/${tool_project_file} \
  -ws \
  -XLIBRARY_TYPE=static \
  ${tool_name} || exit $?
}

# Keeps going.  Returns 1 if any failed, 0 if all succeeded:
process_units () {
  status=0  
  cd ${current_dir}
  log_separator_1
  log "Processing specified files in ${target_dir} with ${tool_name}."
  for target_unit in ${target_units}
  do
    log "Processing ${target_unit}"
    # Parameters:
    # Target project file
    # target file(s)
    log_then_run ${obj_dir}/${tool_name} \
       ${target_dir}/${target_project} \
       ${target_dir}/${target_unit} \
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

