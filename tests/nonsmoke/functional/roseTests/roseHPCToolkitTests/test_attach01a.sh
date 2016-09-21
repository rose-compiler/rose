#!/bin/sh

# This software was produced with support in part from the Defense Advanced
# Research Projects Agency (DARPA) through AFRL Contract FA8650-09-C-1915.
# Nothing in this work should be construed as reflecting the official policy
# or position of the Defense Department, the United States government,
# or Rice University.

# Same as test_attach01.sh, but uses a HPCViewer profile.
#

SRC_DIR=${SRC_DIR_PREFIX}data/01
PROFILE_DIR=${SRC_DIR}/ANALYSIS

# return code for passing
PASS=0

set -x
"$@" ./attach_metrics \
		-rose:hpct:prof ${PROFILE_DIR}/example_all_events.xml \
		-rose:hpct:eqpath ./src/home/dxnguyen/git-rose/rose-zung/tests/nonsmoke/functional/roseTests/roseHPCToolkitTests/data/01=data/01 \
	-c ${SRC_DIR}/example.c >&1 | tee test_attach01a-out.txt
diff test_attach01a-out.txt ${PROFILE_DIR}/test_attach01a-answer.txt
rc=$?
if [ $rc -eq $PASS ]; then
  echo "******* HPCTOOLKIT test_attach01a PASSED *********"
#  rm -f test_attach01a-out.txt
else
  echo "******* HPCTOOLKIT test_attach01a FAILED *********"
fi


exit $rc

# eof
