#!/bin/sh

# This software was produced with support in part from the Defense Advanced
# Research Projects Agency (DARPA) through AFRL Contract FA8650-09-C-1915.
# Nothing in this work should be construed as reflecting the official policy
# or position of the Defense Department, the United States government,
# or Rice University.

SRC_DIR=${SRC_DIR_PREFIX}data/01
PROFILE_DIR=${SRC_DIR}/PROFILE

# return code for passing
PASS=0

set -x
"$@" ./propagate_metrics \
		-rose:hpct:prof ${PROFILE_DIR}/PAPI_TOT_CYC.xml \
		-rose:hpct:prof ${PROFILE_DIR}/PAPI_FP_OPS.xml \
		-rose:hpct:prof ${PROFILE_DIR}/PAPI_L1_DCM.xml \
		-rose:hpct:prof ${PROFILE_DIR}/PAPI_L2_TCM.xml \
		-rose:hpct:eqpath ./src/home/dxnguyen/git-rose/rose-zung/tests/roseTests/roseHPCToolkitTests/data/01=data/01 \
	-c ${SRC_DIR}/example.c >&1 | tee test_prop01-out.txt
diff test_prop01-out.txt ${PROFILE_DIR}/test_prop01-answer.txt
rc=$?

if [ $rc -eq $PASS ]; then
  echo "******* HPCTOOLKIT test_prop01 PASSED *********"
else
  echo "******* HPCTOOLKIT test_prop01 FAILED *********"
fi

rm -f test_prop01-out.txt

exit $rc

# eof
