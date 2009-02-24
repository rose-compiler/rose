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
 *    Subsystem  :   DNI (DEBIE Null Interface)
 *    Module     :   taskctrl.h
 *
 * Macros and function prototypes for operations dealing with the
 * kernel, task switching, message passing. The contents of this
 * DNI version of this header file are identical to the DHI version.
 * The DNI/DHI difference is in the bodies of the operations.
 *
 * Based on the SSF DHI file taskctrl.h, revision 1.11, Mon May 17 22:51:12 1999.
 *      
 *- * --------------------------------------------------------------------------
 */

/* Type definitions */

#ifndef TASKCTRL_H
#define TASKCTRL_H

#include "keyword.h"
 
#define MACHINE_CYCLE     1.085
/* The machine (processor) cycle time, in microseconds. */

#define DELAY_LIMIT(TIME) (unsigned short)((((TIME) / MACHINE_CYCLE) - 4) / 2)
/* Computes the number of ShortDelay() argument-units that corresponds */
/* to a certain delay TIME in microseconds. Note that this formula can */
/* yield values larger than ShortDelay() can implement in one call.    */
/* This formula is mainly intended for use with compile-time constant  */
/* values for TIME.                                                    */

#define MAX_SHORT_DELAY 255
/* The largest possible argument for ShortDelay(). */


typedef struct {
   unsigned char  rtx_task_number;
   void           (*task_main_function)(void);
} task_info_t;

/* Function prototypes */

extern void ShortDelay (uint_least8_t delay_loops);

extern void CreateTask(task_info_t EXTERNAL *new_task);

extern void WaitInterval(unsigned char time);

extern void WaitTimeout(unsigned char time) COMPACT REENTRANT_FUNC;

extern void SetTimeSlice(unsigned int time_slice);

extern void StartSystem(unsigned char task_number);

extern void SendTaskMail (
   unsigned char mailbox, 
   uint16_t      message,
   unsigned char timeout);

extern unsigned char isr_send_message (
   unsigned char mailbox,
   uint16_t      message);

#define OK     8
#define NOT_OK 9

#define Send_ISR_Mail(mailbox,message)                 \
   if (isr_send_message (mailbox, message) == NOT_OK)  \
   {                                                   \
      telemetry_data.isr_send_message_error = mailbox; \
   }
/* Send_ISR_Mail is to be used from C51 interrupt routines to send    */
/* mail messages to tasks. If RTX-51 reports an error, the mailbox    */
/* number is set in telemetry. The reason is probably the following:  */
/* -Specified mailbox does not exist(wrong mailbox parameter).        */
/* Send_ISR_Mail is made a macro instead of a function to avoid using */
/* reentrant functions from interrupt routines.                       */
/* Users of Send_ISR_Mail must have access to telemetry_data.         */

#endif
