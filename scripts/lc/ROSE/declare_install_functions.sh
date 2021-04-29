#!/bin/echo "ERROR: Do not execute this script! Source it from bash:"
# This script defines functions for use by ROSE installation scripts.
# DEPENDENCIES: 
#   ../bin/utility_functions.sh
#   (Also see dependencies for set_main_vars, below.)

# Defines:
#   (also see ../bin/utility_functions.sh)
#   ROSE_ROSE_SCRIPT_DIR
#   ROSE_BIN_SCRIPT_DIR
#   RUN_AND_LOG
#   SRUN_DO
#   _temp
#   build_boost_rose
#   clone_latest_workspace
#   do_gcc_configure
#   _do_gcc_configure_common
#   do_gcc_configure_with_ada
#   do_gcc_configure_with_binary_analysis
#   do_gcc_configure_with_c_cxx
#   do_gcc_configure_with_cxx11
#   do_gcc_configure_with_fortran
#   do_gcc_configure_with_profiling
#   do_intel_configure
#   do_preconfigure
#   do_preconfigure_no_latest_copy
#   echo_version_from_file
#   make_and_install
#   make_docs
#   print_rose_vars
#   _set_ROSE_BUILD_PATH_and_ROSE_INSTALL_PATH
#   _set_ROSE_COMPILER_VERSIONED_STD
#   set_ROSE_HOME_ROSE_LD_LIBRARY_PATH
#   set_ROSE_LATEST_INSTALL_VERSION
#   set_ROSE_LATEST_WORKSPACE_VERSION
#   _set_ROSE_REPO_NAME_from_BRANCH_and_VERSION
#   set_main_vars
#   setup_boost
#   setup_boost_rose
#   setup_boost_tce
#   setup_gcc_compiler
#   setup_gcc_compiler_base
#   setup_gcc_compiler_with_ada
#   setup_gcc_compiler_with_binary_analysis
#   setup_gcc_compiler_with_c_cxx
#   setup_gcc_compiler_with_fortran
#   setup_gcc_compiler_with_profiling
#   setup_intel_18_0_2_compiler_non_mpi
#   setup_intel_19_0_4_compiler
#   setup_intel_compiler
#   _setup_intel_compiler_common
#   _setup_jvm_library_path
#   _turn_on_module
#   use_existing_workspace
#   use_latest_existing_install
#   use_latest_existing_workspace
#   use_latest_gcc_rose
#   use_latest_gcc_rose_with_binary_analysis
#   use_latest_gcc_rose_with_c_cxx
#   use_latest_gcc_rose_with_fortran
#   use_latest_gcc_rose_with_profiling
#   use_latest_intel_19_0_4_rose
#   use_latest_intel_rose
#   _use_latest_rose_begin
#   _use_rose_end
#   use_script_repo_branch
#   use_specific_intel_rose
#   _use_specific_rose_begin

#
# Example users:
#  clone_latest:
#   set_main_vars
#   print_rose_vars
#   clone_latest_workspace
#
#  clone_and_preconfigure.sh:
#   set_main_vars
#   print_rose_vars
#   clone_latest_workspace
#   do_preconfigure
# 
#  install_with_gcc.sh:
#   set_main_vars
#   use_latest_existing_workspace
#   setup_gcc_compiler
#   use_tce_boost
#   print_rose_vars
#   do_gcc_configure
#   make_and_install
# 
#  install_with_intel.sh:
#   set_main_vars
#   use_latest_existing_workspace
#   setup_intel_compiler
#   use_tce_boost
#   print_rose_vars
#   do_intel_configure
#   make_and_install

# Optional lines for client scripts:
# Don't actually run:
#   export RUN_OR_NOT_EFFORT_ONLY=TRUE
#
# For manual testing:
#   export ROSE_REPO_BASE="${HOME}/code/ROSE"
#   export ROSE_BUILD_BASE="${HOME}/code/ROSE"

# Variable naming:
#  - Variables in lower case are for internal use in and between functions 
#  defined here.  They may change without notice, and scripts outside this file
#  should not depend on them.
#  - Variables starting with "_" are not used outside the function in which 
#  they are set.

# Find ourselves:
# (Sourcers of this script should be in [repo]/scripts/lc/ROSE)
_rel_enclosing_dir=`dirname ${BASH_SOURCE[0]}`
export ROSE_SCRIPT_REPO_DIR=`(cd ${_rel_enclosing_dir}/../../..; pwd)`
export ROSE_LC_SCRIPT_DIR="${ROSE_SCRIPT_REPO_DIR}/scripts/lc"
export ROSE_BIN_SCRIPT_DIR="${ROSE_LC_SCRIPT_DIR}/bin"
export ROSE_COMPDB_SCRIPT_DIR="${ROSE_LC_SCRIPT_DIR}/compdb"
export ROSE_ROSE_SCRIPT_DIR="${ROSE_LC_SCRIPT_DIR}/ROSE"

# Sets strict mode, defines set_strict, unset_strict, log_*,
# log_then_run, run_or_not, echo_maybe_not_set_var, etc.:
source ${ROSE_BIN_SCRIPT_DIR}/utility_functions.sh

export RUN_AND_LOG="${ROSE_BIN_SCRIPT_DIR}/run_and_log"
export SRUN_DO="${ROSE_BIN_SCRIPT_DIR}/srun_do"
export MAX_PROCS="36"

_turn_on_module () {
  ###############################################################################
  # This is  is an excerpt from /g/g17/charles/.profile.toss_3_x86_64_ib:
  ###############################################################################
  # Run this again or else "module" doesn't work (might have been run by 
  #   /etc/profile -> /etc/profile.d/z00_lmod.sh but ENVIRONMENT was BATCH then):  
  #
  # Turn off unset variable detection because z00_lmod.sh reads BASH_ENV, which 
  # is not set when .profile is run for the first time by .bashrc, e.g. when
  # connecting to quartz with x-win32 using "/usr/bin/xterm -ls":
  push_set_state
  set +u
  source /etc/profile.d/z00_lmod.sh
  # Turn unset variable detection back on:
  pop_set_state
  # ... (unused excerpt omitted)
  module use /usr/tce/modulefiles/Core
  module use /usr/tce/modulefiles/Compiler
  module use /usr/tce/modulefiles/MPI
  ###############################################################################
  # End excerpt
  ###############################################################################
}

