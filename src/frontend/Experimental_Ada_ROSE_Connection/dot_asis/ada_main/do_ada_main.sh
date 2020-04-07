#!/bin/bash
# Builds a dot_asis tool with an Ada main program, and runs it on some test code.
#
# Parameters:
# -d  Turns on debug

# Prevents some informative error messages, so is usually commented out:
#set -o errexit
#trap "__exit_status=$?; echo Error - exiting do_asis_tool_2.sh with status ${__exit_status}; exit ${__exit_status}" ERR

# This script is in the base directory of this build:
rel_base_dir=`dirname $0`
base_dir=`(cd ${rel_base_dir}; pwd)`

# Defines log, log_and_run, etc.:
source ${base_dir}/../utility_functions.sh

gprbuild_path=`which gprbuild` || exit -1
gnat_bin=`dirname ${gprbuild_path}`
gnat_home=`dirname ${gnat_bin}`

obj_dir=${base_dir}/obj

tool_name=run_asis_tool_2
target_dir=${base_dir}/../test_units
#target_units="minimal.adb"
#target_units="unit_2.ads"
#target_units="unit_2.adb"
#target_units="variable_declaration.ads"
#target_units="if_statement.adb"
#target_units="ordinary_type_declaration.ads"
target_units=`(cd ${target_dir}; ls *.ad[bs])`

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
  # -Xnm=val Specify an external reference for Project Files
  log_then_run gprbuild \
  -p \
  -P ${base_dir}/dot_asis.gpr \
  -XLIBRARY_TYPE=static \
  -XASIS_BUILD=static \
  ${tool_name} || exit $?
}

# Keeps going.  Returns 1 if any failed, 0 if all succeeded:
process_units () {
  status=0  
  log_separator_1
  log "Processing specified files in ${target_dir} with ${tool_name}"
  for target_unit in ${target_units}
  do
    log "Processing ${target_unit}"
    # -f, --file - Input file name (required)
    # -g, --gnat_home - GNAT home directory (required)
    # -o, --output_dir - Output directory (optional)
    log_then_run ${obj_dir}/${tool_name} \
       --file=${target_dir}/${target_unit} \
       --gnat_home=${gnat_home} \
       --output_dir=`pwd` \
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

