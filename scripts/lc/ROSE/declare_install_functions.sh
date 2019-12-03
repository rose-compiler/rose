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
#   _do_preconfigure_no_latest_copy
#   _print_rose_vars
#   _set_REPO_PATH_and_INSTALL_BASE_VERSIONED
#   _turn_on_module
#   clone_latest_workspace
#   do_gcc_configure
#   do_gcc_configure_common
#   do_gcc_configure_with_profiling
#   do_intel_configure
#   do_preconfigure
#   echo_version_from_file
#   make_and_install
#   make_docs
#   set_ROSE_HOME_ROSE_LD_LIBRARY_PATH
#   set_ROSE_LATEST_INSTALL_VERSION
#   set_main_vars
#   setup_boost
#   setup_gcc_compiler
#   setup_gcc_compiler_with_profiling
#   setup_intel_18_0_2_compiler_non_mpi
#   setup_intel_compiler
#   use_existing_workspace
#   use_latest_existing_install
#   use_latest_existing_workspace
#   use_latest_gcc_rose
#   use_latest_gcc_rose_with_profiling
#   use_latest_intel_rose
#   use_latest_rose
#
# Example users:
#  clone_latest:
#   set_main_vars
#   clone_latest_workspace
#
#  clone_and_preconfigure.sh:
#   set_main_vars
#   clone_latest_workspace
#   do_preconfigure
# 
#  install_with_gcc.sh:
#   set_main_vars
#   use_latest_existing_workspace
#   setup_gcc_compiler
#   setup_boost
#   do_gcc_configure
#   make_and_install
# 
#  install_with_intel.sh:
#   set_main_vars
#   use_latest_existing_workspace
#   setup_intel_compiler
#   setup_boost
#   do_intel_configure
#   make_and_install

# Optional lines for client scripts:
# Don't actually run:
#   export RUN_OR_NOT_EFFORT_ONLY=TRUE
#
# For manual testing:
#   export ROSE_BUILD_BASE="${HOME}/code/ROSE"

# Find ourselves:
rel_enclosing_dir=`dirname ${BASH_SOURCE[0]}`
export ROSE_LC_SCRIPT_DIR=`(cd ${rel_enclosing_dir}/..; pwd)`
export ROSE_BIN_SCRIPT_DIR="${ROSE_LC_SCRIPT_DIR}/bin"
export ROSE_COMPDB_SCRIPT_DIR="${ROSE_LC_SCRIPT_DIR}/compdb"
export ROSE_ROSE_SCRIPT_DIR="${ROSE_LC_SCRIPT_DIR}/ROSE"

# Sets strict mode, defines set_strict, unset_strict, log_*,
# log_then_run, run_or_not, etc.:
source ${ROSE_BIN_SCRIPT_DIR}/utility_functions.sh

export RUN_AND_LOG=${ROSE_BIN_SCRIPT_DIR}/run_and_log
export SRUN_DO=${ROSE_BIN_SCRIPT_DIR}/srun_do

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

_print_rose_vars () {
  log_separator_1
  log "ROSE environment variables:"
  log "COMMON_BUILD_BASE=${COMMON_BUILD_BASE}"
  log "COMMON_INSTALL_BASE=${COMMON_INSTALL_BASE}"
  for line in `env | grep '^ROSE' | sort`
  do
    log $line
  done
  log_separator_1
}

