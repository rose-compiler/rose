/*------------------------------------------------------------------------------
 *
 * Copyright (C) 1998 : Space Systems Finland Ltd.
 *
 * Space Systems Finland Ltd (SSF) allows you to use this version of
 * the DEBIE-I DPU software for the specific purpose and under the
 * specific conditions set forth in the Terms Of Use document enclosed
 * with or attached to this software. In particular, the software
 * remains the property of SSF and you must not distribute the software
 * to third parties without written and signed authorization from SSF.
 *
 *    System Name:   DEBIE DPU SW, test harness for WCET analysis
 *    Subsystem  :   DNI (DEBIE Null Interface)
 *    Module     :   harness.c
 *
 * Implementations of the DNI operations, suitable for compiling
 * and linking the DEBIE I DPU Software as a standard C program,
 * on any processor that supports the required data types (eg. 8-bit
 * char, 32-bit long) and has enough program and data memory.
 *
 * This implementation is designed to support measurement-based
 * timing analysis by feeding the DEBIE SW with selected sets of
 * test data.
 *
 * Options: if the preprocessor symbol TRACE_HARNESS is defined,
 * the harness operations generate trace message on stdout.
 *
 * Based, with extensive changes, on the SSF file rtx_if.c, rev 1.13,
 * Fri May 21 00:14:00 1999.
 *
 *- * --------------------------------------------------------------------------
 */

/* This file contains several sets of operations, as follows:
 *
 * > processor registers as variables: reg52.h
 *
 * > kernel operations: taskctrl.h, isr_ctrl.h, msg_ctrl.h
 *
 * > processor/memory operations: dpu_ctrl.h
 *
 * > telecommand and telemetry interface operations: ttc_ctrl.h, isr_ctrl.h
 *
 * > operations for the A/D converted: ad_conv.h
 *
 * > operations on the sensor units: su_ctrl.h, dpu_ctrl.h
 *
 * > accessing the VALUE_OF "unsigned short" telemetry items: keyword.h
 *
 * > "calling" a "patch function": keyword.h
 *
 * > checking the sizes of various types (for analysis only).
 *
 * For testing purposes the main operation is StartSystem(). The DEBIE
 * main function calls StartSystem(), which normally activates the
 * kernel and the DEBIE application tasks and interrupt handlers. In this
 * version, StartSystem() takes over the show and runs the test scenario.
 *
 */


#if defined(TRACE_HARNESS)
#include <stdio.h>
#endif

#include <stdlib.h>
#include "keyword.h"
#include "health.h"
#include "measure.h"
#include "tc_hand.h"
#include "telem.h"


/*   Processor registers : reg52.h    */


unsigned char EA;


/*   Kernel operations : taskctrl.h    */

#include "taskctrl.h"

void CreateTask (task_info_t EXTERNAL *new_task)
/* Purpose        : Task is created in the RTX.                              */
/* Interface      : input:   - new_task                                      */
/*                  output:  - telemetry_data.os_create_task_error           */
/* Preconditions  : none                                                     */
/* Algorithm      : -In case of an error, 'new_task' is stored to telemetry  */
/*                   as an error indication.                                 */
{
#if defined(TRACE_HARNESS)
   printf ("CreateTask %d\n", new_task->rtx_task_number);
#endif

   switch (new_task->rtx_task_number) {

   case TC_TM_INTERFACE_TASK:

      InitTelecommandTask ();

      break;

   case ACQUISITION_TASK:

      InitAcquisitionTask ();

      break;

   case HIT_TRIGGER_ISR_TASK:

      InitHitTriggerTask ();

      break;

   default:

#if defined(TRACE_HARNESS)
      printf ("CreateTask: unknown task number\n");
#endif

      break;
   }
}


void WaitInterval(unsigned char time)
/* Purpose        : Interval is waited with RTX.                             */
/* Interface      : input:   - time                                          */
/*                  output:  - telemetry_data.os_wait_error                  */
/* Preconditions  : none                                                     */
/* Postconditions : Interval for wait is set.                                */
/* Algorithm      : -In case of an error, 'K_IVL' is stored to telemetry     */
/*                   as an error indication and error bit is set in          */
/*                   software_error register.                                */
{
#if defined(TRACE_HARNESS)
   printf ("WaitInterval %d\n", time);
#endif
}


void WaitTimeout(unsigned char time) COMPACT REENTRANT_FUNC
/* Purpose        : Timeout is waited with RTX.                              */
/* Interface      : input:   - time                                          */
/*                  output:  - telemetry_data.os_wait_error                  */
/* Preconditions  : none                                                     */
/* Postconditions : Specified time has elapsed.                              */
/* Algorithm      : -In case of an error, 'K_TMO' is stored to telemetry     */
/*                   as an error indication and error bit is set in          */
/*                   software_error register.                                */
{
#if defined(TRACE_HARNESS)
   printf ("WaitTimeout %d\n", time);
#endif
}


void SetTimeSlice(unsigned int time_slice)
/* Purpose        : Time slice in the RTX is set.                            */
/* Interface      : input:   - time_slice                                    */
/*                  output:  - telemetry_data.os_set_slice_error             */
/* Preconditions  : none                                                     */
/* Postconditions : Timeslice which defines the time interval in number of   */
/*                  processor cycles is set.                                 */
/* Algorithm      :  In case of an error, indication bit is set in           */
/*                   the software_error register.                            */
{
#if defined(TRACE_HARNESS)
   printf ("SetTimeSlice %d\n", time_slice);
#endif
}