print_rose_vars () {
  log_separator_1
  log "ROSE environment variables:"
  log_var COMMON_BUILD_BASE
  log_var COMMON_INSTALL_BASE
  log_var CC
  log_var CXX
  log_var LD_LIBRARY_PATH
  log_var PATH
  for line in `env | grep '^ROSE' | sort`
  do
    log $line
  done
  log_separator_1
}

# Call before calling set_main_vars:
# Sets:
#   ROSE_BRANCH_NAME_GIT
use_script_repo_branch () {
  export ROSE_BRANCH_NAME_GIT=`git branch --no-color | grep '* ' | tr -d ' *'`
}

# Uses these if set, else sets them:
#   COMMON_BUILD_BASE
#   COMMON_INSTALL_BASE
#   ROSE_BUILD_BASE
#   ROSE_BRANCH_NAME_GIT
#   ROSE_INSTALL_BASE
#   ROSE_REPO_BASE
# Sets:
#   rose_latest_workspace_version_file
#   rose_latest_install_version_file
#   rose_remote_repo
#   rose_repo_path_cloned
#   ROSE_BRANCH
#   ROSE_MPI_KIND
#   ROSE_MPI_VERSION
#   ROSE_BOOST_VERSION
#   ROSE_DO_UPDATE_VERSIONS
set_main_vars () {
  export COMMON_REPO_BASE=${COMMON_REPO_BASE:-"${HOME}/code"}
  export COMMON_BUILD_BASE=${COMMON_BUILD_BASE:-"${HOME}/code"}
  export COMMON_INSTALL_BASE=${COMMON_INSTALL_BASE:-"/collab/usr/global/tools"}
  export ROSE_REPO_BASE=${ROSE_REPO_BASE:-"${COMMON_REPO_BASE}/ROSE"}
  export ROSE_BUILD_BASE=${ROSE_BUILD_BASE:-"${COMMON_BUILD_BASE}/ROSE"}
  export ROSE_INSTALL_BASE=${ROSE_INSTALL_BASE:-"${COMMON_INSTALL_BASE}/rose/${SYS_TYPE}"}
  # The Git branch name may include "/":
  # e.g. "reynolds12/automation"
  export ROSE_BRANCH_NAME_GIT=${ROSE_BRANCH_NAME_GIT:-"master"}

  # Take out any slashes:
  # e.g. "dquinlan/dq-development-dev" -> "dquinlan-dq-development-dev"
  export ROSE_BRANCH=$(echo "${ROSE_BRANCH_NAME_GIT}" | tr '/' '-')
  
  export rose_latest_workspace_version_file="${ROSE_REPO_BASE}/VERSION_OF_LATEST_WORKSPACE"
  set_ROSE_LATEST_WORKSPACE_VERSION  
  export rose_latest_install_version_file="${ROSE_INSTALL_BASE}/VERSION_OF_LATEST_INSTALL"
  # Used by test_this_build.sh and others:
  set_ROSE_LATEST_INSTALL_VERSION
  export ROSE_DO_UPDATE_VERSIONS="TRUE"
  
  # The LLNL ROSE git project and repo:
#  _rose_remote_project="rose-dev@rosecompiler1.llnl.gov:rose/scratch"
  _rose_remote_project="ssh://git@rosecompiler2.llnl.gov:10022/rose-compiler"

  # e.g. "rose-dev@rosecompiler1.llnl.gov:rose/scratch/rose":
  export rose_remote_repo="${_rose_remote_project}/rose"
  # e.g. "${HOME}/code/ROSE/rose":
  export rose_repo_path_cloned="${ROSE_REPO_BASE}/rose"
  
  # Non-compiler version info:
  export ROSE_MPI_KIND="mvapich2"
  export ROSE_MPI_VERSION="2.2"
  export ROSE_BOOST_VERSION="1.66.0"
}

# Uses:
#   ROSE_BRANCH
#   ROSE_REPO_BASE
#   ROSE_VERSION
# Sets:
#   ROSE_REPO_NAME
#   ROSE_REPO_PATH
_set_ROSE_REPO_NAME_from_BRANCH_and_VERSION () {
  export ROSE_REPO_NAME="rose-${ROSE_BRANCH}-${ROSE_VERSION}"
  export ROSE_REPO_PATH="${ROSE_REPO_BASE}/${ROSE_REPO_NAME}"
}

# Uses:
#   ROSE_SCRIPT_REPO_DIR
# Sets:
#   ROSE_REPO_NAME
#   ROSE_REPO_PATH
_set_ROSE_REPO_NAME_from_workspace_name () {
  export ROSE_REPO_NAME=`basename ${ROSE_SCRIPT_REPO_DIR}`
  export ROSE_REPO_PATH="${ROSE_REPO_BASE}/${ROSE_REPO_NAME}"
  export ROSE_DO_UPDATE_VERSIONS="FALSE"
}

# One parameter - file name:
echo_version_from_file () {
  if [ -f $1 ]
  then
    cat $1
  else
    # Doesn't seem to work:
#    echo "ERROR: echo_version_from_file could not find file \"$1\"" 1>&2
#    exit 1
    echo "unknown_version"
  fi
}

# Uses:
#   rose_latest_install_version_file
set_ROSE_LATEST_INSTALL_VERSION () {
  export ROSE_LATEST_INSTALL_VERSION=`echo_version_from_file ${rose_latest_install_version_file}`
}

# Uses:
#   rose_latest_workspace_version_file
set_ROSE_LATEST_WORKSPACE_VERSION () {
  export ROSE_LATEST_WORKSPACE_VERSION=`echo_version_from_file ${rose_latest_workspace_version_file}`
}

