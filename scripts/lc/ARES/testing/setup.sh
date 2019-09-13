export ROSE_VERSION="rose-0.9.9.83"
export ROSE_HOME="/collab/usr/global/tools/rose/$SYS_TYPE/rose-0.9.9.83-1742d17-gcc"


# TODO Remove after release
export PATH="${ROSE_HOME}/bin:${PATH}"

#------------------------------------------------------------------------------
# Load Spack into shell environment
#------------------------------------------------------------------------------
export SPACK_HOME="/collab/usr/global/tools/rose/release/.spack"
export PATH="${SPACK_HOME}/bin:${PATH}"

#------------------------------------------------------------------------------
# Load Spack's shell support
#------------------------------------------------------------------------------
source "${SPACK_HOME}/share/spack/setup-env.sh"

#------------------------------------------------------------------------------
# Load Intel compiler into shell environment
#------------------------------------------------------------------------------
: . /usr/local/tools/dotkit/init.sh
use mvapich2-intel-1.9 >/dev/null 

#------------------------------------------------------------------------------
# Load ROSE into shell environment
#------------------------------------------------------------------------------
spack load rose@${ROSE_VERSION} || false

#------------------------------------------------------------------------------
# Display ROSE Backend Compiler
#------------------------------------------------------------------------------
cat <<-EOF
$(identityTranslator --version)
  --- using ROSE_HOME: '${ROSE_HOME}'
EOF
