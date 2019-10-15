#!/bin/echo "ERROR: Do not execute this script! Source it from bash:"
#
# This script defines functions for use by ROSE installation scripts.
# DEPENDENCIES: 
#   ROSE_PROJECT_BASE (optional)
#   ../bin/utility_functions.sh

enclosing_directory=`dirname ${BASH_SOURCE[0]}`
export ROSE_SCRIPT_DIR=`(cd ${enclosing_directory}; pwd)`
export ROSE_BIN_SCRIPT_DIR=`dirname ${ROSE_SCRIPT_DIR}`/bin

export RUN_AND_LOG=${ROSE_BIN_SCRIPT_DIR}/run_and_log
export SRUN_DO=${ROSE_BIN_SCRIPT_DIR}/srun_do
###############################################################################
# This is  is an excerpt from /g/g17/charles/.profile.toss_3_x86_64_ib:
# Sets strict mode, defines set_strict, unset_strict, log_*,
# log_then_run, run_or_not, etc.:
source ${ROSE_BIN_SCRIPT_DIR}/utility_functions.sh

# Run this again or else "module" doesn't work (might have been run by 
#   /etc/profile -> /etc/profile.d/z00_lmod.sh but ENVIRONMENT was BATCH then):
#
# Turn off unset variable detection because z00_lmod.sh reads BASH_ENV, which 
# is not set when .profile is run for the first time by .bashrc, e.g. when
# connecting to quartz with x-win32 using "/usr/bin/xterm -ls":
push_set_state
set +u
source /etc/profile.d/z00_lmod.sh
# Turn it back on:
pop_set_state
# ...
module use /usr/tce/modulefiles/Core
module use /usr/tce/modulefiles/Compiler
module use /usr/tce/modulefiles/MPI
# End excerpt
###############################################################################

print_rose_vars () {
  log_separator_1
  for line in `env | grep '^ROSE' | sort`
  do
    log $line
  done
  log_separator_1
}

set_main_vars () {
  export SYS_TYPE=${SYS_TYPE:-"toss_3_x86_64_ib"}
  export ROSE_PROJECT_BASE=${ROSE_PROJECT_BASE:-"/collab/usr/global/tools/rose/${SYS_TYPE}"}
  # The LLNL ROSE git project and repo:
  export ROSE_REMOTE_PROJECT="rose-dev@rosecompiler1.llnl.gov:rose/scratch"
  export ROSE_REPO_NAME="rose"

  # The Git branch name may include "/":
  #export ROSE_BRANCH_NAME_GIT="rasmussen17/jovial-directives"
  #export ROSE_BRANCH_NAME_GIT="rose/reynolds12/automation"
  #export ROSE_BRANCH_NAME_GIT="schordan1/codethorn-dev"
  #export ROSE_BRANCH_NAME_GIT="rasmussen17/jovial-tables-dev"
  export ROSE_BRANCH_NAME_GIT=${ROSE_BRANCH_NAME_GIT:-"master"}

  # e.g. "rose-dev@rosecompiler1.llnl.gov:rose/scratch/rose":
  export ROSE_REMOTE_REPO="${ROSE_REMOTE_PROJECT}/${ROSE_REPO_NAME}"
  # e.g. "${HOME}/code/ROSE/rose":
  export ROSE_REPO_PATH_CLONED=${ROSE_PROJECT_BASE}/${ROSE_REPO_NAME}
  # Take out any slashes:
  # e.g. "dquinlan/dq-development-dev" -> "rose-dquinlan-dq-development-dev"
  export ROSE_BRANCH_PATH_PART=$(echo ${ROSE_BRANCH_NAME_GIT} | tr '/' '-')
  
  export ROSE_LATEST_VERSION_FILE="${ROSE_PROJECT_BASE}/VERSION_OF_LATEST_WORKSPACE"

  # Non-compiler version info:
  export ROSE_MPI_KIND="mvapich2"
  export ROSE_MPI_VERSION="2.2"
  export BOOST_VERSION="1.66.0"
  print_rose_vars
}

# Internal - to reduce code duplication:
set_ROSE_REPO_PATH_VERSIONED () {
  export ROSE_REPO_PATH_VERSIONED="${ROSE_PROJECT_BASE}/${ROSE_REPO_NAME}-${ROSE_BRANCH_PATH_PART}-${ROSE_VERSION}"
}

