#!/bin/bash
#
# Validate ./configure lines to maintain configure correctness
#
#

# ROSE_TEST_BOOST_ROOT

ROSE_DIR="`pwd`/.."
CONFIGURE="${ROSE_DIR}/configure --with-boost=${ROSE_TEST_BOOST_ROOT}"
TESTDIR=/tmp/ROSE_test_configure_language_support
LOG="config.output"
LOG_OUT="language_support.output"
CORRECT_OUT="language_support.output.correct"
#########################################################################################
# Create a sandbox to test configure lines
mkdir -p ${TESTDIR} 
cd ${TESTDIR} 
echo "Running configure tests in sandbox: ${TESTDIR}"
#########################################################################################

# won't output
#`${CONFIGURE} --enable-languages=c`
#`${CONFIGURE} --enable-languages=c`

# $ sed -n 's/.*\([+-] .*\)/\1/p'
# $ sed -n '/checking user-specified languages to support/,/disabled/p'
# checking user-specified languages to support... 
#
#  -  Binary-analysis
#  +  C
#  -  C++
#  -  Cuda
#  -  Fortran
#  -  Java
#  -  PHP
#  -  OpenCL
#
# (+)enabled (-)disabled
#
# $ sed -n '/checking user-specified languages to support/,/disabled/p' | sed -n 's/.*\([+-] .*\)/\1/p' 
#
# Example:
# $ sed -n '/checking user-specified languages to support/,/disabled/p' /tmp/config.log | sed -n 's/.*\([+-] .*\)/\1/p'
#-  Binary-analysis
#-  C++
#-  Cuda
#-  Fortran
#-  Java
#-  PHP
#-  OpenCL

# test_language_options <language_options>
function run_configure {
 cmd="${CONFIGURE} $*"
 echo "Executing ${cmd}"
 $cmd 2>/dev/null > $LOG
}

function test_configure {
#replaces newline: sed '{:q;N;s/\n/ /g;t q}'  
 sed -n '/checking user-specified languages to support/,/disabled/p' $LOG | sed -n 's/.*\([-+] .*\)/\1/p' | sed '{:q;N;s/\n/ /g;t q}' > $LOG_OUT
 cat $LOG_OUT
 echo "-------------------------"
 echo -e $1 > $CORRECT_OUT
 cat $CORRECT_OUT
 diff -w $LOG_OUT $CORRECT_OUT
 if test $? != 0 ; then
  echo "ERROR: language support configuration is broken"
  exit $?
 fi
 echo ""
}

#########################################################################################
run_configure --enable-binary-analysis
#########################################################################################
test_configure "\
  + Binary analysis \
  + C \
  + C++ \
  + Cuda \
  + Fortran \
  + Java \
  + PHP \
  + OpenCL"

#########################################################################################
run_configure -enable-binary-analysis=yes
#########################################################################################
test_configure "\
  + Binary analysis \
  + C \
  + C++ \
  + Cuda \
  + Fortran \
  + Java \
  + PHP \
  + OpenCL"

#########################################################################################
run_configure --enable-binary-analysis=no
#########################################################################################
test_configure "\
  - Binary analysis \
  + C \
  + C++ \
  + Cuda \
  + Fortran \
  + Java \
  + PHP \
  + OpenCL"

#########################################################################################
run_configure --disable-binary-analysis
#########################################################################################
test_configure "\
  - Binary analysis \
  + C \
  + C++ \
  + Cuda \
  + Fortran \
  + Java \
  + PHP \
  + OpenCL"

#########################################################################################
run_configure --enable-languages=c --enable-binary-analysis
#########################################################################################
test_configure "\
  + Binary analysis \
  + C \
  - C++ \
  - Cuda \
  - Fortran \
  - Java \
  - PHP \
  - OpenCL"

#########################################################################################
run_configure --enable-languages=c --enable-binary-analysis=yes
#########################################################################################
test_configure "\
  + Binary analysis \
  + C \
  - C++ \
  - Cuda \
  - Fortran \
  - Java \
  - PHP \
  - OpenCL"

#########################################################################################
run_configure --enable-languages=c --enable-binary-analysis=no
#########################################################################################
test_configure "\
  - Binary analysis \
  + C \
  - C++ \
  - Cuda \
  - Fortran \
  - Java \
  - PHP \
  - OpenCL"

#########################################################################################
run_configure --enable-languages=c --disable-binary-analysis
#########################################################################################
test_configure "\
  - Binary analysis \
  + C \
  - C++ \
  - Cuda \
  - Fortran \
  - Java \
  - PHP \
  - OpenCL"

#########################################################################################
run_configure --enable-languages=c,binaries --disable-binary-analysis
#########################################################################################
test_configure "\
  - Binary analysis \
  + C \
  - C++ \
  - Cuda \
  - Fortran \
  - Java \
  - PHP \
  - OpenCL"

#########################################################################################
run_configure --enable-languages=binaries --disable-binary-analysis
#########################################################################################
test_configure "\
  - Binary analysis \
  - C \
  - C++ \
  - Cuda \
  - Fortran \
  - Java \
  - PHP \
  - OpenCL"



#########################################################################################
# Remove the test sandbox for testing configure lines
rm -rf ${TESTDIR}
#########################################################################################
echo "*********************************************"
echo " SUCCESS: language support configuration is working!"
echo "*********************************************"

