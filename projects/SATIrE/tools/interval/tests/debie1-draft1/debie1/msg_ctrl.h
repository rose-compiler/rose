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
 *    Module     :   msg_ctrl.h
 *
 * Macros and operations for sending messages between tasks and
 * from interrupt handlers to tasks.
 *      
 * Based on the SSF DHI file msg_ctrl.h, rev 1.11, Mon May 17 22:50:44 1999.
 *      
 *- * --------------------------------------------------------------------------
 */


#ifndef MSG_CTRL_H
#define MSR_CTRL_H

#include "keyword.h"

 
#define MSG_RECEIVED 1
/* The value of execution_result in incoming_mail_t that */
/* signifies that a mail message has been received.      */
/* Must be different from NOT_OK as defined in RTX51.h.  */

#define TIMEOUT_OCCURRED 4
/* The value of execution_result in incoming_mail_t that */
/* signifies that the wait for mail has timed out.       */
/* Must be different from NOT_OK as defined in RTX51.h.  */


typedef struct {
   unsigned char         mailbox_number;
   unsigned char         timeout;
   uint16_t     EXTERNAL *message;
   signed char execution_result;  /* This variable is used to indicate       */
                                  /* execution results.                      */

   signed char wait_result;       /* Result from a RTX operation.            */
 
   unsigned char event_selector;  /* The value of this variable defines the  */
                                  /* execution of the wait-task.             */
}incoming_mail_t ;  

/* Function prototypes */

extern void WaitMail(incoming_mail_t EXTERNAL *message)
    COMPACT REENTRANT_FUNC;

#endif

