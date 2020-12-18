#!/bin/bash
# Runs the ROSE tool we are interested in right now.  For use by
# /g/g17/charles/code/ROSE/ROSE-18/kull/systemconf/compilers/Linux_identityTranslator_0_9_9_238_gnu_4_9_3_mvapich2_2_2_compiler.py
# and similar.
# Runs it through do_pass_fail, which records whether the tool succeeded, 
# but always returns status 0.

# When compiling version.cc, -DMACRONAME arguments will be quoted strings. 
# Instead of trying to quote the striings properly, just call the Kull mpicxx!
# (Should happen automatically. See above.)

# Sets strict, defines log, log_then_run, unset_script etc.:	
source `which utility_functions.sh`
log_start

this_dir=`dirname $0`

source ${this_dir}/set_ROSE_HOME
export ROSE_TOOL_PATH=${ROSE_HOME}/bin/identityTranslator
# For ROSE installations by Justin:
#export ROSE_TOOL_PATH=${ROSE_HOME}/tutorial/identityTranslator

# Needed to run a ROSE Tool:
# (Not needed if libtool did it's thing correctly):
# LD_LIBRARY_PATH=${ROSE_LD_LIBRARY_PATH}:${LD_LIBRARY_PATH}

CXX=/usr/tce/packages/gcc/gcc-4.9.3/bin/g++
## Fix \"this\ that\", etc:
#quoted_parms=`fix_escapes_in_each_parm "$@"`
#commandline="${ROSE_TOOL_PATH} ${quoted_parms}"
commandline="${ROSE_TOOL_PATH} $*"
#source ~/environment/setup_debug_aliases_bash
#echo_var commandline


# might fail. keep going:
unset_strict
log_then_run ${commandline}
status=$?
if [[ ${status} -eq 0 ]]; then
  log PASSED
  echo "$commandline" >> ${this_dir}/passed.txt
else
  log FAILED
  echo "$commandline (status=${status})" >> ${this_dir}/failed.txt
  log_then_run ${CXX} $@
  status=$?
  if [[ ${status}  -ne 0 ]]; then
  log ERROR_RUNNING_CXX
  fi
fi

log_end

