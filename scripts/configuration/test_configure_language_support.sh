#!/bin/bash
#
# Validate ./configure lines to maintain configure correctness
#
#

# ROSE_TEST_BOOST_ROOT

SCRIPT_DIR="`pwd`"
ROSE_DIR="`pwd`/../.."
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
 rm -rf $LOG
 cmd="${CONFIGURE} $*"
 echo "Executing ${cmd}"
 $cmd 2>/dev/null > $LOG
 if test $? != 0 ; then
  echo "   Configure-line failed...($cmd)" 
  exit $?
 else
  echo "   PASSED"
  echo ""
 fi
}

function run_xfail_configure {
 rm -rf $LOG
 cmd="${CONFIGURE} $*"
 echo "Executing ${cmd}"
 $cmd 2>/dev/null > $LOG
 if test $? = 0 ; then
  echo "   Configure-line was expected to fail...($cmd)" 
  exit 1
 else
  echo "   PASSED (expected failure)"
  echo ""
 fi
}

function test_configure {
 rm -rf $LOG_OUT $CORRECT_OUT
#replaces newline: sed '{:q;N;s/\n/ /g;t q}'  
 sed -n '/checking for user-specified languages to support/,/disabled/p' $LOG | sed -n 's/.*\( [-+] .*\)/\1/p' | sed '{:q;N;s/\n/ /g;t q}' | sed 's/^[ \t]*//;s/[ \t]*$//' > $LOG_OUT
# sed -n '/checking for user-specified languages to support/,/disabled/p' $LOG | sed -n 's/.*\([-+] .*\)/\1/p' | sed '{:q;N;s/\n/ /g;t q}' > $LOG_OUT
 echo "   actual:   " `cat $LOG_OUT`
# echo "   -------------------------"
 echo -e $1 > $CORRECT_OUT
 echo "   expected: " `cat $CORRECT_OUT`
 diff -w $LOG_OUT $CORRECT_OUT
 if test $? != 0 ; then
  echo "ERROR: language support configuration is broken"
  exit $?
 fi
 echo ""
}


if test "x$1" = "xtest" ; then
source ${SCRIPT_DIR}/test_configure_binary_support.unit
source ${SCRIPT_DIR}/test_configure_c_support.unit
source ${SCRIPT_DIR}/test_configure_cxx_support.unit
source ${SCRIPT_DIR}/test_configure_cuda_support.unit
source ${SCRIPT_DIR}/test_configure_fortran_support.unit
source ${SCRIPT_DIR}/test_configure_java_support.unit
source ${SCRIPT_DIR}/test_configure_php_support.unit
source ${SCRIPT_DIR}/test_configure_opencl_support.unit

echo "*********************************************"
echo " SUCCESS: language support configuration is working!"
echo "*********************************************"
fi

#########################################################################################
# Remove the test sandbox for testing configure lines
#rm -rf ${TESTDIR}
#########################################################################################
