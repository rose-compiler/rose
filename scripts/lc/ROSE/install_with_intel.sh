#!/bin/bash
set -x
# Clone and build ROSE 0.9.10.XX with intel 16.0.3 or 18.0.1:

# Coudn't find ${HOME}/bin/srun_do starting 15 Apr 2019.  Run user setup first:
# .profile calls .bashrc:
source ${HOME}/.profile
echo "======================================================"
echo "PATH"
echo $PATH
echo "======================================================"

set_main_vars () {
  export ROSE_PROJECT_BASE="${HOME}/code/ROSE"
  # The LLNL ROSE git project and repo:
  export ROSE_REMOTE_PROJECT="rose-dev@rosecompiler1.llnl.gov:rose/scratch"
  export ROSE_REPO_NAME="rose"
  #export ROSE_BRANCH_NAME_GIT="rasmussen17/jovial-directives"
  #export ROSE_BRANCH_NAME_GIT="rose/reynolds12/automation"
  export ROSE_BRANCH_NAME_GIT="master"

  # e.g. "rose-dev@rosecompiler1.llnl.gov:rose/scratch/rose":
  export ROSE_REMOTE_REPO="${ROSE_REMOTE_PROJECT}/${ROSE_REPO_NAME}"
  # e.g. "${HOME}/code/ROSE/rose":
  export ROSE_REPO_PATH_CLONED=${ROSE_PROJECT_BASE}/${ROSE_REPO_NAME}
  # Take out any slashes:
  # e.g. "dquinlan/dq-development-dev" -> "rose-dquinlan-dq-development-dev"
  export ROSE_BRANCH_PATH_PART=$(echo ${ROSE_BRANCH_NAME_GIT} | tr '/' '-')

  # Non-compiler version info:
  export MPI_KIND="mvapich2"
  export MPI_VERSION="2.2"
  export BOOST_VERSION="1.66.0"
}

#===============
# FOR NEW CLONE:
#===============
get_new_clone () {
  cd ${ROSE_PROJECT_BASE}
  git clone ${ROSE_REMOTE_REPO}
  cd ${ROSE_REPO_PATH_CLONED}
  # Check out the desired branch if needed:
  git checkout ${ROSE_BRANCH_NAME_GIT}
  git submodule init
  git submodule update
  export ROSE_VERSION=`cat ROSE_VERSION`
  export ROSE_BRANCH_VERSION="${ROSE_BRANCH_PATH_PART}-${ROSE_VERSION}"
  export ROSE_REPO_PATH_VERSIONED="${ROSE_REPO_PATH_CLONED}-${ROSE_BRANCH_VERSION}"
  mv ${ROSE_REPO_PATH_CLONED} ${ROSE_REPO_PATH_VERSIONED}
  cd ${ROSE_REPO_PATH_VERSIONED}
  srun_do run_and_log build
}

#========================
# FOR EXISTING CLONE:
#========================
use_existing_clone () {
  export ROSE_VERSION="0.9.10.216"
  export ROSE_BRANCH_VERSION="${ROSE_BRANCH_PATH_PART}-${ROSE_VERSION}"
  export ROSE_REPO_PATH_VERSIONED="${ROSE_REPO_PATH_CLONED}-${ROSE_BRANCH_VERSION}"
}
#========================

#=====================================
# FOR INTEL 18.0.1 MPI (Used by ARES):
#=====================================
setup_compiler () {
  export ROSE_COMPILER="intel"
  export ROSE_COMPILER_VERSION="18.0.1"
  export ROSE_COMPILER_VERSIONED="${ROSE_COMPILER}-${ROSE_COMPILER_VERSION}"
  export ROSE_COMPILER_HOME="/usr/tce/packages/${MPI_KIND}/${MPI_KIND}-${MPI_VERSION}-${ROSE_COMPILER_VERSIONED}"
  export  CC="${ROSE_COMPILER_HOME}/bin/mpicc"
  export CXX="${ROSE_COMPILER_HOME}/bin/mpic++"

  export BACKEND_COMPILER="intel"
  export BACKEND_COMPILER_VERSION="18.0.1"
  export BACKEND_COMPILER_VERSIONED="${BACKEND_COMPILER}-${BACKEND_COMPILER_VERSION}"
  export BACKEND_COMPILER_HOME="/usr/tce/packages/${MPI_KIND}/${MPI_KIND}-${MPI_VERSION}-${BACKEND_COMPILER_VERSIONED}"

  #---------------------------------------
  # Run the Intel setup script so we don't have to add 
  # "-L/usr/tce/packages/intel/intel-18.0.1/lib/intel64" to line 275 of 
  # "${ROSE_REPO_PATH_VERSIONED}/config/Makefile.for.ROSE.includes.and.libs" to avoid
  # link problems:
  export NON_MPI_COMPILER_HOME="/usr/tce/packages/${ROSE_COMPILER}/${ROSE_COMPILER_VERSIONED}"
  export COMPILERVARS_ARCHITECTURE=intel64
  export COMPILERVARS_PLATFORM=linux
  set +ux
  source ${NON_MPI_COMPILER_HOME}/bin/compilervars.sh
  # Yes, you need gcc 4.9.3 or later for intel:
  module load gcc/4.9.3
  set -ux
  #---------------------------------------

  export ROSE_BUILD_PATH="${ROSE_REPO_PATH_VERSIONED}-${ROSE_COMPILER_VERSIONED}.mpi-${BACKEND_COMPILER_VERSIONED}.mpi"
  mkdir ${ROSE_BUILD_PATH}
}
#======================================

setup_boost () {
  # SELECT BOOST:
  export BOOST_HOME="/usr/tce/packages/boost/boost-${BOOST_VERSION}-${MPI_KIND}-${MPI_VERSION}-${ROSE_COMPILER_VERSIONED}"
}


#=====================================
# FOR INTEL 18.0.1 MPI (Used by ARES):
#=====================================

do_configure () {
  cd ${ROSE_BUILD_PATH}
  echo "------------------------"
  echo "NEEDED: LD_LIBRARY_PATH does not have any non-gcc compiler paths in it."
  echo "LD_LIBRARY_PATH=${LD_LIBRARY_PATH}"
  echo "------------------------"
  echo "DO NOT run multiple configures using ${ROSE_REPO_PATH_VERSIONED} in parallel if they need different intel versions."
  echo "------------------------"
  echo "DOING configure"
  srun_do run_and_log \
  "${ROSE_REPO_PATH_VERSIONED}/configure" \
  "--without-java" \
  "--with-boost=${BOOST_HOME}" \
  "--disable-binary-analysis" \
  "--disable-boost-version-check" \
  "--enable-edg_version=5.0" \
  "--with-alternate_backend_Cxx_compiler=${BACKEND_COMPILER_HOME}/bin/mpicxx" \
  "--with-alternate_backend_C_compiler=${BACKEND_COMPILER_HOME}/bin/mpicc" \
  "--with-alternate_backend_fortran_compiler=${BACKEND_COMPILER_HOME}/bin/mpif77" 

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
#======================================

#======================================
# Make and install ROSE:
#======================================
make_and_install () {
  cd ${ROSE_BUILD_PATH}
  srun_do -c36 run_and_log --use_first_parm make all -j36
  srun_do -c36 run_and_log --use_first_parm make install -j36
  # Install the comp db scripts in prefix/bin (currently ${ROSE_BUILD_PATH}/bin):
  cd projects/CompilationDB
  run_and_log --use_first_parm make install
}
#======================================

set_main_vars

get_new_clone

#use_existing_clone

setup_compiler
setup_boost
do_configure
make_and_install

