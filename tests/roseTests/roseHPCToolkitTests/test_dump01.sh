#!/bin/sh

SRC_DIR=${SRC_DIR_PREFIX}data/01
PROFILE_DIR=${SRC_DIR}/PROFILE

set -x
"$@" ./dump_ast \
		-rose:hpct:prof ${PROFILE_DIR}/PAPI_TOT_CYC.xml \
		-rose:hpct:prof ${PROFILE_DIR}/PAPI_FP_OPS.xml \
		-rose:hpct:prof ${PROFILE_DIR}/PAPI_L1_DCA.xml \
		-rose:hpct:prof ${PROFILE_DIR}/PAPI_L1_DCM.xml \
		-rose:hpct:prof ${PROFILE_DIR}/PAPI_L2_TCM.xml \
		-rose:hpct:eqpath .=${SRC_DIR} \
	-c ${SRC_DIR}/example.c

# eof