# Uses:
#   ROSE_BRANCH
#   rose_repo_path_cloned
#   rose_remote_repo
#   ROSE_BRANCH_NAME_GIT
#   ROSE_REPO_BASE
# Sets:
#   ROSE_REPO_NAME
#   ROSE_REPO_PATH
#   ROSE_VERSION
clone_latest_workspace () {
  cd ${ROSE_BUILD_BASE}
#  run_or_not git clone ${rose_remote_repo}
  run_or_not scp rose-mgr@hudson-rose-45:/export/tmp.rose-mgr/rose-tar/merging/rose.tar ./
  run_or_not tar -xf rose.tar
  run_or_not cd ${rose_repo_path_cloned}
  # Check out the desired branch as needed:
  run_or_not git checkout ${ROSE_BRANCH_NAME_GIT}
  run_or_not git submodule init
  run_or_not git submodule update
  export ROSE_VERSION=`echo_version_from_file ROSE_VERSION`
  _set_ROSE_REPO_NAME_from_BRANCH_and_VERSION
  run_or_not mv ${rose_repo_path_cloned} ${ROSE_REPO_PATH}
}

# Uses:
#   One parameter - the ROSE version
#   ROSE_BRANCH
# Sets:
#   ROSE_REPO_NAME
#   ROSE_REPO_PATH
#   ROSE_VERSION
use_existing_workspace () {
  export ROSE_VERSION=$1
  _set_ROSE_REPO_NAME_from_BRANCH_and_VERSION
}

# Uses:
#   ROSE_BRANCH
#   ROSE_LATEST_WORKSPACE_VERSION
# Sets:
#   ROSE_REPO_NAME
#   ROSE_REPO_PATH
#   ROSE_VERSION
use_latest_existing_workspace () {
  use_existing_workspace ${ROSE_LATEST_WORKSPACE_VERSION}
}

# Uses:
#   ROSE_SCRIPT_REPO_DIR
# Sets:
#   ROSE_REPO_NAME
#   ROSE_REPO_PATH
use_this_workspace () {
  _set_ROSE_REPO_NAME_from_workspace_name
}

#========================
# For test_this_build.sh:
#========================

# Uses:
#   ROSE_BRANCH
#   ROSE_LATEST_INSTALL_VERSION
#   ROSE_REPO_BASE
# Sets:
#   ROSE_REPO_NAME
#   ROSE_REPO_PATH
#   ROSE_VERSION
use_latest_existing_install () {
  use_existing_workspace ${ROSE_LATEST_INSTALL_VERSION}
}

# Parameter:
#   Rose version
# Sets:
#   COMMON_BUILD_BASE
#   ROSE_INSTALL_BASE
#   ROSE_LATEST_INSTALL_VERSION
#   ROSE_REPO_NAME
#   ROSE_REPO_PATH
#   ROSE_VERSION
#   (See set_main_vars for more)
_use_specific_rose_begin () {
  set_main_vars
  use_existing_workspace $1
}

# Sets:
#   COMMON_BUILD_BASE
#   ROSE_INSTALL_BASE
#   ROSE_LATEST_INSTALL_VERSION
#   ROSE_REPO_NAME
#   ROSE_REPO_PATH
#   ROSE_VERSION
#   (See set_main_vars for more)
_use_latest_rose_begin () {
  set_main_vars
  use_latest_existing_install
}

# Uses:
#   ROSE_COMPDB_SCRIPT_DIR
#   ROSE_COMPILER_SUFFIX
# Sets:
#   COMP_DB_MAP
#   RENDER_TEXT
#   ROSE_HOME
#   ROSE_BUILD_PATH
#   ROSE_INSTALL_PATH
#   ROSE_LD_LIBRARY_PATH
#   ROSE_TOOL
_use_rose_end () {
  _set_ROSE_BUILD_PATH_and_ROSE_INSTALL_PATH
  export ROSE_HOME=${ROSE_INSTALL_PATH}
  export ROSE_LD_LIBRARY_PATH=\
"${ROSE_HOME}/src/.libs":\
"${ROSE_HOME}/src/3rdPartyLibraries/libharu-2.1.0/src/.libs"
  # Used by test_this_build.sh:
  export COMP_DB_MAP="${ROSE_COMPDB_SCRIPT_DIR}/comp_db_map.py"
  export RENDER_TEXT="${ROSE_COMPDB_SCRIPT_DIR}/render_text.py"
  export ROSE_TOOL="${ROSE_HOME}/bin/identityTranslator"
}

# All "use_latest..." below 
# Uses:
#   ROSE_COMPDB_SCRIPT_DIR
# Sets:
#   COMMON_BUILD_BASE
#   COMP_DB_MAP
#   RENDER_TEXT
#   ROSE_BACKEND_CC
#   ROSE_BACKEND_COMPILER_HOME
#   ROSE_BACKEND_CXX
#   ROSE_HOME
#   ROSE_REPO_NAME
#   ROSE_LATEST_INSTALL_VERSION
#   ROSE_LD_LIBRARY_PATH
#   ROSE_TOOL
#   (See set_main_vars for more)
# See set_main_vars for overridable vars.

# Has C/C++, Fortran, and Binary Analysis enabled:
use_latest_gcc_rose () {
  _use_latest_rose_begin
  setup_gcc_compiler
 _use_rose_end
}

# Just has C/C++ enabled:
use_latest_gcc_rose_with_c_cxx () {
  _use_latest_rose_begin
  setup_gcc_compiler_with_c_cxx
 _use_rose_end
}

# Has C/C++ and Binary Analysis enabled:
use_latest_gcc_rose_with_binary_analysis () {
  _use_latest_rose_begin
  setup_gcc_compiler_with_binary_analysis
 _use_rose_end
}

# Has C/C++ and Fortran enabled:
use_latest_gcc_rose_with_fortran () {
  _use_latest_rose_begin
  setup_gcc_compiler_with_fortran
 _use_rose_end
}

# Has C/C++ and -gp:
use_latest_gcc_rose_with_profiling () {
  _use_latest_rose_begin
  setup_gcc_compiler_with_profiling
 _use_rose_end
}

use_latest_intel_rose () {
  _use_latest_rose_begin
  setup_intel_compiler
 _use_rose_end
}

# Parmater:
#   Rose version
# Uses the Intel compiler from the version of ROSE specified:
use_specific_intel_rose () {
  _use_specific_rose_begin $1
  setup_intel_compiler
 _use_rose_end
}

# Use latest versioned ROSE built with Inte 19.0.4 (for ARES 2020-01-10):
use_latest_intel_19_0_4_rose () {
  _use_latest_rose_begin
  setup_intel_19_0_4_compiler
 _use_rose_end
}



#========================