void ShortDelay (uint_least8_t delay_loops)
{
#if defined(TRACE_HARNESS)
   printf ("ShortDelay %d\n", delay_loops);
#endif
}


unsigned char isr_send_message (
   unsigned char mailbox,
   uint16_t      message)
/* Purpose        : Send mail from ISR to a requested mailbox.              */
/* Interface      : input:   - mailbox, message                             */
/*                  output:  - telemetry_data.os_send_message_error         */
/* Preconditions  : Mailbox number should be a value 0 - 7                  */
/* Postconditions : Mail is send to a given mailbox.                        */
/* Algorithm      : - In case of an error, failed 'mailbox' is stored to    */
/*                    telemetry.                                            */
{
#if defined(TRACE_HARNESS)
   printf ("isr_send_message to %d, message %d = 0x%x\n",
      mailbox, message, message);
#endif
   SendTaskMail (mailbox, message, 0);
   return OK;
}


/*   Interrupt services : isr_ctrl.h   */


#include "isr_ctrl.h"


void AttachInterrupt(unsigned char ISR_VectorNumber)
/* Purpose        : Interrupt with a given number is assigned to a task in   */
/* Interface      : input:   - ISR_VectorNumber                              */
/*                  output:  - telemetry_data.os_attach_interrupt_error      */
/* Preconditions  : none                                                     */
/* Postconditions : Interrupt is attached to a calling task.                 */
/* Algorithm      : -In case of an error, 'ISR_VectorNumber' is stored to    */
/*                   telemetry as an error indication.                       */
{
#if defined(TRACE_HARNESS)
   printf ("AttachInterrupt %d\n", ISR_VectorNumber);
#endif
}


void EnableInterrupt(unsigned char ISR_VectorNumber)
/* Purpose        : Interrupt with a given number is enabled in the RTX.     */
/* Interface      : input:   - ISR_VectorNumber                              */
/*                  output:  - telemetry_data.os_enable_isr_error            */
/* Preconditions  : none                                                     */
/* Postconditions : Interrupt is enabled.                                    */
/* Algorithm      : -In case of an error, 'ISR_VectorNumber' is stored to    */
/*                   telemetry as an error indication.                       */
{
#if defined(TRACE_HARNESS)
   printf ("EnableInterrupt %d\n", ISR_VectorNumber);
#endif
}


void DisableInterrupt(unsigned char ISR_VectorNumber)
/* Purpose        : Interrupt with a given number is disabled in the RTX.    */
/* Interface      : input:   - ISR_VectorNumber                              */
/*                  output:  - telemetry_data.os_disable_isr_error           */
/* Preconditions  : none                                                     */
/* Postconditions : Interrupt is enabled.                                    */
/* Algorithm      : -In case of an error, 'ISR_VectorNumber' is stored to    */
/*                   telemetry as an error indication.                       */
{
#if defined(TRACE_HARNESS)
   printf ("DisableInterrupt %d\n", ISR_VectorNumber);
#endif
}


signed char SetInterruptMask(unsigned char ISR_MaskNumber)
/* Purpose        : Interrupt mask bit is set is in the RTX.                 */
/* Interface      : Return value, which describes the execution result, is   */
/*                  always zero as this function does no parameter checking. */
/*                  Used to manipulate special bits, which are part of the   */
/*                  interrupt enable registers or to modify interrupt enable */
/*                  bits from inside a C51 interrupt function.               */
/*                  Not to be used for interrupt sources attached to RTX51   */
/*                  tasks.                                                   */
/* Preconditions  :                                                          */
/* Postconditions : Interrupt mask is set.                                   */
/* Algorithm      : RTX syntax is used.                                      */
{
#if defined(TRACE_HARNESS)
   printf ("SetInterruptMask 0x%x\n", ISR_MaskNumber);
#endif
   return 0;  /* Success. */
}


signed char ResetInterruptMask(unsigned char ISR_MaskNumber)
/* Purpose        : Interrupt mask bit is reset is in the RTX.               */
/* Interface      : Return value, which describes the execution result, is   */
/*                  always zero as this function does no parameter checking. */
/*                  Used to manipulate special bits, which are part of the   */
/*                  interrupt enable registers or to modify interrupt enable */
/*                  bits from inside a C51 interrupt function.               */
/*                  Not to be used for interrupt sources attached to RTX51   */
/*                  tasks.                                                   */
/* Preconditions  :                                                          */
/* Postconditions : Interrupt mask is reset.                                 */
/* Algorithm      : RTX syntax is used.                                      */
{
#if defined(TRACE_HARNESS)
   printf ("ResetInterruptMask 0x%x\n", ISR_MaskNumber);
#endif
   return 0;  /* Success. */
}


void WaitInterrupt (unsigned char ISR_VectorNumber, unsigned char timer)
/* Purpose        : Interrupt is waited in the RTX.                          */
/* Interface      : input:   - ISR_VectorNumber,timer                        */
/*                  output:  - telemetry_data.os_wait_error                  */
/* Preconditions  : none                                                     */
/* Postconditions : Interrupt is waited.                                     */
/* Postconditions : Interrupt is enabled.                                    */
/* Algorithm      : -In case of an error, 'K_INT' is stored to telemetry     */
/*                   as an error indication and error bit is set in          */
/*                   software_error register.                                */
{
#if defined(TRACE_HARNESS)
   printf ("WaitInterrupt %d, time %d\n", ISR_VectorNumber, timer);
#endif
}

