/*------------------------------------------------------------------------------
 *
 *    Copyright (C) 1998 : Space Systems Finland Ltd.
 *
 * Space Systems Finland Ltd (SSF) allows you to use this version of
 * the DEBIE-I DPU software for the specific purpose and under the
 * specific conditions set forth in the Terms Of Use document enclosed
 * with or attached to this software. In particular, the software
 * remains the property of SSF and you must not distribute the software
 * to third parties without written and signed authorization from SSF.
 *
 *    System Name:   DEBIE DPU SW
 *    Subsystem  :   DAS
 *    Module     :   health.h
 *
 * Prototypes etc. for the Health module.
 *
 * Based on the SSF file health.h, revision 1.12, Wed Oct 13 19:50:12 1999.
 *      
 *- * --------------------------------------------------------------------------
 */

#ifndef HEALTH_H
#define HEALTH_H

#include "keyword.h"
#include "dpu_ctrl.h"
#include "su_ctrl.h"

#define CH_SELECTED 1
#define CH_NOT_SELECTED 0

#define RESULT_OK 1
#define CONVERSION_ACTIVE 0
#define HIT_OCCURRED 2 

#define CONVERSION_STARTED 1

#define TEST_OK 1

#define LOW  0
#define HIGH 1

/* Health monitoring round identification numbering. */

#define HEALTH_COUNT    9
/* Health Monitoring loop count. */

#define VOLTAGE_COUNT   17
/* Voltage Measurement loop count. */
 
#define TEMP_COUNT      5
/* Temperature measurement loop count. Its value must equal or greater than  */
/* NUM_SU, because its value defines the SU whos temperatures are to be      */
/* measured.                                                                 */

#define CHECK_COUNT     59
/* Checksum loop count. */

#define MAX_TEMP_1      0xFA
#define MAX_TEMP_2      0xF4
/* Maximum temperature (0xFA = 90 C and 0xF4 = 85C) for a Sensor Unit. */

#define CHECK_SIZE      547
/* Checksum is counted for code memory 547 bytes per check round. */
 
#define CODE_MEMORY_END        0x7FFF
/* The last code memory address to be checked in function */
/* 'CalculateChecksum'.                                   */
/* 'CODE_MEMORY_END'  should have a value smaller         */
/* than 2^16 - 1. Otherwise it will affect a 'for'        */
/* loop in 'CalculateChecksum' function in a way          */
/* that makes this loop infinite.                         */
  
#define MAX_CHECKSUM_COUNT 59
#define MIN_CHECKSUM_COUNT 0
/* Limiting values used in function 'CalculateChecksum'. */

extern EXTERNAL unsigned char confirm_hit_result;  
extern EXTERNAL dpu_time_t internal_time;

extern void SetSoftwareError (unsigned char error)
   COMPACT_DATA REENTRANT_FUNC;
extern void ClearSoftwareError (void);
extern void SetModeStatusError (unsigned char mode_status_error)
   COMPACT_DATA REENTRANT_FUNC;
extern void ClearModeStatusError(void);

extern void SetMode (DEBIE_mode_t mode)
   COMPACT_DATA REENTRANT_FUNC;
extern DEBIE_mode_t GetMode(void);
extern void Clear_SU_Error(void);
extern void Set_SU_Error(sensor_index_t SU_index, unsigned char SU_error);
extern void SetErrorStatus(unsigned char error_source);
extern void ClearErrorStatus(void);
extern void Clear_RTX_Errors(void);


extern void Boot (void);

/* Task functions, for testing: */

extern void InitHealthMonitoring (void);
extern void HandleHealthMonitoring (void);

#endif