# Uses:
#   rose_latest_workspace_version_file
#   ROSE_DO_UPDATE_VERSIONS
#   ROSE_REPO_PATH
_update_latest_workspace_version () {
  if [ ${ROSE_DO_UPDATE_VERSIONS} == "TRUE" ] 
  then
    run_or_not cp ${ROSE_REPO_PATH}/ROSE_VERSION ${rose_latest_workspace_version_file}
  fi
}

# Uses:
#   rose_latest_workspace_version_file
#   ROSE_DO_UPDATE_VERSIONS
#   ROSE_REPO_PATH
do_preconfigure () {
  run_or_not cd ${ROSE_REPO_PATH}
  run_or_not ${SRUN_DO} ${RUN_AND_LOG} ./build
  _update_latest_workspace_version
}

#================================================
# FOR all C++ compilers
#================================================
# Uses:
#   ROSE_COMPILER
#   ROSE_COMPILER_VERSION
# Sets:
#   ROSE_COMPILER_CXX_STD
#   ROSE_COMPILER_CXX_STD_FLAG
#   ROSE_COMPILER_VERSIONED
#   ROSE_COMPILER_VERSIONED_STD
_set_ROSE_COMPILER_VERSIONED_STD () {
  export ROSE_COMPILER_CXX_STD="c++11"
  export ROSE_COMPILER_CXX_STD_FLAG="-std=${ROSE_COMPILER_CXX_STD}"
  export ROSE_COMPILER_VERSIONED="${ROSE_COMPILER}-${ROSE_COMPILER_VERSION}"
  export ROSE_COMPILER_VERSIONED_STD="${ROSE_COMPILER_VERSIONED}-${ROSE_COMPILER_CXX_STD}"
}

#================================================
# FOR INTEL 18.0.1 MPI (Used by ARES 2019-03-14):
#================================================
# Uses:
#   ROSE_BUILD_BASE
#   ROSE_INSTALL_BASE
#   ROSE_REPO_NAME
# Sets:
#   CC
#   CXX
#   ROSE_BACKEND_CC
#   ROSE_BACKEND_COMPILER_HOME
#   ROSE_BACKEND_COMPILER_VERSION
#   ROSE_BACKEND_CXX
#   ROSE_BUILD_PATH
#   ROSE_COMPILER
#   ROSE_COMPILER_CXX_STD
#   ROSE_COMPILER_CXX_STD_FLAG
#   ROSE_COMPILER_HOME
#   ROSE_COMPILER_VERSION
#   ROSE_COMPILER_VERSIONED
#   ROSE_COMPILER_VERSIONED_STD
#   ROSE_INSTALL_PATH
#   ROSE_INTEL_GCC_COMPILER_VERSION
setup_intel_compiler () {
  export ROSE_COMPILER_VERSION="18.0.1"
  export ROSE_BACKEND_COMPILER_VERSION="18.0.1"
  export ROSE_INTEL_GCC_COMPILER_VERSION="6.1.0"
  _setup_intel_compiler_common
}
#================================================

#================================================
# FOR INTEL 19.0.4 MPI (Used by ARES 2020-01-20):
#================================================
# Uses:
#   ROSE_BUILD_BASE
#   ROSE_INSTALL_BASE
#   ROSE_REPO_NAME
# Sets:
#   CC
#   CXX
#   ROSE_BACKEND_CC
#   ROSE_BACKEND_COMPILER_HOME
#   ROSE_BACKEND_COMPILER_VERSION
#   ROSE_BACKEND_CXX
#   ROSE_BOOST_VERSION
#   ROSE_BUILD_PATH
#   ROSE_COMPILER
#   ROSE_COMPILER_CXX_STD
#   ROSE_COMPILER_CXX_STD_FLAG
#   ROSE_COMPILER_HOME
#   ROSE_COMPILER_VERSION
#   ROSE_COMPILER_VERSIONED
#   ROSE_COMPILER_VERSIONED_STD
#   ROSE_INSTALL_PATH
#   ROSE_INTEL_GCC_COMPILER_VERSION
setup_intel_19_0_4_compiler () {
  export ROSE_COMPILER_VERSION="19.0.4"
  export ROSE_BACKEND_COMPILER_VERSION="19.0.4"
  export ROSE_INTEL_GCC_COMPILER_VERSION="8.1.0"
  # Needs later MPI:
  export ROSE_MPI_VERSION="2.3" 
  # Uses ROSE_MPI_VERSION:
  _setup_intel_compiler_common
  # Needs later BOOST:
  export ROSE_BOOST_VERSION="1.69.0"
}
#================================================