/* See below, Telecommand and telemetry interface, for the ISR */
/* operations related to the TC timer.                         */



/*   Message sending : msg_ctrl.h */


#include "msg_ctrl.h"


static uint16_t mail_message[8];
/* The last message in the mailbox. */

static int mail_count[8] = {0, 0, 0, 0, 0, 0, 0, 0};
/* The number of messages in the mailbox. */
/* Should be between 0 and 1.             */

static int mail_overflows = 0;
/* The number of times a mailbox has overflowed. */


void SendTaskMail (
   unsigned char mailbox,
   uint16_t      message,
   unsigned char timeout)
/* Purpose        : Send mail to a requested mailbox.                       */
/* Interface      : input:   - mailbox, message, timeout                    */
/*                  output:  - telemetry_data.os_send_message_error         */
/* Preconditions  : Mailbox number should be a value 0 - 7                  */
/*                  timeout should be a value 0 - 255                       */
/* Postconditions : Mail is send to a given mailbox.                        */
/* Algorithm      : - In case of an error, failed 'mailbox' is stored to    */
/*                    telemetry.                                            */
{
#if defined(TRACE_HARNESS)
   printf ("SendTaskMail to %d, message %d, timeout %d\n",
      mailbox, message, timeout);
#endif

   if (mail_count[mailbox] == 0)
   {
      mail_message[mailbox] = message;
   }
   else
   {
#if defined(TRACE_HARNESS)
      printf ("Overflow on mailbox %d, already %d message(s)\n",
         mailbox, mail_count[mailbox]);
#endif
      mail_overflows ++;
   }

   mail_count[mailbox] ++;
}


void WaitMail (incoming_mail_t EXTERNAL *message) COMPACT_DATA REENTRANT_FUNC
/* Purpose        : Mail is waited from the given mailbox.                   */
/* Interface      : Return value, which describes the execution result, is   */
/*                  stored in to a struct.                                   */
/* Preconditions  : Mailbox number should be a value 0 - 7                   */
/*                  Time-out should have a value 0 - 255.                    */
/* Postconditions : Message is received or timeout has occurred or error has */
/*                  occurred due to unvalid parameter values.                */
/* Algorithm      : -In case of an error, 'event_selector' is stored to      */
/*                   telemetry as an error indication and error bit is set   */
/*                   in software_error register.                             */
{
#if defined(TRACE_HARNESS)
   printf ("WaitMail from %d, timeout %d\n",
      message -> mailbox_number, message -> timeout);
#endif

   if (mail_count[message -> mailbox_number] > 0)
   {
      message -> wait_result      = MSG_RECEIVED;
      message -> execution_result = MSG_RECEIVED;
      *(message -> message)       = mail_message[message -> mailbox_number];

#if defined(TRACE_HARNESS)
      printf ("Message from %d is %d = 0x%x\n",
         message -> mailbox_number,
         *(message -> message),
         *(message -> message));
#endif

      mail_count[message -> mailbox_number] --;
   }
   else
   {
      message -> wait_result      = TIMEOUT_OCCURRED;
      message -> execution_result = TIMEOUT_OCCURRED;
      *(message -> message)       = 0;
   }
}


static void FlushMail (unsigned char mailbox)
/* Remove all mail from the mailbox. Harness use only. */
{
#if defined(TRACE_HARNESS)
   printf ("FlushMail from box %d, which had %d messages.\n",
      mailbox, mail_count[mailbox]);
#endif

   mail_count[mailbox] = 0;
}


/*    Memory operations : dpu_ctrl.h    */


#include "dpu_ctrl.h"


void Set_Data_Byte (data_address_t addr, unsigned char value)
{
#if defined(TRACE_HARNESS)
   printf ("Set_Data_Byte 0x%x to %d = 0x%x\n", addr, value, value);
#endif
}


unsigned char Get_Data_Byte (data_address_t addr)
{
#if defined(TRACE_HARNESS)
   printf ("Get_Data_Byte 0x%x\n", addr);
#endif
   return 0;
}


unsigned char Get_Code_Byte (code_address_t addr)
{
#if defined(TRACE_HARNESS)
   printf ("Get_Code_Byte 0x%x\n", addr);
#endif
   return 0;
}


unsigned char Check_Current (unsigned char bits)
{
   unsigned char val;
#if defined(TRACE_HARNESS)
   printf ("Check_Current 0x%x\n", bits);
#endif
   switch (bits) {
   case   3: val =  1; break;
   case  12: val =  4; break;
   case  48: val = 16; break;
   case 192: val = 64; break;
   default : val =  0;
#if defined(TRACE_HARNESS)
             printf ("Check_Current param error\n");
             break;
#endif
   }
   return val;
}


/* The following routines are implemented in das/hw_if.c:
 * Init_DPU
 * GetResetClass
 * SignalMemoryErrors
 * SetMemoryConfiguration
 * GetMemoryConfiguration
 * PatchCode
 */


void Reboot(reset_class_t boot_type)
{
#if defined(TRACE_HARNESS)
   printf ("Reboot %d\n", boot_type);
#endif
   while(1) {};
}


/* Functions originally implemented in assembly-language (asmfuncs.a51): */


