#!/bin/bash
#---------------------------------
# A fully automated deployment process of rose 
# Assumption: all dependent software packages have been installed already

# Directory layout
# ./this-script
#
# ./rose
#  | _ timestamps
#        |
#         --- sourcetree
#        |
#         --- buildDebug or buildOpt
#        |
#         --- installDebug or installOpt
#
# Liao, 2019/3/25

#-------------------------------------------------------------
# Set variables
function configure_variables()
{
  #variables should use default values. Only modify them if really needed. 
  #---------environment settings ----------------
  # where to find required software packages
  #module load intel/18.0.1
  # Only a limited places to look for boost installation

# On tux385   
 if [ -d "/nfs/casc/overture/ROSE/opt/rhel7/x86_64/boost/1_60_0/gcc/4.9.3" ] ; then
    BOOST_INSTALL_PATH=/nfs/casc/overture/ROSE/opt/rhel7/x86_64/boost/1_60_0/gcc/4.9.3
    JAVA_INSTALL_PATH=/nfs/casc/overture/ROSE/opt/rhel7/x86_64/java/jdk/1.8.0_131
# On LC system     
 elif  [ -d "/g/g17/liao6/opt/boost_1_55_0_inst" ] ; then   
    BOOST_INSTALL_PATH=/g/g17/liao6/opt/boost_1_55_0_inst
    JAVA_INSTALL_PATH=/g/g17/liao6/opt/jdk1.8.0_201
 fi
  

# sanity check of boost and java
  if [ ! -d ${BOOST_INSTALL_PATH} ] ; then
     echo "Specified ${BOOST_INSTALL_PATH} does not exist. aborting..."
     exit 1
  fi

  if [ ! -d ${JAVA_INSTALL_PATH} ] ; then
     echo "Specified ${JAVA_INSTALL_PATH} does not exist. aborting..."
     exit 1
  fi

  #--------------paths to store/find relevant software components
  
  # where to clone ROSE
  ROSE_REMOTE_GIT_PATH=rose-dev@rosecompiler1.llnl.gov:rose/scratch/rose
  
  # top level workspace to store everything
  TOP_WORKSPACE="`pwd`"
  
  #----------------ROSE related paths and settings 
  # top rose path to host three subdirectories: source, build and installation paths
# latest timestamp's top level path within rose
  ROSE_DEPLOY=${TOP_WORKSPACE}/rose/${ROSE_TIMESTAMP}

  ROSE_REPO=${ROSE_DEPLOY}/sourcetree

  ROSE_BUILD_DEBUG=${ROSE_DEPLOY}/buildDebug
  ROSE_INSTALL_DEBUG=${ROSE_DEPLOY}/installDebug

# not really used for now
  ROSE_INSTALL_OPT=${ROSE_DEPLOY}/installOpt
  ROSE_BUILD_OPT=${ROSE_DEPLOY}/buildOpt
  
  # we need the same intel compiler used to build ARES
#  MVAPICH_INTEL=/usr/tce/packages/mvapich2/mvapich2-2.2-intel-18.0.1
#  ROSE_CC=${MVAPICH_INTEL}/bin/mpicc
#  ROSE_CXX=${MVAPICH_INTEL}/bin/mpicxx
#  ALTERNATE_COMPILER_PATH=${MVAPICH_INTEL}/bin

}

#-------------------------------------------------------------
#------------- check out a lightweight rose and grab its timestamp
function get_rose_remote_timestamp()
{
  set -e
  rm -rf temp_light_repo
  mkdir temp_light_repo
  cd temp_light_repo
  git clone -b master --single-branch rose-dev@rosecompiler1.llnl.gov:rose/scratch/rose --depth=3 
  cd rose
  ROSE_TIMESTAMP=`git log -1 --pretty=format:%ci |sed -e 's/ /_/g' | sed -e 's/:/-/g'`
  echo $ROSE_TIMESTAMP
  cd ..
  cd ..
  set +e
  TEMP_LIGHT_REPO=temp_light_repo/rose
#  rm -rf temp_light_repo
}