#==============================
# FOR ALL INTEL MPI (internal):
#==============================
# Uses:
#   ROSE_BACKEND_COMPILER_VERSION
#   ROSE_BUILD_BASE
#   ROSE_COMPILER_VERSION
#   ROSE_INSTALL_BASE
#   ROSE_INTEL_GCC_COMPILER_VERSION
#   ROSE_MPI_VERSION
#   ROSE_REPO_NAME
# Sets:
#   CC
#   CXX
#   ROSE_BACKEND_CC
#   ROSE_BACKEND_COMPILER
#   ROSE_BACKEND_COMPILER_HOME
#   ROSE_BACKEND_CXX
#   ROSE_BUILD_PATH
#   ROSE_COMPILER
#   ROSE_COMPILER_CXX_STD
#   ROSE_COMPILER_CXX_STD_FLAG
#   ROSE_COMPILER_HOME
#   ROSE_COMPILER_VERSIONED
#   ROSE_COMPILER_VERSIONED_STD
#   ROSE_INSTALL_PATH
#   ROSE_NON_MPI_COMPILER_HOME
#   COMPILERVARS_ARCHITECTURE
#   COMPILERVARS_PLATFORM
_setup_intel_compiler_common () {
  export ROSE_COMPILER="intel"
  # Intel compiler version should have been set by our caller:
  _set_ROSE_COMPILER_VERSIONED_STD
  export ROSE_COMPILER_HOME="/usr/tce/packages/${ROSE_MPI_KIND}/${ROSE_MPI_KIND}-${ROSE_MPI_VERSION}-${ROSE_COMPILER_VERSIONED}"
  export  CC="${ROSE_COMPILER_HOME}/bin/mpicc"
  export CXX="${ROSE_COMPILER_HOME}/bin/mpic++"

  export ROSE_BACKEND_COMPILER="intel"
  _rose_backend_compiler_versioned="${ROSE_BACKEND_COMPILER}-${ROSE_BACKEND_COMPILER_VERSION}"
  export ROSE_BACKEND_COMPILER_HOME="/usr/tce/packages/${ROSE_MPI_KIND}/${ROSE_MPI_KIND}-${ROSE_MPI_VERSION}-${_rose_backend_compiler_versioned}"
  export  ROSE_BACKEND_CC="${ROSE_BACKEND_COMPILER_HOME}/bin/mpicc"
  export ROSE_BACKEND_CXX="${ROSE_BACKEND_COMPILER_HOME}/bin/mpic++"

  #---------------------------------------
  # Run the Intel setup script so we don't have to add 
  # "-L/usr/tce/packages/intel/intel-18.0.1/lib/intel64" to line 275 of 
  # "${ROSE_REPO_PATH}/config/Makefile.for.ROSE.includes.and.libs" to avoid
  # link problems:
  export ROSE_NON_MPI_COMPILER_HOME="/usr/tce/packages/${ROSE_COMPILER}/${ROSE_COMPILER_VERSIONED}"
  export COMPILERVARS_ARCHITECTURE=intel64
  export COMPILERVARS_PLATFORM=linux
  push_set_state
  set +u
  source ${ROSE_NON_MPI_COMPILER_HOME}/bin/compilervars.sh
  # Yes, you need gcc 4.9.3 or later for intel:
  _turn_on_module
  module load gcc/${ROSE_INTEL_GCC_COMPILER_VERSION}
  pop_set_state
  #---------------------------------------
  ROSE_COMPILER_SUFFIX="${ROSE_COMPILER_VERSIONED}.mpi-${_rose_backend_compiler_versioned}.mpi"
}
#=====================================

#=====================================
# FOR INTEL 18.0.2 non-MPI (Used by BLAST):
#=====================================
# Uses:
#   ROSE_BUILD_BASE
#   ROSE_INSTALL_BASE
#   ROSE_REPO_NAME
# Sets:
#   CC
#   CXX
#   ROSE_BACKEND_CC
#   ROSE_BACKEND_COMPILER_HOME
#   ROSE_BACKEND_CXX
#   ROSE_BUILD_PATH
#   ROSE_COMPILER
#   ROSE_COMPILER_CXX_STD
#   ROSE_COMPILER_CXX_STD_FLAG
#   ROSE_COMPILER_HOME
#   ROSE_COMPILER_VERSION
#   ROSE_COMPILER_VERSIONED
#   ROSE_COMPILER_VERSIONED_STD
#   ROSE_INSTALL_PATH
setup_intel_18_0_2_compiler_non_mpi () {
  export ROSE_COMPILER="intel"
  export ROSE_COMPILER_VERSION="18.0.2"
  _set_ROSE_COMPILER_VERSIONED_STD
  export ROSE_COMPILER_HOME="/usr/tce/packages/${ROSE_COMPILER}/${ROSE_COMPILER_VERSIONED}"
  export  CC="${ROSE_COMPILER_HOME}/bin/icc"
  export CXX="${ROSE_COMPILER_HOME}/bin/icpc"

  export ROSE_BACKEND_COMPILER=${ROSE_COMPILER}
  _rose_backend_compiler_versioned=${ROSE_COMPILER_VERSIONED_STD}
  export ROSE_BACKEND_COMPILER_HOME=${ROSE_COMPILER_HOME}
  export  ROSE_BACKEND_CC="${ROSE_BACKEND_COMPILER_HOME}/bin/icc"
  export ROSE_BACKEND_CXX="${ROSE_BACKEND_COMPILER_HOME}/bin/icpc"

  #---------------------------------------
  # Run the Intel setup script so we don't have to add 
  # "-L/usr/tce/packages/intel/intel-18.0.1/lib/intel64" to line 275 of 
  # "${ROSE_REPO_PATH}/config/Makefile.for.ROSE.includes.and.libs" to avoid
  # link problems:
  export ROSE_NON_MPI_COMPILER_HOME="${ROSE_COMPILER_HOME}"
  export COMPILERVARS_ARCHITECTURE=intel64
  export COMPILERVARS_PLATFORM=linux
  # Temporarily turn off undefind variable checking:
  push_set_state
  set +u
  source ${ROSE_NON_MPI_COMPILER_HOME}/bin/compilervars.sh
  # Yes, you need gcc 4.9.3 or later for intel:
  _turn_on_module
  module load gcc/6.1.0
  pop_set_state
  #---------------------------------------

  ROSE_COMPILER_SUFFIX="${ROSE_COMPILER_VERSIONED}-${_rose_backend_compiler_versioned}"
}
#=====================================

#======================================
# FOR GCC 4.9.3 or 6.1.0 non-MPI (used by Kull):
#======================================
# Sets:
#   CC
#   CXX
#   ROSE_BACKEND_CC
#   ROSE_BACKEND_COMPILER_HOME
#   ROSE_BACKEND_CXX
#   ROSE_COMPILER
#   ROSE_COMPILER_CXX_STD
#   ROSE_COMPILER_CXX_STD_FLAG
#   ROSE_COMPILER_HOME
#   ROSE_COMPILER_VERSION
#   ROSE_COMPILER_VERSIONED
#   ROSE_COMPILER_VERSIONED_STD
setup_gcc_compiler_base () {
  export ROSE_COMPILER="gcc"
#  export ROSE_COMPILER_VERSION="6.1.0"
  export ROSE_COMPILER_VERSION="4.9.3"
  _set_ROSE_COMPILER_VERSIONED_STD
  export ROSE_COMPILER_HOME="/usr/tce/packages/${ROSE_COMPILER}/${ROSE_COMPILER_VERSIONED}"
  export  CC="${ROSE_COMPILER_HOME}/bin/cc"
  export CXX="${ROSE_COMPILER_HOME}/bin/c++"

  # ROSE_BACKEND_COMPILER_HOME is not used to build ROSE, but may be useful elsewhere e.g. run_rose.sh:
  export ROSE_BACKEND_COMPILER_HOME=${ROSE_COMPILER_HOME}
  export  ROSE_BACKEND_CC="${ROSE_BACKEND_COMPILER_HOME}/bin/cc"
  export ROSE_BACKEND_CXX="${ROSE_BACKEND_COMPILER_HOME}/bin/c++"
}

