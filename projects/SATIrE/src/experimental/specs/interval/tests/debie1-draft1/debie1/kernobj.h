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
 *    Module     :   kernobj.h
 *
 * Definitions for the interface to the real-time kernel.
 *
 * Based on the SSF file kernobj.h, revision 1.3, Thu Sep 09 16:01:56 1999.
 *
 *- * --------------------------------------------------------------------------
 */

/* Definitions of kernel objects (eg. task and mailbox numbers) */

/* Task numbers */

#ifndef KERNOBJ_H
#define KERNOBJ_H

#define HEALTH_MONITORING_TASK     0
#define TC_TM_INTERFACE_TASK       1
#define ACQUISITION_TASK           2

#define HIT_TRIGGER_ISR_TASK       3

/* Task priorities */

#define HEALTH_MONITORING_PR       0
#define ACQUISITION_PR             1
#define TC_TM_INTERFACE_PR         2

#define HIT_TRIGGER_PR             3

/* Mailbox numbers */

#define TCTM_MAILBOX               0
#define ACQUISITION_MAILBOX        1

/* ISR source numbers */

#define TC_ISR_SOURCE              0
#define TM_ISR_SOURCE              2
#define HIT_TRIGGER_ISR_SOURCE     5

#endif

