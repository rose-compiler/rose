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
#test_dir="${test_base_dir}/tests"
test_dir="${test_base_dir}/dot_asis_tests/test_units"
reference_dot_file_dir="${test_base_dir}/dot_asis_tests/referecing_dot_output"

target_dir="${test_dir}"
output_dir="${reference_dot_file_dir}"

if [[ ${#*} -ge 1 ]] 
then
  target_units="$@"
else
  target_units=`(cd ${target_dir}; ls *.ad[bs])`
fi

tool_name=run_parser_adapter

show_compiler_version () {
  log_separator_1
  log "Compiler version:"
  gnat --version || exit -1
}

build_asis_tool () {
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
  -P ${base_dir}/dot_asis.gpr \
  -ws \
  -XLIBRARY_TYPE=static \
  -XASIS_BUILD=static \
  ${tool_name} || exit $?
}

# Keeps going.  Returns 1 if any failed, 0 if all succeeded:
process_units () {
  status=0  
  log_separator_1
  log "Processing specified files in ${target_dir} with ${tool_name}."
  log "Writing dot files to ${output_dir}."
  for target_unit in ${target_units}
  do
    log "Processing ${target_unit}"
    # -d, --debug - turn on debug
    # -f, --file - Input file name (required)
    # -g, --gnat_home - GNAT home directory (required)
    # -i, --process_implementation_units - Process implementation-specific library units
    # -p, --process_predefined_units     - Process Ada predefined language environment units
    # -o, --output_dir - Output directory (optional)
    log_then_run ${obj_dir}/${tool_name} \
   \
       --file=${target_dir}/${target_unit} \
       --gnat_home=${gnat_home} \
       --output_dir=${output_dir} \
       "$@" || status=1
  done
  return ${status}
}

log_start
log_invocation "$@"

show_compiler_version
build_asis_tool
process_units "$@" || exit $?

log_end