unsigned char TestMemBits (data_address_t address)
/*
 * Test each bit of a cell in RAM memory.
 * For each of the eight bits, the value with this bit ON and
 * all others bits OFF is written to the cell, and the written
 * value is read back and verified.
 * The original content of the cell is destroyed.
 * Parameters:
 *    address  Cell address.
 * TestMemBits returns zero for success, and otherwise the
 * first bit pattern for which the cell failed.
 *
 */
{
#if defined(TRACE_HARNESS)
   printf ("TestMemBits 0x%x\n", address);
#endif
   return 0;
}


unsigned char TestMemData (
   data_address_t start,
   uint_least8_t  bytes)
/*
 * Test an area of RAM memory.
 * For each RAM cell in the given area, the values AAh
 * and 55h are written to the cell, and the written value
 * is read back and verified.
 * The original content of each cell is destroyed.
 * Parameters:
 *    start   Starting address
 *    bytes   Length of area
 *            1 .. 255. Note, zero is NOT allowed!
 * TestMem returns the number of bytes remaining to test
 * when the first memory failure occurred.
 * If this value is nonzero, the first failing address is
 * found by subtracting the return value from 'start+bytes'.
 *
 */
{
#if defined(TRACE_HARNESS)
   printf ("TestMemData start 0x%x, bytes %d\n", start, bytes);
#endif
   return 0;
}


unsigned char TestMemSeq (
   data_address_t start,
   uint_least8_t  bytes)
/*
 * Test an area of RAM memory.
 * For each RAM cell in the given area, the logical complement of
 * the low byte of the cell address is written into the cell, in
 * one pass from the start to the end. Then, in a second pass,
 * each cell is read and the value is is verified.
 * The original content of each cell is destroyed.
 * Parameters:
 *    start   Starting address
 *    bytes   Length of area
 *            1 .. 255. Note, zero is NOT allowed!
 * TestMemSeq returns the number of bytes remaining to test
 * when the first memory failure occurred.
 * If this value is nonzero, the first failing address is
 * found by subtracting the return value from 'start+bytes'.
 *
 */
{
#if defined(TRACE_HARNESS)
   printf ("TestMemSeq start 0x%x, bytes %d\n", start, bytes);
#endif
   return 0;
}


/*    Telecommand and telemetry interface : ttc_ctrl.h    */


#include "ttc_ctrl.h"


static unsigned char tc_msb, tc_lsb;
/* Simulated TC interface registers. */


unsigned char Read_TC_MSB (void)
{
#if defined(TRACE_HARNESS)
   printf ("Read_TC_MSB\n");
#endif
   return tc_msb;
}


unsigned char Read_TC_LSB (void)
{
#if defined(TRACE_HARNESS)
   printf ("Read_TC_LSB\n");
#endif
   return tc_lsb;
}


static unsigned char tm_msb, tm_lsb;
/* Simulated TM interface registers. */


void Write_TM_LSB (unsigned char value)
{
#if defined(TRACE_HARNESS)
   printf ("Write_TM_LSB %d = 0x%x\n", value, value);
#endif
   tm_lsb = value;
}


void Write_TM_MSB (unsigned char value)
{
#if defined(TRACE_HARNESS)
   printf ("Write_TM_MSB %d = 0x%x\n", value, value);
#endif
   tm_msb = value;
}


/*    TC timer operations : isr_ctrl.h    */


static unsigned char tc_timer_overflow = 1;
/* Simulated overflow flag on the TC timer. */


unsigned char TC_Timer_Overflow_Flag (void)
{
#if defined(TRACE_HARNESS)
   printf ("TC_Timer_Overflow_Flag\n");
#endif
   return tc_timer_overflow;   /* 1 = timer overflow, good. TBA bad. */
}


void Clear_TC_Timer_Overflow_Flag (void)
{
#if defined(TRACE_HARNESS)
   printf ("Clear_TC_Timer_Overflow_Flag\n");
#endif
   tc_timer_overflow = 0;
}


void Set_TC_Timer_Overflow_Flag (void)
{
#if defined(TRACE_HARNESS)
   printf ("Set_TC_Timer_Overflow_Flag\n");
#endif
   tc_timer_overflow = 1;
}


/*    A/D Converter operations : ad_ctrl.h    */


#include "ad_conv.h"

static int ad_conv_timer = 0;
/* Simulate some delay in the conversion. */

static int ad_conv_delay = 5;
/* The conversion finishes when ad_conv_timer >= ad_conv_delay. */

#define AD_NUM_RAND 319
/* The amount of random A/D data defined below. */