# Sets:
#   LD_LIBRARY_PATH
_setup_jvm_library_path () {
  # Old:
  #jvm_lib_path="/usr/lib/jvm/java-1.8.0-openjdk-1.8.0.232.b09-0.el7_7.x86_64/jre/lib/amd64/server"
  # Previous:
  #jvm_home="/usr/lib/jvm/java-1.8.0-openjdk-1.8.0.242.b08-0.el7_7.x86_64"
  # 2020-04-22:
  #jvm_home="/usr/lib/jvm/java-1.8.0-openjdk-1.8.0.252.b09-2.el7_8.x86_64"
  # 2020-08-10:
  #jvm_home="/usr/lib/jvm/java-1.8.0-openjdk-1.8.0.262.b10-0.el7_8.x86_64"
  # Link to current:
  jvm_home="/usr/lib/jvm/java-1.8.0"
  jvm_lib_path="${jvm_home}/jre/lib/amd64/server"
  if [ -z "${LD_LIBRARY_PATH+var_is_set}" ]
  then
    #LD_LIBRARY_PATH is not set
    export LD_LIBRARY_PATH="${jvm_lib_path}"
  else
    export LD_LIBRARY_PATH="${jvm_lib_path}:${LD_LIBRARY_PATH:?}"
  fi
}

# Helps set the build and install paths, which have a common last path segment of:
# rose-<branch>-<version>-<rose compiler>[-<target compiler>]
# e.g.
# rose-master-0.11.18.1-gcc-4.9.3-c++11
# rose-master-0.11.18.1-intel-19.0.4.mpi-intel-19.0.4.mpi
# 
# ROSE_COMPILER_SUFFIX is set separately in setup_gcc_compiler* and
# setup_intel_compiler*.  Those do not use ROSE_REPO_NAME because it is not 
# set if there is no rose build or rose use going on, e.g. build boost.
# 
# Uses:
#   ROSE_BUILD_BASE
#   ROSE_COMPILER_SUFFIX
#   ROSE_INSTALL_BASE
#   ROSE_REPO_NAME
# Sets:
#   ROSE_BUILD_PATH
#   ROSE_INSTALL_PATH
_set_ROSE_BUILD_PATH_and_ROSE_INSTALL_PATH () {
  export ROSE_BUILD_PATH="${ROSE_BUILD_BASE}/${ROSE_REPO_NAME}-${ROSE_COMPILER_SUFFIX}"
  export ROSE_INSTALL_PATH="${ROSE_INSTALL_BASE}/${ROSE_REPO_NAME}-${ROSE_COMPILER_SUFFIX}"  
}

# Uses:
#   ROSE_BUILD_BASE_REPO_NAME
#   ROSE_INSTALL_BASE_REPO_NAME
# Sets:
#   CC
#   CXX
#   LD_LIBRARY_PATH
#   ROSE_BACKEND_CC
#   ROSE_BACKEND_COMPILER_HOME
#   ROSE_BACKEND_CXX
#   ROSE_BUILD_PATH
#   ROSE_COMPILER
#   ROSE_COMPILER_CXX_STD
#   ROSE_COMPILER_CXX_STD_FLAG
#   ROSE_COMPILER_HOME
#   ROSE_COMPILER_SUFFIX
#   ROSE_COMPILER_VERSION
#   ROSE_COMPILER_VERSIONED
#   ROSE_COMPILER_VERSIONED_STD
setup_gcc_compiler () {
  _setup_jvm_library_path
  setup_gcc_compiler_base
  export  ROSE_COMPILER_SUFFIX="${ROSE_COMPILER_VERSIONED_STD}"
}

# Uses and Sets are same as setup_gcc_compiler, but does not set:
#   LD_LIBRARY_PATH
# Sets:
#   PATH
setup_gcc_compiler_with_ada () {
  setup_gcc_compiler_base
  export  ROSE_COMPILER_SUFFIX="${ROSE_COMPILER_VERSIONED_STD}-ada"
  # To get gprbuild etc.:
  # Be sure we use the right gcc and the right gnat.  There is a gcc we don't 
  # want in the gnat bin directory.  There is a gnat we don't want already in 
  # ${PATH}.  Let the new path be [gcc dir]:[gnat dir]:${PATH}:
  export PATH="\
  ${ROSE_COMPILER_HOME}/bin:\
  /collab/usr/global/tools/rose/toss_3_x86_64_ib/GNAT/2019/bin:\
  ${PATH}"
}

# Uses and Sets are same as setup_gcc_compiler:
setup_gcc_compiler_with_binary_analysis () {
  _setup_jvm_library_path
  setup_gcc_compiler_base
  export  ROSE_COMPILER_SUFFIX="${ROSE_COMPILER_VERSIONED_STD}-binary"
}

# Uses and Sets are same as setup_gcc_compiler, but does not set:
#   LD_LIBRARY_PATH
setup_gcc_compiler_with_c_cxx () {
  setup_gcc_compiler_base
  export  ROSE_COMPILER_SUFFIX="${ROSE_COMPILER_VERSIONED_STD}-c-cxx"
}

# Uses and Sets are same as setup_gcc_compiler:
setup_gcc_compiler_with_fortran () {
  _setup_jvm_library_path
  setup_gcc_compiler_base
  export  ROSE_COMPILER_SUFFIX="${ROSE_COMPILER_VERSIONED_STD}-fortran"
}

# Uses and Sets are same as setup_gcc_compiler, but does not set:
#   LD_LIBRARY_PATH
setup_gcc_compiler_with_profiling () {
  setup_gcc_compiler_base
  export  ROSE_COMPILER_SUFFIX="${ROSE_COMPILER_VERSIONED_STD}-gprof"
}

#======================================

