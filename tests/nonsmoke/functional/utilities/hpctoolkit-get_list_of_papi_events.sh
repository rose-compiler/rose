#!/bin/bash
#
# ==Overview
#
# Prints list of PAPI events supported by your platform.
#
# ==Requirements
#
# The `hpcrun` command must be available via your $PATH.
#
# ==Example
#
#     ```bash
#     $ ./scripts/hpctoolkit-get_list_of_papi_events.sh
#     PAPI_L1_DCM
#     PAPI_L1_ICM
#     PAPI_L2_ICM
#     PAPI_L2_TCM
#     PAPI_L3_TCM
#     PAPI_L3_LDM
#     PAPI_TLB_DM
#     PAPI_TLB_IM
#     PAPI_L1_LDM
#     PAPI_L1_STM
#     PAPI_L2_LDM
#     PAPI_L2_STM
#     PAPI_BR_UCN
#     PAPI_BR_CN
#     PAPI_BR_TKN
#     PAPI_BR_MSP
#     PAPI_TOT_IIS
#     PAPI_TOT_INS
#     PAPI_FP_INS
#     PAPI_LD_INS
#     PAPI_SR_INS
#     PAPI_BR_INS
#     PAPI_RES_STL
#     PAPI_TOT_CYC
#     PAPI_L2_DCA
#     PAPI_L2_DCR
#     PAPI_L3_DCR
#     PAPI_L2_DCW
#     PAPI_L3_DCW
#     PAPI_L1_ICH
#     PAPI_L2_ICH
#     PAPI_L1_ICA
#     PAPI_L2_ICA
#     PAPI_L3_ICA
#     PAPI_L1_ICR
#     PAPI_L2_ICR
#     PAPI_L3_ICR
#     PAPI_L2_TCA
#     PAPI_L3_TCA
#     PAPI_L2_TCW
#     PAPI_L3_TCW
#     PAPI_VEC_SP
#     PAPI_VEC_DP
#     PAPI_REF_CYC
#     ```

if test -z "$(which hpcrun)"; then
    echo "[FATAL] hpcrun command not found. Please add HPCTOOLKIT to your \$PATH."
    exit 1
else
    # Check for "^PAPI_" (again) at the end of the pipeline as an extra level
    # of validation that the final output list does indeed contain PAPI_* events.
    hpcrun -L | grep "^PAPI_" | grep "Yes" | awk '{print $1}' | grep "^PAPI_"
    # Validate exit status of each command in the pipeline:
    if [ $?               -ne 0 -o \
         ${PIPESTATUS[0]} -ne 0 -o \
         ${PIPESTATUS[1]} -ne 0 -o \
         ${PIPESTATUS[2]} -ne 0 -o \
         ${PIPESTATUS[3]} -ne 0 ];
    then
        echo "[FATAL] Unable to retrieve the list of supported PAPI events on your platform."
        exit 1
    fi
fi

