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
 *    Module     :   tc_hand.h
 *
 * Prototypes etc. for the Telecommand Handler module, TC_Hand.
 *
 * Based on the SSF file tc_hand.h, rev 1.11, Mon Mar 22 11:39:54 1999.
 *      
 *- * --------------------------------------------------------------------------
 */


#ifndef TC_HAND_H
#define TC_HAND_H

#include "keyword.h"

/* Valid telecommand address codes:                 */
/* NOTE that all codes are not yet defined, because */
/* all telecommands are not implemented in the      */
/* Prototype SW.                                    */
                       
#define UNUSED_TC_ADDRESS                      0x00

#define START_ACQUISITION                      0x01
#define STOP_ACQUISITION                       0x02

#define ERROR_STATUS_CLEAR                     0x03

#define SEND_STATUS_REGISTER                   0x05
#define SEND_SCIENCE_DATA_FILE                 0x06

#define SET_TIME_BYTE_0                        0x0C
#define SET_TIME_BYTE_1                        0x0D
#define SET_TIME_BYTE_2                        0x0E
#define SET_TIME_BYTE_3                        0x0F

#define SOFT_RESET                             0x09

#define CLEAR_WATCHDOG_FAILURES                0x0A
#define CLEAR_CHECKSUM_FAILURES                0x0B

#define WRITE_CODE_MEMORY_MSB                  0x10
#define WRITE_CODE_MEMORY_LSB                  0x6F
#define WRITE_DATA_MEMORY_MSB                  0x15
#define WRITE_DATA_MEMORY_LSB                  0x6A
#define READ_DATA_MEMORY_MSB                   0x1F
#define READ_DATA_MEMORY_LSB                   0x60

#define SWITCH_SU_1                            0x20
#define SWITCH_SU_2                            0x30
#define SWITCH_SU_3                            0x40
#define SWITCH_SU_4                            0x50

#define SET_SU_1_PLASMA_1P_THRESHOLD           0x21
#define SET_SU_2_PLASMA_1P_THRESHOLD           0x31
#define SET_SU_3_PLASMA_1P_THRESHOLD           0x41
#define SET_SU_4_PLASMA_1P_THRESHOLD           0x51

#define SET_SU_1_PLASMA_1M_THRESHOLD           0x22
#define SET_SU_2_PLASMA_1M_THRESHOLD           0x32
#define SET_SU_3_PLASMA_1M_THRESHOLD           0x42
#define SET_SU_4_PLASMA_1M_THRESHOLD           0x52

#define SET_SU_1_PIEZO_THRESHOLD               0x23
#define SET_SU_2_PIEZO_THRESHOLD               0x33
#define SET_SU_3_PIEZO_THRESHOLD               0x43
#define SET_SU_4_PIEZO_THRESHOLD               0x53

#define SET_SU_1_PLASMA_1P_CLASS_LEVEL         0x24
#define SET_SU_2_PLASMA_1P_CLASS_LEVEL         0x34
#define SET_SU_3_PLASMA_1P_CLASS_LEVEL         0x44
#define SET_SU_4_PLASMA_1P_CLASS_LEVEL         0x54

#define SET_SU_1_PLASMA_1M_CLASS_LEVEL         0x25
#define SET_SU_2_PLASMA_1M_CLASS_LEVEL         0x35
#define SET_SU_3_PLASMA_1M_CLASS_LEVEL         0x45
#define SET_SU_4_PLASMA_1M_CLASS_LEVEL         0x55

#define SET_SU_1_PLASMA_2P_CLASS_LEVEL         0x28
#define SET_SU_2_PLASMA_2P_CLASS_LEVEL         0x38
#define SET_SU_3_PLASMA_2P_CLASS_LEVEL         0x48
#define SET_SU_4_PLASMA_2P_CLASS_LEVEL         0x58

#define SET_SU_1_PIEZO_1_CLASS_LEVEL           0x26
#define SET_SU_2_PIEZO_1_CLASS_LEVEL           0x36
#define SET_SU_3_PIEZO_1_CLASS_LEVEL           0x46
#define SET_SU_4_PIEZO_1_CLASS_LEVEL           0x56

#define SET_SU_1_PIEZO_2_CLASS_LEVEL           0x27
#define SET_SU_2_PIEZO_2_CLASS_LEVEL           0x37
#define SET_SU_3_PIEZO_2_CLASS_LEVEL           0x47
#define SET_SU_4_PIEZO_2_CLASS_LEVEL           0x57 

#define SET_SU_1_PLASMA_1E_1I_MAX_TIME         0x29
#define SET_SU_2_PLASMA_1E_1I_MAX_TIME         0x39
#define SET_SU_3_PLASMA_1E_1I_MAX_TIME         0x49
#define SET_SU_4_PLASMA_1E_1I_MAX_TIME         0x59
 
#define SET_SU_1_PLASMA_1E_PZT_MIN_TIME        0x2A
#define SET_SU_2_PLASMA_1E_PZT_MIN_TIME        0x3A
#define SET_SU_3_PLASMA_1E_PZT_MIN_TIME        0x4A
#define SET_SU_4_PLASMA_1E_PZT_MIN_TIME        0x5A
 
#define SET_SU_1_PLASMA_1E_PZT_MAX_TIME        0x2B
#define SET_SU_2_PLASMA_1E_PZT_MAX_TIME        0x3B
#define SET_SU_3_PLASMA_1E_PZT_MAX_TIME        0x4B
#define SET_SU_4_PLASMA_1E_PZT_MAX_TIME        0x5B
 
#define SET_SU_1_PLASMA_1I_PZT_MIN_TIME        0x2C
#define SET_SU_2_PLASMA_1I_PZT_MIN_TIME        0x3C
#define SET_SU_3_PLASMA_1I_PZT_MIN_TIME        0x4C
#define SET_SU_4_PLASMA_1I_PZT_MIN_TIME        0x5C
 
#define SET_SU_1_PLASMA_1I_PZT_MAX_TIME        0x2D
#define SET_SU_2_PLASMA_1I_PZT_MAX_TIME        0x3D
#define SET_SU_3_PLASMA_1I_PZT_MAX_TIME        0x4D
#define SET_SU_4_PLASMA_1I_PZT_MAX_TIME        0x5D

#define SET_COEFFICIENT_1                      0x70
#define SET_COEFFICIENT_2                      0x71
#define SET_COEFFICIENT_3                      0x72
#define SET_COEFFICIENT_4                      0x73
#define SET_COEFFICIENT_5                      0x74

/* TC codes for SWITCH_SU_x: */

#define ON_VALUE  0x55
#define OFF_VALUE 0x73
#define SELF_TEST 0x99

/* Last TC code for SEND_STATUS_REGISTER: */

#define LAST_EVEN 0x74

/* State of Telecommand Execution task */

typedef enum {
   TC_handling_e, 
   read_memory_e,
   memory_dump_e, 
   write_memory_e,
   memory_patch_e, 
   register_TM_e, 
   SC_TM_e
   } TC_state_t;

extern EXTERNAL TC_state_t TC_state;

/*pointer to a task*/
extern void (* EXTERNAL TC_task)(void);

/* Functions prototype */
extern void Set_TC_Error(void);

/* Task functions, for testing: */

extern void TC_InterruptService (void);
extern void InitTelecommandTask (void);
extern void HandleTelecommand (void);

#endif
          