static unsigned char ad_random[AD_NUM_RAND] = {
   0x6a, 0xde, 0xba, 0x90, 0xf2, 0x18, 0x48, 0xf3,
   0x9e, 0x2b, 0x31, 0xdb, 0xe0, 0x7e, 0xc6, 0x18,
   0x43, 0xd0, 0xd7, 0x6e, 0xbc, 0xee, 0x93, 0x9a,
   0x06, 0xb2, 0x3d, 0x1f, 0xc1, 0x51, 0x66, 0x69,
   0xbf, 0x1c, 0x9c, 0xfc, 0x9b, 0xf7, 0xf2, 0xd0,
   0xf4, 0x26, 0x60, 0x69, 0xc4, 0xd9, 0xdb, 0xd4,
   0xe7, 0x2b, 0x8a, 0xea, 0x9f, 0xab, 0x40, 0x3e,
   0xc3, 0xd8, 0x21, 0x61, 0x3b, 0x0f, 0xc1, 0x49,
   0xd3, 0x09, 0x9a, 0x4d, 0x33, 0x52, 0x7b, 0x8e,
   0x7e, 0x7b, 0x6a, 0x88, 0x4f, 0x84, 0xa2, 0xb4,
   0x83, 0xd9, 0xba, 0x79, 0x7d, 0x8f, 0xdf, 0xb2,
   0x8c, 0x86, 0x77, 0x4f, 0x29, 0x86, 0xd4, 0x8b,
   0x11, 0x65, 0x55, 0x74, 0xf4, 0x76, 0x83, 0x88,
   0xd6, 0xa6, 0xa7, 0x33, 0x22, 0xa3, 0x2e, 0x88,
   0x06, 0x54, 0x90, 0x37, 0xd5, 0xdb, 0xce, 0x7c,
   0x0b, 0xd1, 0xe1, 0xc0, 0x7d, 0xa5, 0x0b, 0xc9,
   0xaf, 0xe3, 0x75, 0xc5, 0xf5, 0xaf, 0xaa, 0xe2,
   0x2a, 0xff, 0x6e, 0x84, 0x0e, 0x04, 0x10, 0xf0,
   0x78, 0xdc, 0x96, 0x3d, 0x22, 0x96, 0x64, 0x5b,
   0x7b, 0x9e, 0x83, 0x45, 0xba, 0xb8, 0xe1, 0x31,
   0xc7, 0x0a, 0xe0, 0x31, 0xce, 0x29, 0x3d, 0x01,
   0xb8, 0xfc, 0x79, 0x83, 0x3d, 0xd1, 0x40, 0xe1,
   0x46, 0xfa, 0xe7, 0xc5, 0xdc, 0xc4, 0x1c, 0x24,
   0x29, 0x5a, 0xef, 0xeb, 0x92, 0x57, 0xba, 0x06,
   0x13, 0x1d, 0x35, 0xef, 0xb0, 0x2d, 0x69, 0x20,
   0x92, 0xb1, 0x82, 0x00, 0x8e, 0x3b, 0x12, 0xb3,
   0x78, 0xd7, 0x18, 0xb3, 0x54, 0x0f, 0xd1, 0x8e,
   0x88, 0x5d, 0x4e, 0x2b, 0x30, 0x30, 0x2d, 0x85,
   0xaa, 0x21, 0x01, 0xe1, 0x2c, 0x35, 0xa1, 0xee,
   0xa2, 0x17, 0xed, 0x60, 0x1b, 0x98, 0xea, 0x12,
   0x85, 0x21, 0xde, 0x45, 0x26, 0xef, 0x12, 0x3c,
   0x02, 0x8c, 0xd7, 0x49, 0xbd, 0x02, 0xa7, 0x7d,
   0xe7, 0x1c, 0x15, 0xf9, 0xaa, 0x44, 0x15, 0xb1,
   0xaa, 0x76, 0x5e, 0xf2, 0xb4, 0xfb, 0x85, 0x77,
   0xb9, 0x32, 0xb4, 0xc9, 0x70, 0xe1, 0xdb, 0x44,
   0x9f, 0x5b, 0x87, 0xca, 0xaa, 0xcb, 0x43, 0x53,
   0x7e, 0x49, 0xec, 0x1a, 0x13, 0x1d, 0xe1, 0x1b,
   0x13, 0xc3, 0x34, 0x95, 0x5d, 0x5a, 0xc3, 0xd0,
   0x33, 0x05, 0x82, 0x4a, 0x2e, 0x6d, 0x39, 0xeb,
   0x9c, 0x65, 0x81, 0x7f, 0xa1, 0x62, 0x11};
/* Random A/D data. */

static unsigned int ad_rand_index = 0;
/* A roving index to the random A/D data. */


void Update_ADC_Channel_Reg (unsigned char channel)
{
#if defined(TRACE_HARNESS)
   printf ("Update_ADC_Channel_Reg %d\n", channel);
#endif
}


void Start_Conversion (void)
{
#if defined(TRACE_HARNESS)
   printf ("Start_Conversion\n");
#endif
   ad_conv_timer = 0;
}


unsigned char End_Of_ADC (void)
{
#if defined(TRACE_HARNESS)
   printf ("End_Of_ADC\n");
#endif

   ad_conv_timer ++;


   if (ad_conv_timer >= ad_conv_delay)
   {
      if (ad_conv_delay > 0) ad_conv_delay --;
      /* Shorter delay next time. */

      return 0;  /* Conversion done. */
   }
   else
   {
      return 1;   /* Conversion still going on. */
   }
}


unsigned char Get_Result (void)
{
   unsigned char value;

   ad_rand_index ++;
   if (ad_rand_index >= AD_NUM_RAND) ad_rand_index = 0;

   value = ad_random[ad_rand_index];

#if defined(TRACE_HARNESS)
   printf ("Get_Result %d = 0x%x\n", value, value);
#endif

   return value;
}

void Set_DAC_Output (unsigned char level)
{
#if defined(TRACE_HARNESS)
   printf ("Set_DAC_Output %d\n", level);
#endif
}



/* The variable ADC_channel_register is defined in health.c. */


/*    Sensor Unit operations : su_ctrl.h, dpu_ctrl.h, isr_ctrl.h    */


#include "su_ctrl.h"

