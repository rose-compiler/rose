#!/bin/sh

# This software was produced with support in part from the Defense Advanced
# Research Projects Agency (DARPA) through AFRL Contract FA8650-09-C-1915.
# Nothing in this work should be construed as reflecting the official policy
# or position of the Defense Department, the United States government,
# or Rice University.

# generate sample HPCToolkit profile data in XML and answer files
# input argument is $CC, the C compiler.

BIN_DIR=${BIN_DIR-.}
SRC_DIR=${SRC_DIR_PREFIX-}data/01
PROFILE_DIR=${SRC_DIR}/PROFILE
ANALYSIS_DIR=${SRC_DIR}/ANALYSIS
PROGRAM_DIR=${SRC_DIR}/PROGRAM

C_FLAGS='-g -O0'

EVENTS='TOT_CYC FP_OPS L1_DCM L2_TCM'

TARGET=example
OUT_STRUCT=$PROGRAM_DIR/${TARGET}.hpcstruct
OUT_ALL_EVENTS=$ANALYSIS_DIR/${TARGET}_all_events.xml

#--------------------------------------------------------------
# HPCToolkit variables pointing to the supported version of HPCTOOLKIT
HPCT_ROOT=$HOME/hpctoolkit/INSTALL
HPCT_RUN=$HPCT_ROOT/bin/hpcrun
HPCT_PROF=$HPCT_ROOT/bin/hpcprof
HPCT_STRUCT=$HPCT_ROOT/bin/hpcstruct
HPCT_DATA=$HOME/hpcdata/INSTALL/hpcdata.sh

set -x
# $1 is the C compiler
$1 $C_FLAGS ${SRC_DIR}/$TARGET.c -o $TARGET

# generate hpcstruct file:
mkdir -p `dirname ${OUT_STRUCT}`
${HPCT_STRUCT} -I . ${TARGET} -o ${OUT_STRUCT}

# generate performance profile for each PAPI event in ${EVENTS} one at a time
mkdir -p ${PROFILE_DIR}
for c in ${EVENTS} ; do \
	ep=''; \
	if [ $c = L1_DCM ]; then
		ep=PAPI_$c@25000
	elif [ $c = L2_TCM ]; then
		ep=PAPI_$c@25000
	else
		ep=PAPI_$c@500000
	fi; \
	${HPCT_RUN} -o ${PROFILE_DIR}/$c-measurements -e $ep ./${TARGET} 6500 ; \
	${HPCT_PROF} -I . -S ${OUT_STRUCT} ${PROFILE_DIR}/$c-measurements -o  ${PROFILE_DIR}/$c-database; \
	bash ${HPCT_DATA} -o ${PROFILE_DIR}/PAPI_$c.xml ${PROFILE_DIR}/$c-database/experiment.xml; \
	rm -rf ${PROFILE_DIR}/$c-measurements ${PROFILE_DIR}/$c-database*; \
done

# generate performance profile for all PAPI event in ${EVENTS} at the same time
mkdir -p ${ANALYSIS_DIR}
event_list='' ; \
for c in ${EVENTS} ; do \
	if [ $c = L1_DCM ]; then
		event_list="${event_list} -e PAPI_$c@25000"
	elif [ $c = L2_TCM ]; then
		event_list="${event_list} -e PAPI_$c@25000"	
	else
		event_list="${event_list} -e PAPI_$c@500000"
	fi; \
done ; \
echo "event list is " ${event_list}; \
${HPCT_RUN} -o ${ANALYSIS_DIR}/all_events-measurements ${event_list} ./${TARGET} 6500 ; \
${HPCT_PROF} -I . -S ${OUT_STRUCT} ${ANALYSIS_DIR}/all_events-measurements -o  ${ANALYSIS_DIR}/all_events-database
bash ${HPCT_DATA} -o ${OUT_ALL_EVENTS} ${ANALYSIS_DIR}/all_events-database/experiment.xml
 rm -rf ${ANALYSIS_DIR}/all_events-measurements ${ANALYSIS_DIR}/all_events-database*

# generate answer file for test_hL01
${BIN_DIR}/test_highlevel \
		-rose:hpct:prof ${PROFILE_DIR}/PAPI_TOT_CYC.xml \
		-rose:hpct:prof ${PROFILE_DIR}/PAPI_FP_OPS.xml \
		-rose:hpct:prof ${PROFILE_DIR}/PAPI_L1_DCM.xml \
		-rose:hpct:prof ${PROFILE_DIR}/PAPI_L2_TCM.xml \
		-rose:hpct:eqpath ./src/home/dxnguyen/git-rose/rose-zung/tests/nonsmoke/functional/roseTests/roseHPCToolkitTests/data/01=data/01 \
	-c ${SRC_DIR}/${TARGET}.c >&1 | tee ${PROFILE_DIR}/test_hl01-answer.txt
	
# generate answer file for test_attach01a
${BIN_DIR}/attach_metrics \
		-rose:hpct:prof ${ANALYSIS_DIR}/example_all_events.xml \
		-rose:hpct:eqpath ./src/home/dxnguyen/git-rose/rose-zung/tests/nonsmoke/functional/roseTests/roseHPCToolkitTests/data/01=data/01 \
	-c ${SRC_DIR}/${TARGET}.c >&1 | tee ${ANALYSIS_DIR}/test_attach01a-answer.txt

# generate answer file for test_attach01
${BIN_DIR}/attach_metrics \
		-rose:hpct:prof ${PROFILE_DIR}/PAPI_TOT_CYC.xml \
		-rose:hpct:prof ${PROFILE_DIR}/PAPI_FP_OPS.xml \
		-rose:hpct:prof ${PROFILE_DIR}/PAPI_L1_DCM.xml \
		-rose:hpct:prof ${PROFILE_DIR}/PAPI_L2_TCM.xml \
		-rose:hpct:eqpath ./src/home/dxnguyen/git-rose/rose-zung/tests/nonsmoke/functional/roseTests/roseHPCToolkitTests/data/01=data/01 \
	-c ${SRC_DIR}/${TARGET}.c >&1 | tee ${PROFILE_DIR}/test_attach01-answer.txt
	
# generate answer file for test_prop01
${BIN_DIR}/propagate_metrics \
		-rose:hpct:prof ${PROFILE_DIR}/PAPI_TOT_CYC.xml \
		-rose:hpct:prof ${PROFILE_DIR}/PAPI_FP_OPS.xml \
		-rose:hpct:prof ${PROFILE_DIR}/PAPI_L1_DCM.xml \
		-rose:hpct:prof ${PROFILE_DIR}/PAPI_L2_TCM.xml \
		-rose:hpct:eqpath ./src/home/dxnguyen/git-rose/rose-zung/tests/nonsmoke/functional/roseTests/roseHPCToolkitTests/data/01=data/01 \
	-c ${SRC_DIR}/${TARGET}.c >&1 | tee ${PROFILE_DIR}/test_prop01-answer.txt

# eof
