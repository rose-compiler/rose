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
 *    Module     :   class.h
 *
 * Prototypes etc. for the event-classification module Class.
 *
 * Based on the SSF file class.h, rev 1.6, Fri Jun 04 14:47:38 1999.
 *
 *- * --------------------------------------------------------------------------
 */


#ifndef CLASS_H
#define CLASS_H

#include "keyword.h"
#include "tm_data.h"

#define MAX_AMPLITUDE_TERM 5
/* Maximum value for an amplitude term in the quality formula. */
/* Valid range: 1 - 255. */

#define DEFAULT_COEFF 5
/* Default  value for classification coefficient        */
/* adjustable with telecommands. Gives maximum allowed  */
/* (5) amplitude term with maximum amplitude with this  */
/* formula. If amplitudes are going to be smaller, the  */
/* amplitude can be amplified by setting greater value  */
/* to the quality coefficient. Minimum amplification is */
/* 1/5 and maximum 50.                                  */
/* Valid range 1 - 255. */

#define AMPLITUDE_DIVIDER ((DEFAULT_COEFF * 16.0) / MAX_AMPLITUDE_TERM)
/* Divider for an amplitude term in the quality formula.      */
/*  16 = maxumum value for the rough 2 based logarithm of the */
/*       signal amplitude in the quality formula.             */

#define PLASMA_1_PLUS_CLASS  0x80
#define PLASMA_1_MINUS_CLASS 0x40
#define PLASMA_2_PLUS_CLASS  0x08
#define PIEZO_1_CLASS        0x20
#define PIEZO_2_CLASS        0x10
/* Classification index mask values for signal amplitudes */
/* above the classification levels.                       */

#define PLASMA_1_PLUS_TO_PIEZO_CLASS  0x02
#define PLASMA_1_MINUS_TO_PIEZO_CLASS 0x01
#define PLASMA_1_PLUS_TO_MINUS_CLASS  0x04
/* Classification index mask values for delays inside the */
/* time windows.                                          */

extern void InitClassification(void);
/* Inits classification thresholds and coefficients. */

extern void ClassifyEvent(event_record_t EXTERNAL *new_event);
/* Classifies event and calculates the quality number. */

extern void Init_SU_Settings (SU_settings_t EXTERNAL *set);
/* Sets the default values for classification parameters. */

#endif