#----------------build the latest rose----------------
function build_rose()
{

#sanity check for some environment variables
  if [ "x${ROSE_DEPLOY}" == "x" ] ||[ "x${ROSE_REPO}" == "x" ] || [ "x${ROSE_INSTALL_DEBUG}" == "x" ] || [ "x${ROSE_BUILD_DEBUG}" == "x" ] ; then
    echo "Fatal error: one of ROSE_* variables is not set..aborting ..."
    echo "this script should be called by ../run.sh, in which configureVariables.sh is called to set up essential variables"
    exit 1
  fi

#  export CC=${ROSE_CC}
#  export CXX=${ROSE_CXX}

#------------------ check out rose repo if it does not exit----------------
  if [ ! -d "$ROSE_REPO" ]; then
# Control will enter here if $DIRECTORY doesn't exist.
    echo "Expecting rose repo directory $ROSE_REPO but cannot find it. Trying to create one .."

    # create the deployment path if needed
    if [ ! -d "$ROSE_DEPLOY" ]; then
        mkdir -p $ROSE_DEPLOY
    fi 

    cd $ROSE_DEPLOY
    #we clone a new one
    git clone ${ROSE_REMOTE_GIT_PATH} sourcetree

    if [ ! -d "$ROSE_REPO" ]; then
      echo "Fatal error: ROSE repo does not exist after git clone ..sourcetree. Aborting ..."
      echo "The path of the repo should be: $ROSE_REPO"
      exit 1
    fi  

    cd sourcetree
    git submodule init
    git submodule update
  fi

#------------------ update the local repo if it is behind the remote repo
# we have a local repo
  cd $ROSE_REPO

# Note: this check no longer is necessary since we create deployment path based on latest commit already
# but we keep it here just in case
# check if we need to rebuild ROSE
  git remote update origin
  LOCALHASH=`git rev-parse HEAD`
  REMOTEHASH=`git rev-parse origin/HEAD`
# return 0 if finds "behind", 1 if it does not find it
  if [ "$LOCALHASH" != "$REMOTEHASH" ]; then
        echo "local repo is different from remote master, updating it.."
# undo possible changes to Makefile
#        git checkout -- config/Makefile.for.ROSE.includes.and.libs
        git pull origin master
        git submodule update
  else
        echo "local repo is up-to-date. " 
  fi

# Note: this check no longer is necessary since we create deployment path based on latest commit already
#---------------- check if installed rose is behind the latest local repo
# refresh local hash gain, after possible sync.
  cd $ROSE_REPO
  LOCALHASH=`git rev-parse HEAD`
  ROSE_LOCAL_GIT_HASH=`git rev-parse --short HEAD`

# if the translator exists 
  INSTALLHASH=xxxxxx
  if [ -f "${ROSE_INSTALL_DEBUG}/bin/identityTranslator" ]; then
     INSTALLHASH=`${ROSE_INSTALL_DEBUG}/bin/identityTranslator --version | grep "ID:" | cut -f 2 -d':' | sed -e 's/^[[:space:]]*//'`

    if [ "$INSTALLHASH" == "$LOCALHASH" ]; then
       echo "Installed rose is up-to-date: same hash as sync.ed local repo's hash. Nothing to do" 
       return
    fi
  
    echo "Installed rose is different from the sync.ed local repo's hash. Recomipling/installing it .." 
  else  
    echo "Cannot find previously installed rose. Comipling/installing it .." 
  fi

  cd $ROSE_REPO

# tweak one file within the repo
#  sed -i 's/^ROSE_INTEL_COMPILER_MATH_LIBS = .*/ROSE_INTEL_COMPILER_MATH_LIBS = -L\/usr\/tce\/packages\/intel\/intel-18\.0\.1\/lib\/intel64 -limf -lirng -lintlc -lsvml/' ${ROSE_REPO}/config/Makefile.for.ROSE.includes.and.libs

  ./build

# build deployment version in the buildOpt directory
  if [ ! -d "$ROSE_BUILD_DEBUG" ]; then
     mkdir -p $ROSE_BUILD_DEBUG
  fi 

  if [ ! -d "$ROSE_INSTALL_DEBUG" ]; then
    mkdir -p $ROSE_INSTALL_DEBUG
  fi

  cd $ROSE_BUILD_DEBUG

# --with-alternate_backend_Cxx_compiler=${ALTERNATE_COMPILER_PATH}/mpicxx --with-alternate_backend_C_compiler=${ALTERNATE_COMPILER_PATH}/mpicc --with-alternate_backend_fortran_compiler=${ALTERNATE_COMPILER_PATH}/mpif77

  ../sourcetree/configure --with-java=${JAVA_INSTALL_PATH} --with-boost=${BOOST_INSTALL_PATH} --with-CXX_DEBUG=-g --with-C_OPTIMIZE=-O0 --with-CXX_OPTIMIZE=-O0 --enable-languages=c++,fortran --disable-boost-version-check --enable-edg_version=5.0 --prefix=${ROSE_INSTALL_DEBUG}

# build and install ROSE tools
  echo "Start to build and install rose core"
  echo `date`  

# if srun exist, we use srun 
  command -v srun >/dev/null 2>&1

  if [ $? -ne 0 ]; then
      make install-core -j
  else
      srun -n 1 --exclusive make install-core -j
  fi

  echo "End of building rose core"
  echo `date`  

#-------------- double check installed rose is the latest version-------------
# check again: the installed rose should be up-to date
   if [ -f "${ROSE_INSTALL_DEBUG}/bin/identityTranslator" ]; then
      INSTALLHASH=`${ROSE_INSTALL_DEBUG}/bin/identityTranslator --version | grep "ID:" | cut -f 2 -d':' | sed -e 's/^[[:space:]]*//'`
   else
      echo "Fatal error: ${ROSE_INSTALL_DEBUG}/bin/identityTranslator does not exist after installing rose! Aborting .."
      exit 1
   fi

   if [ "$INSTALLHASH" == "$REMOTEHASH" ]; then
          echo "installed rose is up-to-date: same hash as remote repo's. Everything looks fine" 
          return
   else
          echo "Fatal error: installed rose's has is different from the remote repo's has. Something is wrong!"
          exit 1
   fi

}

#=========================call the functions ============
# redirect both stdout and stderr into a file
exec 6>&1
exec 7>&2
exec &>log-$(date +%Y%m%d_%H%M%S).txt


echo "Start of this script..."
date

# step 1
get_rose_remote_timestamp

# step 2
# configure environment variables
configure_variables

# step 3
# actually build rose
build_rose

echo "End of this script..."
date

exec 1>&6 6>&- # restore 1 and cancel 6
exec 2>&7 7>&- # restore 2 and cancel 7