# Uses these if set, else sets them:
#   COMMON_BUILD_BASE
#   ROSE_BUILD_BASE
#   COMMON_INSTALL_BASE
#   ROSE_INSTALL_BASE
#   ROSE_BRANCH_NAME_GIT
# Sets more, see code:
set_main_vars () {
  export COMMON_BUILD_BASE=${COMMON_BUILD_BASE:-"${HOME}/code"}
  export ROSE_BUILD_BASE=${ROSE_BUILD_BASE:-"${COMMON_BUILD_BASE}/ROSE"}
  export COMMON_INSTALL_BASE=${COMMON_INSTALL_BASE:-"/collab/usr/global/tools"}
  export ROSE_INSTALL_BASE=${ROSE_INSTALL_BASE:-"${COMMON_INSTALL_BASE}/rose/${SYS_TYPE}"}
  # The Git branch name may include "/":
  # e.g. "reynolds12/automation"
  export ROSE_BRANCH_NAME_GIT=${ROSE_BRANCH_NAME_GIT:-"master"}
  
  export ROSE_LATEST_WORKSPACE_VERSION_FILE="${ROSE_BUILD_BASE}/VERSION_OF_LATEST_WORKSPACE"
  export ROSE_LATEST_INSTALL_VERSION_FILE="${ROSE_INSTALL_BASE}/VERSION_OF_LATEST_INSTALL"
  # Used by test_this_build.sh and others:
  export ROSE_LATEST_INSTALL_VERSION=`echo_version_from_file ${ROSE_LATEST_INSTALL_VERSION_FILE}`
  
  # The LLNL ROSE git project and repo:
  export ROSE_REMOTE_PROJECT="rose-dev@rosecompiler1.llnl.gov:rose/scratch"

  # e.g. "rose-dev@rosecompiler1.llnl.gov:rose/scratch/rose":
  export ROSE_REMOTE_REPO="${ROSE_REMOTE_PROJECT}/rose"
  # e.g. "${HOME}/code/ROSE/rose":
  export ROSE_REPO_PATH_CLONED="${ROSE_BUILD_BASE}/rose"
  # Take out any slashes:
  # e.g. "dquinlan/dq-development-dev" -> "dquinlan-dq-development-dev"
  export ROSE_BRANCH_PATH_PART=$(echo ${ROSE_BRANCH_NAME_GIT} | tr '/' '-')
  
  # Non-compiler version info:
  export ROSE_MPI_KIND="mvapich2"
  export ROSE_MPI_VERSION="2.2"
  export ROSE_BOOST_VERSION="1.66.0"
}

