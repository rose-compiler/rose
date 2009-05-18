#!/bin/bash -vx
#
# A script to test an installed copy of ROSE on 
# a set of external benchmarks.
#
# Liao, 5/18/2009

if [ $# -lt 1 ]
then
  echo This script needs one argument
  echo Usage:$0 ROSE_INSTALL_PATH
  echo Exampe:$0 /home/liao6/opt/roseLatest
  exit
fi

# retrieve the argument as rose's installation path
ROSE_INS=$1
SPEC_CPU2006_INS=/home/liao6/opt/spec_cpu2006

# check if the directory exists
test -d $ROSE_INS || (echo "ERROR: ROSE installation path:$ROSE_INS does not exists! "; exit 1)

# prepare environments for using ROSE as a compiler
PATH=$ROSE_INS/bin:$PATH
LD_LIBRARY_PATH=$ROSE_INS/lib:$LD_LIBRARY_PATH
export PATH LD_LIBRARY_PATH

# goto the external benchmarks to do the tests
# test ROSE using SPEC CPU 2006
#------------------------------------------
C_BENCH_TO_PASS=(perlbench bzip2 mcf milc gobmk hmmer sjeng libquantum lbm sphinx3)
C_BENCH_CURRENTLY_FAIL=(gcc h264ref)

CXX_BENCH_TO_PASS=(namd astar)
CXX_BENCH_CURRENTLY_FAIL=(dealII soplex povray omnetpp xalancbmk)

cd $SPEC_CPU2006_INS
. ./shrc
# clean up previous object/executable files
runspec --config=tux268-rose-gcc42.cfg --tune=base --size=test --iterations=1 --noreportable fp int --action=clobber
runspec --config=tux268-rose-gcc42.cfg --tune=base --size=test --iterations=1 --noreportable ${C_BENCH_CURRENTLY_FAIL[@]}
#runspec --config=tux268-rose-gcc42.cfg --tune=base --size=test --iterations=1 --noreportable ${C_BENCH_TO_PASS[@]}

