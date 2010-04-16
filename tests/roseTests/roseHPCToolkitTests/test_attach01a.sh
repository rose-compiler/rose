#!/bin/sh
#
# Same as test_attach01.sh, but uses a HPCViewer profile.
#

SRC_DIR=${SRC_DIR_PREFIX}data/01
PROFILE_DIR=${SRC_DIR}/ANALYSIS

set -x
"$@" ./attach_metrics \
		-rose:hpct:prof ${PROFILE_DIR}/scope_tree.hpcviewer \
		-rose:hpct:eqpath ./src1=${SRC_DIR} \
	-c ${SRC_DIR}/example.c

# eof