static unsigned char trigger_flag     = 1;
static unsigned char event_flag       = ACCEPT_EVENT;
static unsigned char trigger_source_0 = 0;
static unsigned char trigger_source_1 = 0;


unsigned char Hit_Trigger_Flag (void)
{
#if defined(TRACE_HARNESS)
   printf ("Hit_Trigger_Flag\n");
#endif
   return trigger_flag;  /* 1 is good (enabled, TBA bad (disabled). */
}


unsigned char Event_Flag (void)
{
#if defined(TRACE_HARNESS)
   printf ("Event_Flag \n");
#endif
   return event_flag;  /* TBA reject. */
}


unsigned char Get_MSB_Counter (void)
{
#if defined(TRACE_HARNESS)
   printf ("Get_MSB_Counter\n");
#endif
   return 134;  /* TBA */
}


unsigned char Get_LSB1_Counter  (void)
{
#if defined(TRACE_HARNESS)
   printf ("Get_LSB1_Counter\n");
#endif
   return 77;  /* TBA */
}


unsigned char Get_LSB2_Counter  (void)
{
#if defined(TRACE_HARNESS)
   printf ("Get_LSB2_Counter\n");
#endif
   return 88;  /* TBA */
}


unsigned char Rise_Time_Counter (void)
{
#if defined(TRACE_HARNESS)
   printf ("Rise_Time_Counter\n");
#endif
   return 102;  /* TBA */
}


unsigned char Trigger_Source_0 (void)
{
#if defined(TRACE_HARNESS)
   printf ("Trigger_Source_0\n");
#endif
   return trigger_source_0;  /* TBA */
}


unsigned char Trigger_Source_1 (void)
{
#if defined(TRACE_HARNESS)
   printf ("Trigger_Source_1\n");
#endif
   return trigger_source_1;  /* TBA */
}


unsigned char V_Down (void) 
{
#if defined(TRACE_HARNESS)
   printf ("V_Down\n");
#endif
   return 1;  /* 1 is good. TBA bad. */
}


void SignalPeakDetectorReset(
   unsigned char low_reset_value,
   unsigned char high_reset_value)
{
#if defined(TRACE_HARNESS)
   printf ("SignalPeakDetectorReset low %d, high %d\n",
      low_reset_value, high_reset_value);
#endif
}


/* The following operations are implemented in hw_if.c:
 * ReadDelayCounters
 * ReadRiseTimeCounter
 * ResetDelayCounters
 * ResetPeakDetector
 * SetTriggerLevel
 * SetTestPulseLevel
 * GetVoltageStatus
 * Switch_SU_On
 * Switch_SU_Off
 * EnableAnalogSwitch
 * DisableAnalogSwitch
 * SelectSelfTestChannel
 * SelectTriggerSwitchLevel
 * SelectStartSwitchLevel
 */


/*    Accessing unaligned tm_ushort_t data   */


unsigned short int Short_Value (tm_ushort_t *x)
{
   unsigned short val;
   memcpy (&val, x, sizeof (val));
   return val;
}


/*    "Calling" a "patch function"   */


void Call_Patch (fptr_t func)
{
#if defined(TRACE_HARNESS)
   printf ("Call_Patch 0x%x\n", (code_address_t)func);
#endif
}


/*    Checking the sizes of some types    */

#include "tm_data.h"

static volatile uint_least8_t  tsize_s;
static volatile uint_least16_t tsize_l;

void Check_Type_Size (void)
/*
 * Disassemble the code of this function to see the sizes of
 * the types char, short, ..., science_data_file_t.
 * This function is never called.
 *
 */
{
   tsize_s = sizeof (char );
   tsize_s = sizeof (short);
   tsize_s = sizeof (int  );
   tsize_s = sizeof (long );

   tsize_s = sizeof (tm_ushort_t     );
   tsize_s = sizeof (dpu_time_t      );
   tsize_s = sizeof (tm_dpu_time_t   );
   tsize_s = sizeof (SU_settings_t   );
   tsize_s = sizeof (telemetry_data_t);

   tsize_s = sizeof (event_record_t     );
   tsize_l = sizeof (science_data_file_t);
}


/*   Test scenarios    */

/*
These test cases are not intended as a full functional test.
They are intended to provide sufficient coverage for measurement-
based WCET analysis of the tested code. Built-in checks of test
execution (program state) are used only to check that the DEBIE
software has done the expected thing in each test case, but the
checks are definitely not exhaustive.
*/


#if defined(TRACE_HARNESS)
#define CASE(TXT) printf ("\nCASE: %s:\n\n", TXT)
#else
#define CASE(TXT)
#endif


static int check_errors = 0;
/* Counts the number of errors found in Check/Zero/Nonzero. */


static void Check_Zero (int cond)
/* Checks that cond == 0. */
{
   if (cond)
   {
      check_errors ++;
#if defined(TRACE_HARNESS)
      printf ("Check_Zero: error\n");
#endif
   }
}


static void Check_Nonzero (int cond)
/* Checks that cond != 0. */
{
   if (cond == 0)
   {
      check_errors ++;
#if defined(TRACE_HARNESS)
      printf ("Check_Nonzero: error\n");
#endif
   }
}

#define Check Check_Nonzero
/* Check that the (boolean) parameter is true (not zero). */


static void Check_No_Errors (void)
/* Checks that no errors are flagged in telemetry_data.error_status. */
{
   Check_Zero (telemetry_data.error_status);
}


