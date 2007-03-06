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
 *    Module     :   telem.h
 *
 * Prototypes etc. for the Telem module.
 *
 * Based on the SSF file telem.h, rev 1.11, Wed May 12 13:08:26 1999.
 *      
 *- * --------------------------------------------------------------------------
 */

#ifndef TELEM_H
#define TELEM_H

#include "keyword.h"
#include "tm_data.h"

/* Special value for TC/TM mail to be used only     */
/* telemetry is ready                               */

#define TM_READY                              0xFFFF

extern unsigned char EXTERNAL *telemetry_pointer;
extern unsigned char EXTERNAL *telemetry_end_pointer;
extern unsigned int  EXTERNAL free_slot_index;

extern unsigned char EXTERNAL read_memory_checksum;
/* Checksum to be sent at the end of Read Memory sequence. */


extern event_record_t EXTERNAL *GetFreeRecord(void);
/* Returns pointer to next free event record from the */
/* event record queue, or pointer to the last record  */
/* od the queue, if the queue is full.                */

extern void ResetEventQueueLength(void);

/* Task functions, for testing: */

extern void TM_InterruptService (void);

#endif

