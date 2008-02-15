#!/bin/sh
#
# This test is similar to test_attach.sh, but uses a more realistic
# and complex SMG2000 example.
#

SMG_ROOT=${HOME}/projects/smg2000
SRC_ROOT=${SMG_ROOT}/preproc-dev

SRC_DIR=${SRC_DIR_PREFIX}data/03
PROFILE_DIR=${SRC_DIR}/PROFILE

set -x
"$@" ./attach_metrics \
		\
		-rose:hpct:prof ${PROFILE_DIR}/PAPI_TOT_CYC.xml \
		-rose:hpct:prof ${PROFILE_DIR}/PAPI_FP_OPS.xml \
		-rose:hpct:prof ${PROFILE_DIR}/PAPI_L1_DCA.xml \
		-rose:hpct:prof ${PROFILE_DIR}/PAPI_L1_DCM.xml \
		-rose:hpct:prof ${PROFILE_DIR}/PAPI_L2_TCM.xml \
		-rose:hpct:eqpath /home/vuduc2/projects/smg2000/data/../dev/struct_ls=${SRC_DIR}/struct_ls \
		\
		-I${SRC_ROOT} \
		-I${SRC_ROOT}/utilities \
		-I${SRC_ROOT}/krylov \
		-I${SRC_ROOT}/struct_mv \
		-DTIMER_USE_MPI -DHYPRE_TIMING \
		-I${HOME}/local/mpich-1.2.6/include \
		-I${HOME}/local/mpich-1.2.6/include/mpi2c++ \
	-c ${SRC_DIR}/struct_ls/CPP-smg_residual.c

# eof