static void Send_TC (unsigned char address, unsigned char code)
/* Invokes TC_InterruptService with a TC composed of the
 * given address and code, provided with valid (even) parity.
 */
{
   unsigned char par;
   /* The parity. */

   /* Encode the address and code in the TC interface registers: */

   tc_msb = address << 1;
   tc_lsb = code;

   /* Generate the even parity bit: */

   par = tc_msb ^ tc_lsb;
   par = (par & 0x0F) ^ (par >>4);
   par = (par & 0x03) ^ (par >>2);
   par = (par & 0x01) ^ (par >>1);

   tc_msb |= par;

   /* Invoke the TC ISR: */

   tc_timer_overflow = 1;
   TC_InterruptService();
}


static void TC_ISR_Tests (void)
/* Test of TC_InterruptService. */
{

#if defined(TRACE_HARNESS)
   printf ("\nTC_ISR_Tests\n");
#endif

   CASE("TC rejected because timer overflow is not set");

   tc_timer_overflow = 0;
   TC_InterruptService();

   CASE("TC rejected because TC_state is SC_TM_e");

   TC_state = SC_TM_e;
   Send_TC (0,0);

   CASE("TC rejected because TC_state is memory_dump_e");

   TC_state = memory_dump_e;
   Send_TC (0,0);

   CASE("TC in TC_state = memory_patch_e");

   TC_state = memory_patch_e;
   Send_TC (0,0);
   Check (mail_count[TCTM_MAILBOX] == 1);
   FlushMail (TCTM_MAILBOX);

   CASE("TC with parity error");

   TC_state = TC_handling_e;
   tc_timer_overflow = 1;
   tc_msb = 0; tc_lsb = 1;
   TC_InterruptService();

   Check_Nonzero (telemetry_data.error_status & PARITY_ERROR);

   CASE("TC = ERROR_STATUS_CLEAR, ok");

   Send_TC (ERROR_STATUS_CLEAR, ERROR_STATUS_CLEAR);

   /* The parity-error flag is not yet reset, because */
   /* the TC was not yet executed:                    */

   Check_Nonzero (telemetry_data.error_status & PARITY_ERROR);

   Check (mail_count[TCTM_MAILBOX] == 1);
   FlushMail (TCTM_MAILBOX);

   /* Clear the error manually: */

   telemetry_data.error_status = 0;

   Check_No_Errors ();

   CASE("TC = SEND_STATUS_REGISTER, ok");

   Send_TC (SEND_STATUS_REGISTER, 8);

   Check_No_Errors ();
   Check (TC_state == register_TM_e);
   Check (mail_count[TCTM_MAILBOX] == 1);
   FlushMail (TCTM_MAILBOX);

/* TBA:
TC in TC_state = register_TM_e.
TC invalid (ALL_INVALID)
TC valid (ALL_VALID)
TC address != TC code (ONLY_EQUAL)
TC code not ON, OFF, SELF_TEST (ON_OFF_TC)
TC code odd (ONLY_EVEN)
TC code even but too large (ONLY_EVEN)
*/
}


static void Exec_TC (unsigned char address, unsigned char code)
/* Invokes TC_InterruptService with a TC composed of the
 * given address and code, provided with valid (even) parity,
 * then invokes HandleTelecommand to execute the TC.
 */
{
   Send_TC (address, code);
   Check (mail_count[TCTM_MAILBOX] == 1);
   HandleTelecommand ();
   Check (mail_count[TCTM_MAILBOX] == 0);
}

static void TC_Task_Tests (void)
/* Test of TelecommandExecutionTask. */
{

#if defined(TRACE_HARNESS)
   printf ("\nTC_Task_Tests\n");
#endif

   CASE("TC = ERROR_STATUS_CLEAR, ok");

   /* Flag an error manually: */
   telemetry_data.error_status = PARITY_ERROR;

   Send_TC (ERROR_STATUS_CLEAR, ERROR_STATUS_CLEAR);

   /* The parity-error flag is not yet reset, because */
   /* the TC was not yet executed:                    */

   Check_Nonzero (telemetry_data.error_status & PARITY_ERROR);

   HandleTelecommand ();

   /* Now the parity-error flag is reset: */

   Check_No_Errors ();

/* TBA:

WaitMail timeout
TM_READY message
each TC

*/

}


void TM_Tests (void)
/* Test of TM_InterruptService and telemetry functionality. */
{
   int octets;
   /* Number of octets sent and acknowledge by TM interrupt. */

#if defined(TRACE_HARNESS)
   printf ("\nTM_ISR_Tests\n");
#endif

   CASE("One whole round of register TM");

   Send_TC (SEND_STATUS_REGISTER, 0);

   Check_No_Errors ();
   Check (TC_state == register_TM_e);
   Check (mail_count[TCTM_MAILBOX] == 1);

   HandleTelecommand ();
   Check (TC_state == register_TM_e);
   Check (mail_count[TCTM_MAILBOX] == 0);

   for (octets = 0; octets < sizeof (telemetry_data_t); octets += 2)
   {
      TM_InterruptService ();
      Check (TC_state == register_TM_e);
   }

   CASE("Science Data TM");

   Send_TC (SEND_SCIENCE_DATA_FILE, SEND_SCIENCE_DATA_FILE);

   Check_No_Errors ();
   Check (TC_state == SC_TM_e);
   Check (mail_count[TCTM_MAILBOX] == 1);

   HandleTelecommand ();
   Check (TC_state == SC_TM_e);
   Check (mail_count[TCTM_MAILBOX] == 0);

   /* Absorb TM until a TM_READY message is sent to the TC task: */

   octets = 0;
   while (mail_count[TCTM_MAILBOX] == 0)
   {
      TM_InterruptService ();
      octets += 2;
      Check (TC_state == SC_TM_e);
   }

#if defined(TRACE_HARNESS)
   printf ("Science TM octets sent %d\n", octets);
#endif

   HandleTelecommand ();
   Check (TC_state == TC_handling_e);
   Check (mail_count[TCTM_MAILBOX] == 0);


/* TBA:
TM bytes left to send (pointer < end_pointer)
Start or wrap-around of register TM
End of memory-dump TM
Stop TM (mail to TCTM_MAILBOX)
*/
}


