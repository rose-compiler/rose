# SOURCE this from a bourne or bash shell.
# Sets ROSE_HOME, ROSE_LD_LIBRARY_PATH and associated variables

export ROSE_PROJECT_BASE="/g/g17/charles/code/ROSE"
export ROSE_REPO_NAME="rose"
export ROSE_BRANCH_NAME="dquinlan/dq-development-dev"
export ROSE_VERSION="0.9.10.150"

export ROSE_REPO_PATH_CLONED=${ROSE_PROJECT_BASE}/${ROSE_REPO_NAME}
# Take out any slashes:
export ROSE_BRANCH_PATH_PART=$(echo ${ROSE_BRANCH_NAME} | tr '/' '-')
export ROSE_BRANCH_VERSION="${ROSE_BRANCH_PATH_PART}-${ROSE_VERSION}"
export ROSE_REPO_PATH_VERSIONED="${ROSE_REPO_PATH_CLONED}-${ROSE_BRANCH_VERSION}"

#=====================================
# FOR INTEL 18.0.1 MPI (Used by ARES):
#=====================================
export ROSE_COMPILER="intel"
export ROSE_COMPILER_VERSION="18.0.1"
export ROSE_COMPILER_VERSIONED="${ROSE_COMPILER}-${ROSE_COMPILER_VERSION}"

export BACKEND_COMPILER="intel"
export BACKEND_COMPILER_VERSION="18.0.1"
export BACKEND_COMPILER_VERSIONED="${BACKEND_COMPILER}-${BACKEND_COMPILER_VERSION}"

export ROSE_BUILD_PATH="${ROSE_REPO_PATH_VERSIONED}-${ROSE_COMPILER_VERSIONED}.mpi-${BACKEND_COMPILER_VERSIONED}.mpi"
#=====================================

export ROSE_HOME=${ROSE_BUILD_PATH}

export ROSE_LD_LIBRARY_PATH=\
"${ROSE_HOME}/src/.libs":\
"${ROSE_HOME}/src/3rdPartyLibraries/libharu-2.1.0/src/.libs"

# For things like projects/CompilationDB/scripts which are not filled by
# make; make install:
export ROSE_REPO=${ROSE_REPO_PATH_VERSIONED}

