#!/bin/bash

#######################################################################
###  SCRIPT TO TEST EXECUTION OF TRANSLATED POP                     ###
###  Scott K Warren, Rice University                                ###
###  April 2, 2011                                                  ###
#######################################################################



#######################################################################
#  CONFIGURATION PARAMETERS                                           #
#######################################################################

# environment variables which must be set for test configuration (example values)
# export HUDSON_NUM_JOBS=1
# export HUDSON_ROSE="/home/scott/test/ROSE"
# export HUDSON_FC="/usr/bin/gfortran"
# export HUDSON_FFLAGS="-ffree-line-length-0"
# export HUDSON_LD="/usr/bin/gfortran"
# export HUDSON_CC="/usr/bin/cc"
# export HUDSON_Cp="/bin/cp"
# export HUDSON_Cpp="/lib/cpp -P"
# export HUDSON_AWK="/usr/bin/gawk"
# export HUDSON_LDLIBS="\$(LIBS) -lcurl"

# rose tools
export ROSE_INSTALL=/home/hudson-rose/opt/roseLatest
export TRANSLATOR=$ROSE_INSTALL/bin/testTranslator
#export TRANSLATOR=$HUDSON_ROSE/install/bin/testTranslator

# unix tools
export DIFF="/usr/bin/diff"
export MAKE="/usr/bin/make"
export SED="/bin/sed"


#######################################################################
#  DEFINITIONS                                                        #
#######################################################################

# subdirectories
export POPTEST=`pwd`
export POP_DISTRO="./pop-distro"           # build tree for POP as distributed
export POP_TRANSLATED="./pop-translated"   # build tree for freshly translated POP
export NETCDF="netcdf-4.1.1"               # build tree for NetCDF (prerequisite for POP)


#######################################################################
#  FUNCTIONS                                                          #
#######################################################################


#---------------------------------------------------------------------#
# run a POP execution test with given configuration parameters        #
#---------------------------------------------------------------------#

function test
{  
  build_test
  run_test
  return $?
}


#---------------------------------------------------------------------#
# configure and build the POP execution test code                     #
#---------------------------------------------------------------------#

function build_test
{  
  configure
  build_prerequisites
  build $POP_DISTRO
  run $POP_DISTRO
}


#---------------------------------------------------------------------#
# run a POP execution test, given the built test code                 #
#---------------------------------------------------------------------#

function run_test
{  
  translate $POP_DISTRO $POP_TRANSLATED
  build $POP_TRANSLATED
  run $POP_TRANSLATED
  compare $POP_DISTRO/run/answers.txt $POP_TRANSLATED/run/answers.txt
  return $?
}


#---------------------------------------------------------------------#
# generate POP config file from Hudson-provided arguments             #
#---------------------------------------------------------------------#

function configure
{
  # extract POP source trees
  tar xzf pop-distro.tgz
  cp -r pop-distro pop-translated

  # substitute Hudson-specified parameters into template
  $SED \
    -e "s#_HUDSON_FC_#$HUDSON_FC#"                      \
    -e "s#_HUDSON_FFLAGS_#$HUDSON_FFLAGS#"              \
    -e "s#_HUDSON_LD_#$HUDSON_LD#"                      \
    -e "s#_HUDSON_CC_#$HUDSON_CC#"                      \
    -e "s#_HUDSON_Cp_#$HUDSON_Cp#"                      \
    -e "s#_HUDSON_Cpp_#$HUDSON_Cpp#"                    \
    -e "s#_HUDSON_AWK_#$HUDSON_AWK#"                    \
    -e "s#_HUDSON_LDLIBS_#$HUDSON_LDLIBS#"              \
    < hudson-template.gnu                               \
	> temp.gnu
  cp temp.gnu $POP_DISTRO/input_templates/hudson.gnu
  cp temp.gnu $POP_TRANSLATED/input_templates/hudson.gnu
  rm temp.gnu

  # configure POP
  (cd pop-distro;     ./setup_run_dir run)
  (cd pop-translated; ./setup_run_dir run)	
}


#---------------------------------------------------------------------#
# build prerequisites for POP                                         #
#---------------------------------------------------------------------#

function build_prerequisites
{
  # netcdf
  tar xzf $NETCDF.tgz
  pushd $NETCDF
  absolute_netcdf_install=`mkdir install; cd install; pwd`
  ./configure --prefix=$absolute_netcdf_install --disable-netcdf-4 FC=$HUDSON_FC CC=$HUDSON_CC CXX=$HUDSON_CXX
#  $MAKE -j $HUDSON_NUM_JOBS check
  $MAKE -j $HUDSON_NUM_JOBS all
  $MAKE -j $HUDSON_NUM_JOBS install
  popd
}


#---------------------------------------------------------------------#
# compile given POP build tree using generated config file            #
#---------------------------------------------------------------------#

function build
{
  # arguments
  path=$1

  # do a clean make
  pushd $path/run
  export ARCHDIR=hudson
  $MAKE -j $HUDSON_NUM_JOBS
  popd
}


#---------------------------------------------------------------------#
# execute given POP and preserve answer file in its build tree        #
#---------------------------------------------------------------------#

function run
{
  # arguments
  path=$1

  # run POP
  pushd $path/run
  ./pop > answers.txt
  popd
}


#---------------------------------------------------------------------#
# translate POP using 'testTranslator' into a given build tree        #
#---------------------------------------------------------------------#

function translate
{
  ./translate-pop.sh $POP_DISTRO $POP_TRANSLATED
}


#---------------------------------------------------------------------#
# compare two answer files, filtering out lines that always vary      #
#---------------------------------------------------------------------#

function compare
{
  # filter out variable content of answers
  filter $POP_DISTRO/run/answers.txt     > distro-answers.txt
  filter $POP_TRANSLATED/run/answers.txt > translated-answers.txt
  
  # compare filtered answers
  $DIFF distro-answers.txt translated-answers.txt
  rc=$?
  
  echo "** DONE COMPARE **"  

  return $rc
}


#---------------------------------------------------------------------#
# filter an answer file to stdout, removing variable lines            #
#---------------------------------------------------------------------#

function filter
{
  # arguments
  path=$1

  # deleting text containing timer values, which vary
  $SED \
    -e "s#Timer number.* seconds##"                        \
    -e "s#Timer stats.* seconds##"                         \
    -e "s#max = .* seconds##"                              \
    -e "s#mean= .* seconds##"                              \
    < $path
}


#######################################################################
#  SCRIPT TOP LEVEL                                                   #
#######################################################################

test; rc=$?

if [ $? -eq 0 ]; then
  echo "******* POP EXECUTION TEST PASSED *********"
else
  echo "******* POP EXECUTION TEST FAILED *********"
fi

exit $rc

