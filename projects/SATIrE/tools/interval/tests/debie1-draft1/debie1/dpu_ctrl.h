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
 *    Module     :   dpu_ctrl.h
 *
 * Operations and macros for low-level control of the Data Processing
 * Unit, the 80C32 computer on which the DEBIE DPU software runs.
 * This includes accessing data and code memories by address; boot
 * and reset operations; watchdog handling; memory patch and test.
 *
 * Based on the SSF DHI file dpu_ctrl.h, rev 1.23, Fri May 28 14:59:30 1999.
 *      
 *- * --------------------------------------------------------------------------
 */

#ifndef DPU_CTRL_H
#define DPU_CTRL_H

#include "keyword.h"

#define SAME 1
#define NOT_SAME 0

#define MEMORY_PATCHED 1
#define MEMORY_NOT_PATCHED 0

#define SELECTED 1
#define NOT_SELECTED 0
#define RESET_OK 1
#define RESET_NOT_OK 0 

#define ACCEPT_EVENT 1
#define REJECT_EVENT 0

extern unsigned char Event_Flag (void);

#define EVENT_FLAG Event_Flag()
/* Event storage condition signal. */

/* memory addresses for program copy                                         */
#define PROGRAM_COPY_START 0x1000
#define PROGRAM_COPY_END   0x8000
/* this can be replaced with real end address (+1) of used program code      */
/* given in the linker map file                                              */

/* Comment or delete following definition, if program should be executed     */
/* from RAM                                                                  */
/* #define USE_ALWAYS_PROM */

/* memory addresses for patching                                             */
#define BEGIN_SRAM1       0x1000
#define END_SRAM1         0x7FFF
#define BEGIN_SRAM3       0x8000
#define END_SRAM3         0xFEFF
#define BEGIN_DATA_RAM    0x0000


#define SCIENCE_DATA_START_ADDRESS 0x0000
/* First free absolute data address. */

#define INITIAL_CHECKSUM_VALUE   0
/* A value given to 'reference_checksum' variable at 'Boot()'. */
/* It is zero, since one code byte is dedicated to a constant  */
/* that ensures that the checksum of the PROM is zero.         */


/* Access simulated memory */

extern void          Set_Data_Byte (data_address_t addr, unsigned char value);
extern unsigned char Get_Data_Byte (data_address_t addr);
extern unsigned char Get_Code_Byte (code_address_t addr);


/* Functions and macros to access external memory by numerical address. */

#define SET_DATA_BYTE(ADDR,VALUE) Set_Data_Byte (ADDR, VALUE)
#define GET_DATA_BYTE(ADDR)       Get_Data_Byte (ADDR)
#define GET_CODE_BYTE(ADDR)       Get_Code_Byte (ADDR)
#define DATA_POINTER(ADDR) ((EXTERNAL unsigned char *)(ADDR))


/* Controlling the watchdog and the memory mapping: */

#define SET_WD_RESET_HIGH  {}
#define SET_WD_RESET_LOW   {}
#define SET_MEM_CONF_PROM  {}
#define SET_MEM_CONF_SRAM  {}


/* macro used in healthmonitoring */

extern unsigned char Check_Current (unsigned char bits);


#define CHECK_CURRENT(BIT_NUMBERS) Check_Current (BIT_NUMBERS)
/* Checks whether given bit in the HV Status Register is HIGH or LOW. */



/*type definitions*/

typedef unsigned char DEBIE_mode_t;
/* Debie mode index. Valid values:  */
/* 00 DPU self test                 */
/* 01 Stand by                      */
/* 10 Acquisition                   */


typedef enum {
   power_up_reset_e = 0, /* Don't change value ! */
   watchdog_reset_e = 1, /* Don't change value ! */
   soft_reset_e, 
   warm_reset_e, 
   error_e,
   checksum_reset_e
}  reset_class_t;

extern reset_class_t EXTERNAL s_w_reset;

typedef enum  {
   PROM_e, SRAM_e
} memory_configuration_t;


#define MAX_TIME 0xFFFFFFFF
/* Maximum value for DEBIE time. */

typedef uint32_t dpu_time_t;
/* Data type for DEBIE time. */

/*-------------------------------------------------------*/
/* Multi-byte TM types without alignment constraints.    */
/* The Keil/8051 system does not constrain alignment for */
/* multi-byte types such as long int, but the Unix test  */
/* systems do, and we must declare the TM structures to  */
/* avoid constraints.                                    */
/* Note that we still assume that the two systems have   */
/* the same endianness.                                  */
/* Note also that the native type is used in the TM when */
/* the component happens to have the correct alignment.  */
/* The special types are used only when the alignment is */
/* otherwise incorrect.                                  */
/* Data is moved between the native type and the special */
/* TM type only using the COPY or VALUE_OF macros. Thus, */
/* on the 8051 the two types can in fact be identical,   */
/* with COPY defined as direct assignment ("=").         */

typedef uskew32_t tm_dpu_time_t;
/* The TM type corresponding to dpu_time_t (32-bit int). */

typedef uskew16_t tm_ushort_t;
/* The TM type corresponding to unsigned short int.      */
/*                                                       */
/*-------------------------------------------------------*/


typedef struct {
   unsigned char *source;
   data_address_t destination;
   uint_least8_t  data_amount;
   unsigned char  execution_command;
} memory_patch_variables_t;
/* Holds parameters for PatchCode function:                         */
/*    source             source address of the patch                */
/*                       (should be between 0x8000 and 0xFF00 -     */
/*                        data_amount)                              */
/*    destination        destination address of the patch           */
/*                       (should be between 0x1000 and 0x8000 -     */
/*                        data_amount)                              */
/*    data_amount        amount of bytes to be patched (max 255)    */
/*    execution_command  action executed after patch                */
/*                          0x00 - continue normally                */
/*                          0x09 - execute soft reset               */
/*                          0x37 - execute warm reset               */
/*                          0x5A - jump to start address of patched */
/*                                 memory area                      */

extern unsigned char EXTERNAL code_not_patched;
/* Initial value is 1, set at Boot(). Value is 1 when code   */
/* checksum value is valid, cleared to 0 when code memory is */
/* patched, set to 1 when next checksum calculation          */
/* period is started.                                        */

extern unsigned char EXTERNAL reference_checksum;
/* Expected value for code checksum. Will be changed when   */
/* code memory is patched.                                  */


/* Function prototypes: */

extern void Init_DPU (reset_class_t reset_class);

extern reset_class_t GetResetClass(void);

extern void SignalMemoryErrors (void);

extern void SetMemoryConfiguration (memory_configuration_t memory);

extern memory_configuration_t GetMemoryConfiguration(void)
   COMPACT REENTRANT_FUNC; 

extern void PatchCode(memory_patch_variables_t EXTERNAL *patch_variables);  

extern void Reboot(reset_class_t boot_type);


/* Assembly-language function prototypes (asmfuncs.a51): */

extern unsigned char TestMemBits (data_address_t address);

extern unsigned char TestMemData (
                data_address_t start,
                uint_least8_t  bytes);

extern unsigned char TestMemSeq (
                data_address_t start,
                uint_least8_t  bytes);

#endif