clone_latest_workspace () {
  cd ${ROSE_PROJECT_BASE}
  run_or_not git clone ${ROSE_REMOTE_REPO}
  run_or_not cd ${ROSE_REPO_PATH_CLONED}
  # Check out the desired branch as needed:
  run_or_not git checkout ${ROSE_BRANCH_NAME_GIT}
  run_or_not git submodule init
  run_or_not git submodule update
  if [ -f ROSE_VERSION ]
  then
    export ROSE_VERSION=`cat ROSE_VERSION`
  else
    export ROSE_VERSION="unknown_version"
  fi
  set_ROSE_REPO_PATH_VERSIONED
  run_or_not mv ${ROSE_REPO_PATH_CLONED} ${ROSE_REPO_PATH_VERSIONED}
}

use_existing_workspace () {
  # One parameter - the ROSE version
  export ROSE_VERSION=$1
  set_ROSE_REPO_PATH_VERSIONED
}

use_latest_existing_workspace () {
  use_existing_workspace `cat ${ROSE_LATEST_VERSION_FILE}`
}

do_preconfigure_no_latest_copy () {
  run_or_not cd ${ROSE_REPO_PATH_VERSIONED}
  run_or_not ${SRUN_DO} ${RUN_AND_LOG} ./build
}

do_preconfigure () {
  do_preconfigure_no_latest_copy
  run_or_not cp ${ROSE_REPO_PATH_VERSIONED}/ROSE_VERSION ${ROSE_LATEST_VERSION_FILE}
}

#=====================================
# FOR INTEL 18.0.1 MPI (Used by ARES):
#=====================================
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
  module load gcc/6.1.0
  pop_set_state
  #---------------------------------------

  export ROSE_COMPILER_PATH_PART="${ROSE_COMPILER_VERSIONED}.mpi-${ROSE_BACKEND_COMPILER_VERSIONED}.mpi"
  export ROSE_BUILD_PATH="${ROSE_REPO_PATH_VERSIONED}-${ROSE_COMPILER_PATH_PART}"
}
#=====================================

#=====================================
# FOR INTEL 18.0.2 non-MPI (Used by BLAST):
#=====================================
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
  module load gcc/6.1.0
  pop_set_state
  #---------------------------------------

  export ROSE_COMPILER_PATH_PART="${ROSE_COMPILER_VERSIONED}-${ROSE_BACKEND_COMPILER_VERSIONED}"
  export ROSE_BUILD_PATH="${ROSE_REPO_PATH_VERSIONED}-${ROSE_COMPILER_PATH_PART}"
}
#=====================================

#======================================
# FOR GCC 4.9.3 or 6.1.0 non-MPI (used by Kull):
#======================================
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

  export ROSE_COMPILER_PATH_PART="${ROSE_COMPILER_VERSIONED}"
  export ROSE_BUILD_PATH="${ROSE_REPO_PATH_VERSIONED}-${ROSE_COMPILER_PATH_PART}"
}
#======================================

# Run after setup_xxx_compiler:
setup_boost () {
  # SELECT BOOST:
  export ROSE_BOOST_HOME="/usr/tce/packages/boost/boost-${BOOST_VERSION}-${ROSE_MPI_KIND}-${ROSE_MPI_VERSION}-${ROSE_COMPILER_VERSIONED}"
  print_rose_vars
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
  run_or_not ${SRUN_DO} ${RUN_AND_LOG} \
  "${ROSE_REPO_PATH_VERSIONED}/configure" \
  "--without-java" \
  "--with-boost=${ROSE_BOOST_HOME}" \
  "--disable-binary-analysis" \
  "--disable-boost-version-check" \
  "--enable-edg_version=5.0" \
  "--with-alternate_backend_Cxx_compiler=${ROSE_BACKEND_COMPILER_HOME}/bin/mpicxx" \
  "--with-alternate_backend_C_compiler=${ROSE_BACKEND_COMPILER_HOME}/bin/mpicc" \
  "--with-alternate_backend_fortran_compiler=${ROSE_BACKEND_COMPILER_HOME}/bin/mpif77" 

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
do_gcc_configure () {
  run_or_not mkdir -p ${ROSE_BUILD_PATH}
  run_or_not cd ${ROSE_BUILD_PATH}
  module load ${ROSE_COMPILER}/${ROSE_COMPILER_VERSION}
  run_or_not ${SRUN_DO} \
  ${RUN_AND_LOG} \
  ${ROSE_REPO_PATH_VERSIONED}/configure \
  --without-java \
  --with-boost=${ROSE_BOOST_HOME} \
  --disable-boost-version-check \
  --enable-edg_version=5.0
  
 # Future:
 # --enable-lang (e.g. C)
 # --with-boost=${ROSE_BOOST_HOME} \
 # no --without-java
 # no --disable-boost-version-check
 # no --enable-edg_version=5.0
 # 
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

