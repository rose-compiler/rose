#!/bin/echo "ERROR: Do not execute this script! Source it from bash:"

define_component_variables () {
  # Most likely to change:
  export ROSE_VERSION=${ROSE_VERSION:-"0.9.11.115"}
  export ROSE_BRANCH_NAME="master"
 
  export CODE_BASE="${HOME}/code"
#  export ROSE_PROJECT_BASE="${CODE_BASE}/ROSE"
#  export ROSE_PROJECT_BASE="/usr/workspace/wsrzd/charles/bamboo/ROSEARES-rzgenie2-charles-1/xml-data/build-dir/ROSE-ROSE-CLONEPRE"
  export ROSE_PROJECT_BASE="/collab/usr/global/tools/rose/toss_3_x86_64_ib"
  export ROSE_REPO_NAME="rose"
  export ROSE_REPO_PATH_CLONED="${ROSE_PROJECT_BASE}/${ROSE_REPO_NAME}"
  # Take out any slashes:
  export ROSE_BRANCH_PATH_PART=$(echo ${ROSE_BRANCH_NAME} | tr '/' '-')
  export ROSE_BRANCH_VERSION="${ROSE_BRANCH_PATH_PART}-${ROSE_VERSION}"
  export ROSE_REPO_PATH_VERSIONED="${ROSE_REPO_PATH_CLONED}-${ROSE_BRANCH_VERSION}"

  # Where most of the compilers live:
  export PACKAGES_BASE="/usr/tce/packages"

  export MPI_KIND="mvapich2"
  export MPI_VERSION="2.2"

  export GCC_COMPILER="gcc"
  export GCC_COMPILER_VERSION="4.9.3"
  export GCC_COMPILER_VERSIONED="${GCC_COMPILER}-${GCC_COMPILER_VERSION}"
  # For locations like "/usr/tce/packages/gcc/gcc-6.1.0":
  export GCC_COMPILER_HOME="${PACKAGES_BASE}/${GCC_COMPILER}/${GCC_COMPILER_VERSIONED}"

  export INTEL_COMPILER="intel"
  export INTEL_COMPILER_VERSION="18.0.1"
  export INTEL_COMPILER_VERSIONED="${INTEL_COMPILER}-${INTEL_COMPILER_VERSION}"
  # for locations like: "/usr/tce/packages/mvapich2/mvapich2-2.2-intel-18.0.1"
  export INTEL_MPI_COMPILER_HOME="${PACKAGES_BASE}/${MPI_KIND}/${MPI_KIND}-${MPI_VERSION}-${INTEL_COMPILER_VERSIONED}"
}

define_desired_export_vars () {
  export ROSE_HOME=${ROSE_BUILD_PATH}

  export ROSE_LD_LIBRARY_PATH=\
"${ROSE_HOME}/src/.libs":\
"${ROSE_HOME}/src/3rdPartyLibraries/libharu-2.1.0/src/.libs"

  # For things like projects/CompilationDB/scripts which are not filled by 
  # make; make install:
  export ROSE_REPO=${ROSE_REPO_PATH_VERSIONED}
}