# Uses:
#   ROSE_BRANCH_PATH_PART
#   ROSE_BUILD_BASE
#   ROSE_INSTALL_BASE
#   ROSE_VERSION
# Sets:
#   ROSE_INSTALL_BASE_VERSIONED
#   ROSE_REPO_PATH_VERSIONED
_set_REPO_PATH_and_INSTALL_BASE_VERSIONED () {
  rose_branch_version="rose-${ROSE_BRANCH_PATH_PART}-${ROSE_VERSION}"
  export ROSE_REPO_PATH_VERSIONED="${ROSE_BUILD_BASE}/${rose_branch_version}"
  export ROSE_INSTALL_BASE_VERSIONED="${ROSE_INSTALL_BASE}/${rose_branch_version}"
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

set_ROSE_LATEST_INSTALL_VERSION () {
  export ROSE_LATEST_INSTALL_VERSION=`echo_version_from_file ${ROSE_LATEST_INSTALL_VERSION_FILE}`
}

clone_latest_workspace () {
  cd ${ROSE_BUILD_BASE}
  run_or_not git clone ${ROSE_REMOTE_REPO}
  run_or_not cd ${ROSE_REPO_PATH_CLONED}
  # Check out the desired branch as needed:
  run_or_not git checkout ${ROSE_BRANCH_NAME_GIT}
  run_or_not git submodule init
  run_or_not git submodule update
  export ROSE_VERSION=`echo_version_from_file ROSE_VERSION`
# Uses:
#   ROSE_BRANCH_PATH_PART
#   ROSE_BUILD_BASE
#   ROSE_INSTALL_BASE
#   ROSE_VERSION
# Sets:
#   ROSE_INSTALL_BASE_VERSIONED
#   ROSE_REPO_PATH_VERSIONED
  _set_REPO_PATH_and_INSTALL_BASE_VERSIONED
  run_or_not mv ${ROSE_REPO_PATH_CLONED} ${ROSE_REPO_PATH_VERSIONED}
}

# Uses:
#   One parameter - the ROSE version
#   ROSE_BRANCH_PATH_PART
#   ROSE_BUILD_BASE
#   ROSE_INSTALL_BASE
# Sets:
#   ROSE_INSTALL_BASE_VERSIONED
#   ROSE_REPO_PATH_VERSIONED
#   ROSE_VERSION
use_existing_workspace () {
  export ROSE_VERSION=$1
# Uses:
#   ROSE_BRANCH_PATH_PART
#   ROSE_BUILD_BASE
#   ROSE_INSTALL_BASE
#   ROSE_VERSION
# Sets:
#   ROSE_INSTALL_BASE_VERSIONED
#   ROSE_REPO_PATH_VERSIONED
  _set_REPO_PATH_and_INSTALL_BASE_VERSIONED
}

# Uses:
#   ROSE_BRANCH_PATH_PART
#   ROSE_BUILD_BASE
#   ROSE_INSTALL_BASE
#   ROSE_LATEST_WORKSPACE_VERSION_FILE
# Sets:
#   ROSE_INSTALL_BASE_VERSIONED
#   ROSE_REPO_PATH_VERSIONED
#   ROSE_VERSION
use_latest_existing_workspace () {
  use_existing_workspace `cat ${ROSE_LATEST_WORKSPACE_VERSION_FILE}`
}

#========================
# For test_this_build.sh:
#========================

# Uses:
#   ROSE_BRANCH_PATH_PART
#   ROSE_BUILD_BASE
#   ROSE_INSTALL_BASE
#   ROSE_LATEST_INSTALL_VERSION
# Sets:
#   ROSE_BACKEND_CXX
#   ROSE_INSTALL_BASE_VERSIONED
#   ROSE_REPO_PATH_VERSIONED
#   ROSE_VERSION
use_latest_existing_install () {
  use_existing_workspace ${ROSE_LATEST_INSTALL_VERSION}
}

# Uses:
#   ROSE_INSTALL_PATH
# Sets:
#   ROSE_HOME
#   ROSE_LD_LIBRARY_PATH
set_ROSE_HOME_ROSE_LD_LIBRARY_PATH () {
  export ROSE_HOME=${ROSE_INSTALL_PATH}
  export ROSE_LD_LIBRARY_PATH=\
"${ROSE_HOME}/src/.libs":\
"${ROSE_HOME}/src/3rdPartyLibraries/libharu-2.1.0/src/.libs"
}

# Uses:
#   One parameter - "gcc" or "intel"
#   ROSE_COMPDB_SCRIPT_DIR
# Sets:
#   COMMON_BUILD_BASE
#   COMP_DB_MAP
#   RENDER_TEXT
#   ROSE_HOME
#   ROSE_LD_LIBRARY_PATH
#   ROSE_TOOL
# See set_main_vars for overridable vars.
use_latest_rose () {
# Sets:
#   COMMON_BUILD_BASE
#   ROSE_INSTALL_BASE
#   ROSE_LATEST_INSTALL_VERSION
  set_main_vars

# Uses:
#   ROSE_INSTALL_BASE
#   ROSE_LATEST_INSTALL_VERSION
# Sets:
#   ROSE_INSTALL_BASE_VERSIONED
  use_latest_existing_install

# Uses:
#   ROSE_INSTALL_BASE_VERSIONED
# Sets:
#   ROSE_BACKEND_CXX
#   ROSE_INSTALL_PATH
  if [ $1 == "gcc" ]
  then
    setup_gcc_compiler
  elif [ $1 == "gcc_profiling" ]
  then
    setup_gcc_compiler_with_profiling
  elif [ $1 == "intel" ]
  then
    setup_intel_compiler
  else
    echo "use_latest_rose: ERROR: parm was \"$1\" not gcc or intel. Calling exit 1."
    exit 1
  fi  

# Uses:
#   ROSE_INSTALL_PATH
# Sets:
#   ROSE_HOME
#   ROSE_LD_LIBRARY_PATH
  set_ROSE_HOME_ROSE_LD_LIBRARY_PATH

  export COMP_DB_MAP="${ROSE_COMPDB_SCRIPT_DIR}/comp_db_map.py"
  export RENDER_TEXT="${ROSE_COMPDB_SCRIPT_DIR}/render_text.py"
  export ROSE_TOOL="${ROSE_HOME}/bin/identityTranslator"
  _print_rose_vars
}

# All "use_latest..." below set:
#   COMMON_BUILD_BASE
#   COMP_DB_MAP
#   RENDER_TEXT
#   ROSE_BACKEND_CXX
#   ROSE_HOME
#   ROSE_LD_LIBRARY_PATH
#   ROSE_TOOL
# See set_main_vars for overridable vars.
use_latest_gcc_rose () {
  use_latest_rose "gcc"
}
use_latest_gcc_rose_with_profiling () {
  use_latest_rose "gcc_profiling"
}
use_latest_intel_rose () {
  use_latest_rose "intel"
}

#========================

# Uses:
#   ROSE_REPO_PATH_VERSIONED
_do_preconfigure_no_latest_copy () {
  run_or_not cd ${ROSE_REPO_PATH_VERSIONED}
  run_or_not ${SRUN_DO} ${RUN_AND_LOG} ./build
}

# Uses:
#   ROSE_LATEST_WORKSPACE_VERSION_FILE
#   ROSE_REPO_PATH_VERSIONED
do_preconfigure () {
  _do_preconfigure_no_latest_copy
  run_or_not cp ${ROSE_REPO_PATH_VERSIONED}/ROSE_VERSION ${ROSE_LATEST_WORKSPACE_VERSION_FILE}
}

#=====================================
# FOR INTEL 18.0.1 MPI (Used by ARES):
#=====================================
# Uses:
#   ROSE_INSTALL_BASE_VERSIONED
#   ROSE_REPO_PATH_VERSIONED
# Sets:
#   CC
#   CXX
#   ROSE_BACKEND_CC
#   ROSE_BACKEND_COMPILER_HOME
#   ROSE_BACKEND_CXX
#   ROSE_BUILD_PATH
#   ROSE_COMPILER
#   ROSE_COMPILER_HOME
#   ROSE_COMPILER_VERSION
#   ROSE_COMPILER_VERSIONED
#   ROSE_INSTALL_PATH
setup_intel_compiler () {
  export ROSE_COMPILER="intel"
  export ROSE_COMPILER_VERSION="18.0.1"
  export ROSE_COMPILER_VERSIONED="${ROSE_COMPILER}-${ROSE_COMPILER_VERSION}"
  export ROSE_COMPILER_HOME="/usr/tce/packages/${ROSE_MPI_KIND}/${ROSE_MPI_KIND}-${ROSE_MPI_VERSION}-${ROSE_COMPILER_VERSIONED}"
  export  CC="${ROSE_COMPILER_HOME}/bin/mpicc"
  export CXX="${ROSE_COMPILER_HOME}/bin/mpic++"

  export ROSE_BACKEND_COMPILER="intel"
  export ROSE_BACKEND_COMPILER_VERSION="18.0.1"
  export ROSE_BACKEND_COMPILER_VERSIONED="${ROSE_BACKEND_COMPILER}-${ROSE_BACKEND_COMPILER_VERSION}"
  export ROSE_BACKEND_COMPILER_HOME="/usr/tce/packages/${ROSE_MPI_KIND}/${ROSE_MPI_KIND}-${ROSE_MPI_VERSION}-${ROSE_BACKEND_COMPILER_VERSIONED}"
  export  ROSE_BACKEND_CC="${ROSE_BACKEND_COMPILER_HOME}/bin/mpicc"
  export ROSE_BACKEND_CXX="${ROSE_BACKEND_COMPILER_HOME}/bin/mpic++"

  #---------------------------------------
  # Run the Intel setup script so we don't have to add 
  # "-L/usr/tce/packages/intel/intel-18.0.1/lib/intel64" to line 275 of 
  # "${ROSE_REPO_PATH_VERSIONED}/config/Makefile.for.ROSE.includes.and.libs" to avoid
  # link problems:
  export ROSE_NON_MPI_COMPILER_HOME="/usr/tce/packages/${ROSE_COMPILER}/${ROSE_COMPILER_VERSIONED}"
  export COMPILERVARS_ARCHITECTURE=intel64
  export COMPILERVARS_PLATFORM=linux
  push_set_state
  set +u
  source ${ROSE_NON_MPI_COMPILER_HOME}/bin/compilervars.sh
  # Yes, you need gcc 4.9.3 or later for intel:
  _turn_on_module
  module load gcc/6.1.0
  pop_set_state
  #---------------------------------------

  export ROSE_COMPILER_PATH_PART="${ROSE_COMPILER_VERSIONED}.mpi-${ROSE_BACKEND_COMPILER_VERSIONED}.mpi"
  export ROSE_INSTALL_PATH="${ROSE_INSTALL_BASE_VERSIONED}-${ROSE_COMPILER_PATH_PART}"
  export ROSE_BUILD_PATH="${ROSE_REPO_PATH_VERSIONED}-${ROSE_COMPILER_PATH_PART}"
}
#=====================================

#=====================================
# FOR INTEL 18.0.2 non-MPI (Used by BLAST):
#=====================================
# Uses:
#   ROSE_INSTALL_BASE_VERSIONED
#   ROSE_REPO_PATH_VERSIONED
# Sets:
#   CC
#   CXX
#   ROSE_BACKEND_CC
#   ROSE_BACKEND_COMPILER_HOME
#   ROSE_BACKEND_CXX
#   ROSE_BUILD_PATH
#   ROSE_COMPILER
#   ROSE_COMPILER_HOME
#   ROSE_COMPILER_VERSION
#   ROSE_COMPILER_VERSIONED
#   ROSE_INSTALL_PATH
setup_intel_18_0_2_compiler_non_mpi () {
  export ROSE_COMPILER="intel"
  export ROSE_COMPILER_VERSION="18.0.2"
  export ROSE_COMPILER_VERSIONED="${ROSE_COMPILER}-${ROSE_COMPILER_VERSION}"
  export ROSE_COMPILER_HOME="/usr/tce/packages/${ROSE_COMPILER}/${ROSE_COMPILER_VERSIONED}"
  export  CC="${ROSE_COMPILER_HOME}/bin/icc"
  export CXX="${ROSE_COMPILER_HOME}/bin/icpc"

  export ROSE_BACKEND_COMPILER=${ROSE_COMPILER}
  export ROSE_BACKEND_COMPILER_VERSIONED=${ROSE_COMPILER_VERSIONED}
  export ROSE_BACKEND_COMPILER_HOME=${ROSE_COMPILER_HOME}
  export  ROSE_BACKEND_CC="${ROSE_BACKEND_COMPILER_HOME}/bin/icc"
  export ROSE_BACKEND_CXX="${ROSE_BACKEND_COMPILER_HOME}/bin/icpc"

  #---------------------------------------
  # Run the Intel setup script so we don't have to add 
  # "-L/usr/tce/packages/intel/intel-18.0.1/lib/intel64" to line 275 of 
  # "${ROSE_REPO_PATH_VERSIONED}/config/Makefile.for.ROSE.includes.and.libs" to avoid
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

  export ROSE_COMPILER_PATH_PART="${ROSE_COMPILER_VERSIONED}-${ROSE_BACKEND_COMPILER_VERSIONED}"
  export ROSE_BUILD_PATH="${ROSE_REPO_PATH_VERSIONED}-${ROSE_COMPILER_PATH_PART}"
  export ROSE_INSTALL_PATH="${ROSE_INSTALL_BASE_VERSIONED}-${ROSE_COMPILER_PATH_PART}"
}
#=====================================

#======================================
# FOR GCC 4.9.3 or 6.1.0 non-MPI (used by Kull):
#======================================
# Uses:
#   ROSE_INSTALL_BASE_VERSIONED
#   ROSE_REPO_PATH_VERSIONED
# Sets:
#   CC
#   CXX
#   ROSE_BACKEND_CC
#   ROSE_BACKEND_COMPILER_HOME
#   ROSE_BACKEND_CXX
#   ROSE_BUILD_PATH
#   ROSE_COMPILER
#   ROSE_COMPILER_HOME
#   ROSE_COMPILER_VERSION
#   ROSE_COMPILER_VERSIONED
#   ROSE_INSTALL_PATH
setup_gcc_compiler () {
  export ROSE_COMPILER="gcc"
#  export ROSE_COMPILER_VERSION="6.1.0"
  export ROSE_COMPILER_VERSION="4.9.3"
  export ROSE_COMPILER_VERSIONED="${ROSE_COMPILER}-${ROSE_COMPILER_VERSION}"
  export ROSE_COMPILER_HOME="/usr/tce/packages/${ROSE_COMPILER}/${ROSE_COMPILER_VERSIONED}"
  export  CC="${ROSE_COMPILER_HOME}/bin/cc"
  export CXX="${ROSE_COMPILER_HOME}/bin/c++"

  # ROSE_BACKEND_COMPILER_HOME is not used to build ROSE, but may be useful elsewhere e.g. run_rose.sh:
  export ROSE_BACKEND_COMPILER_HOME=${ROSE_COMPILER_HOME}
  export  ROSE_BACKEND_CC="${ROSE_BACKEND_COMPILER_HOME}/bin/cc"
  export ROSE_BACKEND_CXX="${ROSE_BACKEND_COMPILER_HOME}/bin/c++"

  export ROSE_COMPILER_PATH_PART="${ROSE_COMPILER_VERSIONED}"
  export ROSE_BUILD_PATH="${ROSE_REPO_PATH_VERSIONED}-${ROSE_COMPILER_PATH_PART}"
  export ROSE_INSTALL_PATH="${ROSE_INSTALL_BASE_VERSIONED}-${ROSE_COMPILER_PATH_PART}"
}

setup_gcc_compiler_with_profiling () {
  setup_gcc_compiler
  export ROSE_COMPILER_PATH_PART="${ROSE_COMPILER_VERSIONED}-gprof"
  export ROSE_BUILD_PATH="${ROSE_REPO_PATH_VERSIONED}-${ROSE_COMPILER_PATH_PART}"
  export ROSE_INSTALL_PATH="${ROSE_INSTALL_BASE_VERSIONED}-${ROSE_COMPILER_PATH_PART}"  
}
#======================================

# Run after setup_xxx_compiler:
setup_boost () {
  # SELECT BOOST:
  export ROSE_BOOST_HOME="/usr/tce/packages/boost/boost-${ROSE_BOOST_VERSION}-${ROSE_MPI_KIND}-${ROSE_MPI_VERSION}-${ROSE_COMPILER_VERSIONED}"
  _print_rose_vars
}

#=====================================
# FOR INTEL 18.0.1 MPI (Used by ARES):
#=====================================
do_intel_configure () {
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
  echo "DO NOT run multiple configures using ${ROSE_REPO_PATH_VERSIONED} in parallel if they need different intel versions."
  echo "------------------------"
  echo "DOING configure"
  run_or_not ${SRUN_DO} \
  ${RUN_AND_LOG} \
  ${ROSE_REPO_PATH_VERSIONED}/configure \
  --prefix=${ROSE_INSTALL_PATH} \
  --without-java \
  --with-boost=${ROSE_BOOST_HOME} \
  --disable-binary-analysis \
  --disable-boost-version-check \
  --enable-edg_version=5.0 \
  --with-alternate_backend_Cxx_compiler=${ROSE_BACKEND_COMPILER_HOME}/bin/mpicxx \
  --with-alternate_backend_C_compiler=${ROSE_BACKEND_COMPILER_HOME}/bin/mpicc \
  --with-alternate_backend_fortran_compiler=${ROSE_BACKEND_COMPILER_HOME}/bin/mpif77

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
#=====================================

#===============================================
# FOR GCC 4.9.3 or 6.1.0 non-MPI (used by Kull):
#===============================================
do_gcc_configure_common () {
# Optional parameters are added to the end of the configure parameters.
  run_or_not mkdir -p ${ROSE_BUILD_PATH}
  run_or_not cd ${ROSE_BUILD_PATH}
  _turn_on_module
  module load ${ROSE_COMPILER}/${ROSE_COMPILER_VERSION}
  run_or_not ${SRUN_DO} \
  ${RUN_AND_LOG} \
  ${ROSE_REPO_PATH_VERSIONED}/configure \
  --prefix=${ROSE_INSTALL_PATH} \
  --without-java \
  --with-boost=${ROSE_BOOST_HOME} \
  --disable-boost-version-check \
  --enable-edg_version=5.0 \
  "$@"
 # Future:
 # --enable-lang (e.g. C)
 # --with-boost=${ROSE_BOOST_HOME} \
 # no --without-java
 # no --disable-boost-version-check
 # no --enable-edg_version=5.0
 # 
}

do_gcc_configure () {
  do_gcc_configure_common
}

do_gcc_configure_with_profiling () {
  do_gcc_configure_common \
  CFLAGS='-pg -g -O2 -Wall -Wstrict-prototypes -Wmissing-prototypes' \
  CXXFLAGS='-pg -g -O2 -Wall'
  # Original flags:
  # CFLAGS='-g -O2 -O2 -Wall -Wstrict-prototypes -Wmissing-prototypes -Wall -Wstrict-prototypes -Wmissing-prototypes' 
  # CXXFLAGS='-O2 -Wall -Wall'
}
#===============================================

#======================================
# Make and install ROSE:
#======================================
make_and_install () {
  run_or_not cd ${ROSE_BUILD_PATH}
  run_or_not ${SRUN_DO} -c36 ${RUN_AND_LOG} --use_first_parm make all -j36
  run_or_not ${SRUN_DO} -c36 ${RUN_AND_LOG} --use_first_parm make install -j36
  # Install the comp db scripts in prefix/bin (currently ${ROSE_BUILD_PATH}/bin):
#  cd projects/CompilationDB
#  ${RUN_AND_LOG} --use_first_parm make install
  run_or_not cp ${ROSE_REPO_PATH_VERSIONED}/ROSE_VERSION ${ROSE_LATEST_INSTALL_VERSION_FILE}
}
#======================================

#======================================
# Make ROSE docs:
#======================================
make_docs () {
  run_or_not cd ${ROSE_BUILD_PATH}
  run_or_not ${SRUN_DO} -c36 ${RUN_AND_LOG} --use_first_parm make check -j36
  run_or_not ${SRUN_DO} -c36 ${RUN_AND_LOG} --use_first_parm make docs -j36
}
#======================================