# Run after setup_xxx_compiler - or run build_boost
# Uses:
#   ROSE_BOOST_VERSION
#   ROSE_COMPILER_VERSIONED
#   ROSE_MPI_KIND
#   ROSE_MPI_VERSION
# Sets:
#   ROSE_BOOST_HOME
setup_boost_tce () {
  # SELECT BOOST:
  export ROSE_BOOST_HOME="/usr/tce/packages/boost/boost-${ROSE_BOOST_VERSION}-${ROSE_MPI_KIND}-${ROSE_MPI_VERSION}-${ROSE_COMPILER_VERSIONED}"
}

# Run after setup_xxx_compiler, instead of setup_boost_tce:
# Builds with -std=c++11
# Uses:
#   ROSE_BOOST_VERSION
#   ROSE_COMPILER_VERSIONED_STD
#   ROSE_INSTALL_BASE
# Sets:
#   ROSE_BOOST_HOME
#   ROSE_BOOST_ROOT
#   ROSE_BOOST_VERSION_UNDERSCORES
#   ROSE_BOOST_BUILD_INSTALL_DIR
setup_boost_rose () {
  # Take out dots:
  export ROSE_BOOST_VERSION_UNDERSCORES=$(echo ${ROSE_BOOST_VERSION} | tr '.' '_')
  export ROSE_BOOST_ROOT="boost_${ROSE_BOOST_VERSION_UNDERSCORES}"
  export ROSE_BOOST_BUILD_INSTALL_DIR="${ROSE_BOOST_ROOT}-${ROSE_COMPILER_VERSIONED_STD}"
  export ROSE_BOOST_HOME="${ROSE_INSTALL_BASE}/${ROSE_BOOST_BUILD_INSTALL_DIR}"
}

setup_boost () {
  setup_boost_rose
}

# Run after setup_xxx_compiler, instead of use_tce_boost:
# Builds with -std=c++11
# Uses:
#   CC
#   CXX
#   ROSE_BOOST_BUILD_INSTALL_DIR
#   ROSE_BOOST_ROOT
#   ROSE_BOOST_VERSION
#   ROSE_BUILD_BASE
#   ROSE_COMPILER_CXX_STD_FLAG
#   ROSE_COMPILER_VERSIONED_STD
#   ROSE_INSTALL_BASE
#   RUN_AND_LOG
#   SRUN_DO
# Sets:
#   ROSE_BOOST_HOME
#   ROSE_BOOST_VERSION_UNDERSCORES
build_boost_rose () {
  setup_boost_rose
  # Using export below for ease of printing these local variables.  "set" shows 
  # unexported variables but shows function bodies too.
  # "env" only shows exported variables.
  export boost_bzip_file=${ROSE_BOOST_ROOT}.tar.bz2
  export boost_untar_path="${ROSE_BUILD_BASE}/${ROSE_BOOST_ROOT}"
  export boost_build_path="${ROSE_BUILD_BASE}/${ROSE_BOOST_BUILD_INSTALL_DIR}"
  export boost_install_path="${ROSE_BOOST_HOME}"
  log_separator_1
  env | grep '^ROSE_BOOST_'
  env | grep '^boost_'
  log_separator_1

  run_or_not cd ${ROSE_BUILD_BASE}
  run_or_not log_then_run wget -nv https://sourceforge.net/projects/boost/files/boost/${ROSE_BOOST_VERSION}/${boost_bzip_file}/download -O ${boost_bzip_file}
  run_or_not log_then_run ${SRUN_DO} tar jxf ${boost_bzip_file}
  run_or_not rm -f ${boost_bzip_file}
  run_or_not mv ${boost_untar_path} ${boost_build_path}

  run_or_not cd ${boost_build_path}
  run_or_not ${SRUN_DO} \
  ${RUN_AND_LOG} \
  ./bootstrap.sh --prefix=${boost_install_path} --with-libraries=all 
  run_or_not ${SRUN_DO} \
  ${RUN_AND_LOG} \
  ./b2 --prefix=${boost_install_path} -j${MAX_PROCS} cxxflags="${ROSE_COMPILER_CXX_STD_FLAG}" install
}

#==============================
# FOR INTEL MPI (Used by ARES):
#==============================
# Uses:
#   LD_LIBRARY_PATH
#   ROSE_BACKEND_COMPILER_HOME
#   ROSE_BOOST_HOME
#   ROSE_COMPILER_CXX_STD_FLAG
#   ROSE_COMPILER_SUFFIX
#   ROSE_REPO_PATH
#   RUN_AND_LOG
#   SRUN_DO
# Sets:
#   ROSE_BUILD_PATH
#   ROSE_INSTALL_PATH
do_intel_configure () {
  _set_ROSE_BUILD_PATH_and_ROSE_INSTALL_PATH
  run_or_not mkdir -p ${ROSE_BUILD_PATH}
  run_or_not cd ${ROSE_BUILD_PATH}
  echo "------------------------"
  echo "NEEDED: LD_LIBRARY_PATH does not have any non-gcc compiler paths in it."
  if [ -z "${LD_LIBRARY_PATH+var_is_set}" ]
  then
    echo "LD_LIBRARY_PATH is not set"
  else
    echo "LD_LIBRARY_PATH=${LD_LIBRARY_PATH:?}"
  fi
  echo "------------------------"
  echo "DO NOT run multiple configures using ${ROSE_REPO_PATH} in parallel if they need different intel versions."
  echo "------------------------"
  echo "DOING configure"
  run_or_not ${SRUN_DO} \
  ${RUN_AND_LOG} \
  ${ROSE_REPO_PATH}/configure \
  --prefix=${ROSE_INSTALL_PATH} \
  --without-java \
  --with-boost=${ROSE_BOOST_HOME} \
  --disable-binary-analysis \
  --disable-boost-version-check \
  --enable-edg_version=5.0 \
  --with-alternate_backend_Cxx_compiler=${ROSE_BACKEND_COMPILER_HOME}/bin/mpicxx \
  --with-alternate_backend_C_compiler=${ROSE_BACKEND_COMPILER_HOME}/bin/mpicc \
  --with-alternate_backend_fortran_compiler=${ROSE_BACKEND_COMPILER_HOME}/bin/mpif77 \
  CXXFLAGS="${ROSE_COMPILER_CXX_STD_FLAG} -g -O2 -Wall"

  # NOTE --disable-binary-analysis above is because (configure output):
  # Checking blacklisted configurations
  # configure: boost version     = 106600
  # configure: frontend compiler = intel-18.0
  # configure: binary analysis   = yes
  # configure:
  #             The following combination of ROSE software dependencies is blacklisted. The ROSE
  #             team occassionally blacklists combinations that are known to cause failures in
  #             perhaps subtle ways. If you would like to continue with this configuration at your
  #             own risk, then reconfigure with --disable-blacklist. (This message comes from
  #             config/support-blacklist.m4.)
}
#==============================