static void Trigger_Hit (sensor_index_t SU)
/* Invoke HandleHitTrigger with the given SU in trigger_source_0/1. */
{
   trigger_source_0 = SU        & 1;
   trigger_source_1 = (SU >> 1) & 1;

   Check (mail_count[ACQUISITION_MAILBOX] == 0);

#if defined(TRACE_HARNESS)
   printf ("Hit!\n");
#endif

   HandleHitTrigger ();

#if defined(TRACE_HARNESS)
   if (mail_count[ACQUISITION_MAILBOX] == 0)
      printf ("- hit rejected\n");
   else
      printf ("- hit accepted\n");
#endif

}


static void Acquire_Hit (sensor_index_t SU)
/* Invoke HandleHitTrigger with the given SU in trigger_source_0/1
 * followed by HandleAcquisition if the hit was accepted.
 */
{
   Trigger_Hit (SU);

   if (mail_count[ACQUISITION_MAILBOX] > 0)
      HandleAcquisition ();
}


static void Hit_ISR_Tests (void)
/* Test of HandleHitTrigger. */
{

#if defined(TRACE_HARNESS)
   printf ("\nHit_ISR_Tests\n");
#endif

   CASE("Hit Trigger, budget exhausted");

      Check (telemetry_data.hit_budget_exceedings == 0);

      hit_budget_left = 0;

      Trigger_Hit (0);

      Check (mail_count[ACQUISITION_MAILBOX] == 0);

      Check (telemetry_data.hit_budget_exceedings == 1);

   CASE("TC = SWITCH_SU_1 ON");

      Check (SU_state[0] == off_e);

      Exec_TC (SWITCH_SU_1, ON_VALUE);

      Check_No_Errors ();

      Check (SU_state[0] == start_switching_e);

   CASE("TC = SWITCH_SU_3 ON");

      Check (SU_state[2] == off_e);

      Exec_TC (SWITCH_SU_3, ON_VALUE);

      Check_No_Errors ();

      Check (SU_state[2] == start_switching_e);

   CASE("Health Monitoring drives SU to ON");

      HandleHealthMonitoring ();

      Check (SU_state[0] == switching_e);
      Check (SU_state[2] == switching_e);

      HandleHealthMonitoring ();

      Check (SU_state[0] == on_e);
      Check (SU_state[2] == on_e);

   CASE("TC = START_ACQUISITION");

      Check ((telemetry_data.mode_status & MODE_BITS_MASK) == STAND_BY);

      Exec_TC (START_ACQUISITION, START_ACQUISITION);

      Check_No_Errors ();

      Check ((telemetry_data.mode_status & MODE_BITS_MASK) == ACQUISITION);
      Check (SU_state[0] == acquisition_e);
      Check (SU_state[2] == acquisition_e);

   CASE("Hit Trigger, budget left");

      free_slot_index = 0;
      hit_budget_left = 15;

      Trigger_Hit (0);

      Check (mail_count[ACQUISITION_MAILBOX] == 1);

      HandleAcquisition ();

      Check (free_slot_index == 1);

   CASE("Hit Triggers until budget exhausted");

      do {
         Acquire_Hit (2);
         Acquire_Hit (0);
         }
      while (hit_budget_left > 0);

      Acquire_Hit (2);  /* This hit rejected. */



   return;

/* TBA:
Hit budget exhausted.
Self Test mode, incorrect pulse.
Self Test mode, correct pulse.
Measurement mode (not Self Test).
ADC error (try-count exceeed)
ADC success
*/
}



/*   StartSystem () : test scenario    */


void StartSystem(unsigned char task_number)
/*   ORIGINALLY:                                                             */
/* Purpose        : Starts the system.                                       */
/* Interface      : input  - none                                            */
/*                  output - none                                            */
/* Preconditions  : none                                                     */
/* Postconditions : First task is called and system started.                 */
/* Algorithm      : See below, self explanatory.                             */
/*   IN HARNESS:                                                             */
/* Executes the test scenario.                                               */
{
#if defined(TRACE_HARNESS)
   printf ("StartSystem %d\n", task_number);
#endif

   /* Initialize the global data of the tasks: */

   InitHealthMonitoring ();

   /* Testing the ISRs and tasks: */

   TC_ISR_Tests ();

   TC_Task_Tests ();

   TM_Tests ();

   Hit_ISR_Tests ();

   /* Check that no errors occurred: */

#if defined(TRACE_HARNESS)
   printf ("Total check errors %d\n", check_errors);
   printf ("Total mailbox overflows %d\n", mail_overflows);
   exit (0);
#endif

}
