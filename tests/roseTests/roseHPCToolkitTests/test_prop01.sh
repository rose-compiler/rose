#!/bin/sh

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

exit $rc

# eof