#===============================================
# FOR GCC 4.9.3 or 6.1.0 non-MPI (used by Kull):
#===============================================
# Uses:
#   ROSE_BOOST_HOME
#   ROSE_COMPILER
#   ROSE_COMPILER_CXX_STD_FLAG
#   ROSE_COMPILER_SUFFIX
#   ROSE_COMPILER_VERSION
#   ROSE_REPO_PATH
#   RUN_AND_LOG
#   SRUN_DO
# Sets:
#   ROSE_BUILD_PATH
#   ROSE_INSTALL_PATH
_do_gcc_configure_common () {
  _set_ROSE_BUILD_PATH_and_ROSE_INSTALL_PATH
  # Optional parameters are added to the end of the configure parameters.
  run_or_not mkdir -p ${ROSE_BUILD_PATH}
  run_or_not cd ${ROSE_BUILD_PATH}
  _turn_on_module
  module load ${ROSE_COMPILER}/${ROSE_COMPILER_VERSION}
  run_or_not ${SRUN_DO} \
  ${RUN_AND_LOG} \
  ${ROSE_REPO_PATH}/configure \
  --prefix=${ROSE_INSTALL_PATH} \
  --with-boost=${ROSE_BOOST_HOME} \
  --disable-boost-version-check \
  CXXFLAGS="${ROSE_COMPILER_CXX_STD_FLAG} -g -O2 -Wall" \
  "$@"
  # Original flags:
  # CXXFLAGS='-O2 -Wall -Wall'
 # Future:
 # no --disable-boost-version-check
 # 
}

# Enables C, CXX, Fortran, and Binary Analysis:
# Same Uses and Sets as _do_gcc_configure_common
do_gcc_configure () {
  _do_gcc_configure_common \
  --enable-c \
  --enable-cxx \
  --enable-fortran \
  --enable-binary-analysis \
  --with-sqlite3
}

# Same Uses and Sets as _do_gcc_configure_common
do_gcc_configure_with_ada () {
  _do_gcc_configure_common \
  --enable-experimental_ada_frontend \
  --disable-binary-analysis \
  --without-swi-prolog \
  --without-cuda \
  --disable-fortran \
  --without-java \
  --without-python \
  CXXFLAGS="${ROSE_COMPILER_CXX_STD_FLAG} -g -O2 -Wall"
}

# Same Uses and Sets as _do_gcc_configure_common
do_gcc_configure_with_binary_analysis () {
  # Per Jim Leek 2019-12-09
  _do_gcc_configure_common \
  --enable-c \
  --enable-cxx \
  --enable-binary-analysis \
  --with-sqlite3
}

# Same Uses and Sets as _do_gcc_configure_common
# Just C and CXX, no Java dependency:
do_gcc_configure_with_c_cxx () {
  _do_gcc_configure_common \
  --enable-c \
  --enable-cxx \
  --without-java
}

# Same Uses and Sets as _do_gcc_configure_common
do_gcc_configure_with_fortran () {
  # Fortran needs Java.
  _do_gcc_configure_common \
  --enable-c \
  --enable-cxx \
  --enable-fortran
}

# Same Uses and Sets as _do_gcc_configure_common
do_gcc_configure_with_profiling () {
  _do_gcc_configure_common \
  --enable-c \
  --enable-cxx \
  --without-java \
  CFLAGS='-pg -g -O2 -Wall -Wstrict-prototypes -Wmissing-prototypes' \
  CXXFLAGS='${ROSE_COMPILER_CXX_STD_FLAG} -pg -g -O2 -Wall'
  # Original flags:
  # CFLAGS='-g -O2 -O2 -Wall -Wstrict-prototypes -Wmissing-prototypes -Wall -Wstrict-prototypes -Wmissing-prototypes' 
  # CXXFLAGS='-O2 -Wall -Wall'
}

#===============================================


# Uses:
#   rose_latest_install_version_file
#   ROSE_DO_UPDATE_VERSIONS
#   ROSE_REPO_PATH
_update_latest_installed_version () {
  if [ ${ROSE_DO_UPDATE_VERSIONS} == "TRUE" ] 
  then
    run_or_not cp ${ROSE_REPO_PATH}/ROSE_VERSION ${rose_latest_install_version_file}
  fi
}

#======================================
# Make and install ROSE:
#======================================
# Uses:
#   rose_latest_install_version_file
#   ROSE_COMPILER_SUFFIX
#   ROSE_DO_UPDATE_VERSIONS
#   ROSE_REPO_PATH
# Sets:
#   ROSE_BUILD_PATH
#   ROSE_INSTALL_PATH
make_and_install () {
  _set_ROSE_BUILD_PATH_and_ROSE_INSTALL_PATH
  run_or_not cd ${ROSE_BUILD_PATH}
  run_or_not ${SRUN_DO} -c${MAX_PROCS} ${RUN_AND_LOG} --use_first_parm make all -j${MAX_PROCS}
  run_or_not ${SRUN_DO} -c${MAX_PROCS} ${RUN_AND_LOG} --use_first_parm make install -j${MAX_PROCS}
  # Install the comp db scripts in prefix/bin (currently ${ROSE_BUILD_PATH}/bin):
#  cd projects/CompilationDB
#  ${RUN_AND_LOG} --use_first_parm make install
  _update_latest_installed_version
}
#======================================

#======================================
# Make ROSE docs:
#======================================
# Uses:
#   ROSE_BUILD_PATH
make_docs () {
  run_or_not cd ${ROSE_BUILD_PATH}
  run_or_not ${SRUN_DO} -c${MAX_PROCS} ${RUN_AND_LOG} --use_first_parm make check -j${MAX_PROCS}
  run_or_not ${SRUN_DO} -c${MAX_PROCS} ${RUN_AND_LOG} --use_first_parm make docs -j${MAX_PROCS}
}
#======================================

_temp () {
 module load vtune
}

