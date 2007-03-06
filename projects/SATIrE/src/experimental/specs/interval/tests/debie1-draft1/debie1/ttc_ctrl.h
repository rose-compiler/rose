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
 *    Module     :   ttc_ctrl.h
 *
 * Macros and function prototypes for handling the Telecommand
 * and Telemetry interface.
 *
 * Based on the SSF DHI file ttc_ctrl.h, rev 1.11,  Sun May 16 09:20:10 1999.
 *
 *- * --------------------------------------------------------------------------
 */


#ifndef TTC_CTRL_H
#define TTC_CTRL_H

/* TC/TM interface functions, simulated */

extern unsigned char Read_TC_MSB (void);
extern unsigned char Read_TC_LSB (void);
extern void          Write_TM_LSB (unsigned char value);
extern void          Write_TM_MSB (unsigned char value);

/* TC and TM register handling */

#define READ_TC_MSB Read_TC_MSB()
#define READ_TC_LSB Read_TC_LSB()

#define WRITE_TM_LSB(TM_LSB) Write_TM_LSB(TM_LSB)
#define WRITE_TM_MSB(TM_MSB) Write_TM_MSB(TM_MSB)

/* TM Interrupt flag */

#define CLEAR_TM_INTERRUPT_FLAG {}

/* TC Interrupt flag*/
#define CLEAR_TC_INTERRUPT_FLAG {}

/*TM and TC interrupt controls*/

#define SET_INT_TYPE1_EDGE {}
#define SET_INT_TYPE0_EDGE {}

/*TM interrupt service handling */
#define TM_ISR_MASK  0x04

/* Error Status register bits concerning TM/TC interface */

#define PARITY_ERROR 2
#define TC_ERROR     1

#define TC_OR_PARITY_ERROR (TC_ERROR + PARITY_ERROR) 

#define TC_INT_MIN_INTERVAL (10000000 / 1085)
/* Minimum interval between two consecutive telecommands */
/* in machine cycles : 10000000 ns  / (1085 ns / cycle)  */

#define TC_TIMER_LSB  ((0xFFFF - TC_INT_MIN_INTERVAL) & 0xFF)
#define TC_TIMER_MSB  ((0xFFFF - TC_INT_MIN_INTERVAL) >> 8)
/* MSB and LSB of the initial value for TC interval timer */

#define SET_TC_TIMER_MODE  {}
/* Set TC timer (0) mode : Mode 1, counter operation, SW control */

#define INIT_TC_TIMER_MSB  {}
#define INIT_TC_TIMER_LSB  {}
/* TC timer initialization macros */ 

#define START_TC_TIMER     {}
#define STOP_TC_TIMER      {}
/* TC timer run control macros    */

#endif



