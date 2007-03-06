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
 *    Module     :   keyword.h
 *
 * Macro definitions for Keil specific keywords to be used
 * in portable parts of the DEBIE DPU software.
 *
 * This version adapted to the GNU ARM compiler for the ARM7, no kernel.
 *
 * Based on the SSF DHI file keyword.h, revision 1.9, Tue Mar 09 12:37:20 1999.
 *
 *- * --------------------------------------------------------------------------
 */


#ifndef KEYWORD_H
#define KEYWORD_H

#include <string.h>
/* For memcpy (). */


/* Integer type definitions for native types that can hold integer
 * values of at least a given number of bits. These types are used
 * as loop counters to give the most natural and speedy code for
 * the current target. This is Not the C99 stdint.h, but the types
 * have similar names.
 *
 * There is a particular form of this file for each (kind of) target
 * processor. The present form is for ARM7/GCC, where the widths of
 * the integer types are the following (as observed from the code of
 * the function Check_Type_Size in harness.c):
 *
 *    Type     Octets  Bits
 *    char       1      8
 *    short      2     16
 *    int        4     32
 *    long       4     32
 *
 * This processor/compiler also has alignment concerns, so here we
 * define all telemetry data as octets and access it using memcpy()
 * instead of direct assignment.
 *
 */

/* General types */

typedef int int_least8_t;
/* A signed integer covering at least -128 .. +127. */

typedef unsigned int uint_least8_t;
/* An unsigned integer covering at least 0 .. 255. */

typedef unsigned short uint16_t;
/* An unsigned 16-bit integer. */

typedef int int_least16_t;
/* A signed integer covering at least -2**15 .. +2**15 - 1. */

typedef unsigned int uint_least16_t;
/* An unsigned integer covering at least 0 .. 2**16 - 1. */

typedef uint16_t uskew16_t;
/* A 16-bit type, perhaps not an integer, perhaps not 16-bit aligned. */
/* If it is an integer type, it is unsigned. */

typedef unsigned int uint32_t;
/* An unsigned 32-bit integer. */

typedef uint32_t uskew32_t;
/* A 32-bit type, perhaps not an integer, perhaps not 32-bit aligned. */
/* If it is an integer type, it is unsigned. */


/* DEBIE-specific types */

typedef uint16_t data_address_t;
/* An address into external data memory. */

typedef uint16_t code_address_t;
/* An address into code memory. */


/* Macros for accessing and copying multi-octet data.      */
/* These may need target-specific adjustment if there are  */
/* alignment restrictions on multi-octet integer values,   */
/* because the operands in these macros may not be aligned */
/* in the required way.                                    */

#define COPY(DEST,SOURCE) memcpy (&(DEST), &(SOURCE), sizeof(DEST))
/* Copies the value of SOURCE to the location DEST. */

extern unsigned short Short_Value (uskew16_t *x);

#define VALUE_OF(SOURCE) Short_Value (&(SOURCE))
/* Returns the (integer) value of SOURCE, type uskew16_t. */

/* Macros for struct (aggregate) assignment. Some compilers */
/* may not support assignment statements for such types.    */

#define STRUCT_ASSIGN(DEST,SOURCE,TYPE) DEST = SOURCE

/* Macros for calling "patch" functions */

typedef code_address_t fptr_t;
/* A function that is to be called after patching code memory. */
/* The function may or may not be part of the patched code.    */
/* In the real SW this is "typedef void (*fptr_t)(void);"      */

extern void Call_Patch (fptr_t func);
/* "Call" the patch func. */

#define CALL_PATCH(FUNCTION) Call_Patch (FUNCTION)
/* Jump to the patched memory. */

/* Some macros for task and interrupt management */

#define TASK(TASK_NUMBER)    
#define PRIORITY(LEVEL)      
#define INTERRUPT(SOURCE)    
#define USED_REG_BANK(BANK)  

/* Macro for declaring re-entrant function */

#define REENTRANT_FUNC

/* Memory model handling macros */

#define COMPACT_DATA
#define COMPACT

#define PROGRAM

#define EXTERNAL          
#define DIRECT_INTERNAL   
#define INDIRECT_INTERNAL 

#define LOCATION(ADDRESS)

#endif



