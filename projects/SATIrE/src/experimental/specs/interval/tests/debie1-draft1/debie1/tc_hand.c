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
 *    Module     :   tc_hand.c
 *
 * Telecommand module.
 *
 * Based on the SSF file tc_hand.c, rev 1.66, Thu Sep 09 13:29:18 1999.
 *      
 *- * --------------------------------------------------------------------------
 */

#include "keyword.h"
#include "kernobj.h"
#include "tm_data.h"
#include "isr_ctrl.h"
#include "msg_ctrl.h"
#include "tc_hand.h"
#include "telem.h"
#include "ttc_ctrl.h"
#include "measure.h"
#include "dpu_ctrl.h"
#include "health.h"
#include "class.h"
#include "taskctrl.h"

/* This file contains functions to implement Telecommand Execution Task */

#define TC_ADDRESS(TC_WORD) ((TC_WORD) >> 9)
#define TC_CODE(TC_WORD)    ((TC_WORD) &  255)

#define SET_TIME_TC_TIMEOUT 100
/* Timeout between Set Time telecommands, 100 x 10ms = 1s */

#define MEM_BUFFER_SIZE     32

/* Possible TC look-up table values:                         */

#define ALL_INVALID   0 
/* used for invalid TC addresses                             */

#define ALL_VALID     1 
/* used for TCs which accept all TC codes                    */

#define ONLY_EQUAL    2 
/* used for TCs which require equal TC address and code      */

#define ON_OFF_TC     3
/* used for TCs which require ON, OFF or SELF_TEST parameter */

#define ONLY_EVEN     4
/* used currently only for SEND_STATUS_REGISTER TC           */


#define MEMORY_WRITE_ERROR 0x10
/* Memory write error bit in Mode Status. */

#define WRITE_MEMORY_TIMEOUT 100
/* Timeout 100 x 10 ms = 1s. */


/* Array */

SU_settings_t EXTERNAL * EXTERNAL SU_config[] = {
         &telemetry_data.sensor_unit_1,
         &telemetry_data.sensor_unit_2,
         &telemetry_data.sensor_unit_3,
         &telemetry_data.sensor_unit_4
      };

/* Type definitions */

typedef struct {
   uint16_t      TC_word;       /* Received telecommand word */
   unsigned char TC_address;    /* Telecommand address       */
   unsigned char TC_code;       /* Telecommand code          */
   } telecommand_t;

typedef enum {
   code_e, data_e
   } memory_type_t;

/* Global variables */

telecommand_t EXTERNAL previous_TC;
/* Holds previous telecommand unless a timeout has occurred */

unsigned char EXTERNAL TC_timeout = 0;
/* Time out for next telecommand, zero means no timeout */


unsigned char EXTERNAL TC_look_up[128];
/* Look-up table for all possible 128 TC address values */

TC_state_t EXTERNAL TC_state;
/* internal state of the Telecommand Execution task */

memory_type_t EXTERNAL memory_type;
/* Selection of memory write target (code/data) */ 

unsigned char EXTERNAL memory_transfer_buffer[MEM_BUFFER_SIZE];
/* Buffer for memory read and write telecommands */

unsigned char EXTERNAL address_MSB;
/* MSB of memory read source and write destination addresses */

unsigned char EXTERNAL address_LSB;
/* LSB of memory read source and write destination addresses. */

uint_least8_t EXTERNAL memory_buffer_index = 0;
/* Index to memory_buffer. */

unsigned char EXTERNAL write_checksum;
/* Checksum for memory write blocks. */


void InitTC_LookUp(void)
/* Purpose        : Initializes the TC look-up table                     */
/* Interface      : inputs  - none                                       */
/*                  outputs - TC_lool_up                                 */
/* Preconditions  : none                                                 */
/* Postconditions : TC_look_up is initialized                            */
/* Algorithm      : - set all elements in table to ALL_INVALID           */
/*                  - set each element corresponding valid TC address    */
/*                    to proper value                                    */
{
   DIRECT_INTERNAL uint_least8_t i;
   /* Loop variable */


   for(i=0; i<128; i++) TC_look_up[i] = ALL_INVALID;

   TC_look_up[START_ACQUISITION]            = ONLY_EQUAL;
   TC_look_up[STOP_ACQUISITION]             = ONLY_EQUAL;

   TC_look_up[ERROR_STATUS_CLEAR]           = ONLY_EQUAL;

   TC_look_up[SEND_STATUS_REGISTER]         = ONLY_EVEN;
   TC_look_up[SEND_SCIENCE_DATA_FILE]       = ONLY_EQUAL;

   TC_look_up[SET_TIME_BYTE_0]              = ALL_VALID;
   TC_look_up[SET_TIME_BYTE_1]              = ALL_VALID;
   TC_look_up[SET_TIME_BYTE_2]              = ALL_VALID;
   TC_look_up[SET_TIME_BYTE_3]              = ALL_VALID;

   TC_look_up[SOFT_RESET]                   = ONLY_EQUAL;

   TC_look_up[CLEAR_WATCHDOG_FAILURES]      = ONLY_EQUAL;
   TC_look_up[CLEAR_CHECKSUM_FAILURES]      = ONLY_EQUAL;

   TC_look_up[WRITE_CODE_MEMORY_MSB]        = ALL_VALID;
   TC_look_up[WRITE_CODE_MEMORY_LSB]        = ALL_VALID;
   TC_look_up[WRITE_DATA_MEMORY_MSB]        = ALL_VALID;
   TC_look_up[WRITE_DATA_MEMORY_LSB]        = ALL_VALID;
   TC_look_up[READ_DATA_MEMORY_MSB]         = ALL_VALID;
   TC_look_up[READ_DATA_MEMORY_LSB]         = ALL_VALID;

   TC_look_up[SWITCH_SU_1]                  = ON_OFF_TC;
   TC_look_up[SWITCH_SU_2]                  = ON_OFF_TC;
   TC_look_up[SWITCH_SU_3]                  = ON_OFF_TC;
   TC_look_up[SWITCH_SU_4]                  = ON_OFF_TC;

   TC_look_up[SET_SU_1_PLASMA_1P_THRESHOLD] = ALL_VALID;
   TC_look_up[SET_SU_2_PLASMA_1P_THRESHOLD] = ALL_VALID;
   TC_look_up[SET_SU_3_PLASMA_1P_THRESHOLD] = ALL_VALID;
   TC_look_up[SET_SU_4_PLASMA_1P_THRESHOLD] = ALL_VALID;

   TC_look_up[SET_SU_1_PLASMA_1M_THRESHOLD] = ALL_VALID;
   TC_look_up[SET_SU_2_PLASMA_1M_THRESHOLD] = ALL_VALID;
   TC_look_up[SET_SU_3_PLASMA_1M_THRESHOLD] = ALL_VALID;
   TC_look_up[SET_SU_4_PLASMA_1M_THRESHOLD] = ALL_VALID;

   TC_look_up[SET_SU_1_PIEZO_THRESHOLD]     = ALL_VALID;
   TC_look_up[SET_SU_2_PIEZO_THRESHOLD]     = ALL_VALID;
   TC_look_up[SET_SU_3_PIEZO_THRESHOLD]     = ALL_VALID;
   TC_look_up[SET_SU_4_PIEZO_THRESHOLD]     = ALL_VALID;

   TC_look_up[SET_SU_1_PLASMA_1P_CLASS_LEVEL] = ALL_VALID;
   TC_look_up[SET_SU_2_PLASMA_1P_CLASS_LEVEL] = ALL_VALID;
   TC_look_up[SET_SU_3_PLASMA_1P_CLASS_LEVEL] = ALL_VALID;
   TC_look_up[SET_SU_4_PLASMA_1P_CLASS_LEVEL] = ALL_VALID;
 
   TC_look_up[SET_SU_1_PLASMA_1M_CLASS_LEVEL] = ALL_VALID;
   TC_look_up[SET_SU_2_PLASMA_1M_CLASS_LEVEL] = ALL_VALID;
   TC_look_up[SET_SU_3_PLASMA_1M_CLASS_LEVEL] = ALL_VALID;
   TC_look_up[SET_SU_4_PLASMA_1M_CLASS_LEVEL] = ALL_VALID;
 
   TC_look_up[SET_SU_1_PLASMA_2P_CLASS_LEVEL] = ALL_VALID;
   TC_look_up[SET_SU_2_PLASMA_2P_CLASS_LEVEL] = ALL_VALID;
   TC_look_up[SET_SU_3_PLASMA_2P_CLASS_LEVEL] = ALL_VALID;
   TC_look_up[SET_SU_4_PLASMA_2P_CLASS_LEVEL] = ALL_VALID;
 
   TC_look_up[SET_SU_1_PIEZO_1_CLASS_LEVEL]   = ALL_VALID;
   TC_look_up[SET_SU_2_PIEZO_1_CLASS_LEVEL]   = ALL_VALID;
   TC_look_up[SET_SU_3_PIEZO_1_CLASS_LEVEL]   = ALL_VALID;
   TC_look_up[SET_SU_4_PIEZO_1_CLASS_LEVEL]   = ALL_VALID;
 
   TC_look_up[SET_SU_1_PIEZO_2_CLASS_LEVEL]   = ALL_VALID;
   TC_look_up[SET_SU_2_PIEZO_2_CLASS_LEVEL]   = ALL_VALID;
   TC_look_up[SET_SU_3_PIEZO_2_CLASS_LEVEL]   = ALL_VALID;
   TC_look_up[SET_SU_4_PIEZO_2_CLASS_LEVEL]   = ALL_VALID;

   TC_look_up[SET_SU_1_PLASMA_1E_1I_MAX_TIME]  = ALL_VALID;
   TC_look_up[SET_SU_2_PLASMA_1E_1I_MAX_TIME]  = ALL_VALID;
   TC_look_up[SET_SU_3_PLASMA_1E_1I_MAX_TIME]  = ALL_VALID;
   TC_look_up[SET_SU_4_PLASMA_1E_1I_MAX_TIME]  = ALL_VALID;
 
   TC_look_up[SET_SU_1_PLASMA_1E_PZT_MIN_TIME] = ALL_VALID;
   TC_look_up[SET_SU_2_PLASMA_1E_PZT_MIN_TIME] = ALL_VALID;
   TC_look_up[SET_SU_3_PLASMA_1E_PZT_MIN_TIME] = ALL_VALID;
   TC_look_up[SET_SU_4_PLASMA_1E_PZT_MIN_TIME] = ALL_VALID;
 
   TC_look_up[SET_SU_1_PLASMA_1E_PZT_MAX_TIME] = ALL_VALID;
   TC_look_up[SET_SU_2_PLASMA_1E_PZT_MAX_TIME] = ALL_VALID;
   TC_look_up[SET_SU_3_PLASMA_1E_PZT_MAX_TIME] = ALL_VALID;
   TC_look_up[SET_SU_4_PLASMA_1E_PZT_MAX_TIME] = ALL_VALID;
 
   TC_look_up[SET_SU_1_PLASMA_1I_PZT_MIN_TIME] = ALL_VALID;
   TC_look_up[SET_SU_2_PLASMA_1I_PZT_MIN_TIME] = ALL_VALID;
   TC_look_up[SET_SU_3_PLASMA_1I_PZT_MIN_TIME] = ALL_VALID;
   TC_look_up[SET_SU_4_PLASMA_1I_PZT_MIN_TIME] = ALL_VALID;
 
   TC_look_up[SET_SU_1_PLASMA_1I_PZT_MAX_TIME] = ALL_VALID;
   TC_look_up[SET_SU_2_PLASMA_1I_PZT_MAX_TIME] = ALL_VALID;
   TC_look_up[SET_SU_3_PLASMA_1I_PZT_MAX_TIME] = ALL_VALID;
   TC_look_up[SET_SU_4_PLASMA_1I_PZT_MAX_TIME] = ALL_VALID;

   TC_look_up[SET_COEFFICIENT_1]               = ALL_VALID;
   TC_look_up[SET_COEFFICIENT_2]               = ALL_VALID;
   TC_look_up[SET_COEFFICIENT_3]               = ALL_VALID;
   TC_look_up[SET_COEFFICIENT_4]               = ALL_VALID;
   TC_look_up[SET_COEFFICIENT_5]               = ALL_VALID;

}


void TC_InterruptService (void) INTERRUPT(TC_ISR_SOURCE) USED_REG_BANK(2)
/* Purpose        : Handles the TC interrupt                             */
/* Interface      : inputs  - TC MSB and LSB hardware registers          */
/*                            TC_look_up table giving valid TC addresses */
/*                            and TC codes                               */
/*                  outputs - TM data registers for received TC and TC   */
/*                            time tag                                   */
/*                            TM data registers for error and mode       */
/*                            status                                     */
/*                            TM MSB and LSB hardware registers          */
/*                            telemetry_pointer                          */
/*                            telemetry_end_pointer                      */
/*                            Telecommand Execution task mailbox         */
/* Preconditions  : none                                                 */
/* Postconditions :                                                      */
/*                  Mail sent to Telecommand Execution task, if TC was   */
/*                  valid                                                */
/* Algorithm      : - Read TC address and code from hardware registers   */
/*                  - Calculate parity                                   */
/*                  - If parity not Ok                                   */
/*                    - Set parity error                                 */
/*                  - Else                                               */ 
/*                    - Clear parity error                               */
/*                    - Check TC address and code                        */
/*                    - If TC Ok                                         */
/*                         Clear TC error and send mail                  */
/*                    - Else                                             */
/*                         Set TC error                                  */
/*                  - If TC is valid Send Status Register                */
/*                    - Send first two registers defined by TC code      */
/*                  - Else if TC is valid Send Science Data File         */
/*                    - Send first teo bytes from Science Data File      */
/*                  - Else if TC responce is enabled                     */
/*                    - Send Error Status and Mode Status                */
/*                  - If TC is invalid                                   */
/*                    - Disable TC responses                             */

{
   DIRECT_INTERNAL unsigned char TC_address;
   DIRECT_INTERNAL unsigned char TC_code;
   DIRECT_INTERNAL uint16_t      TC_word;
   /* Telecommand and it's parts */

   DIRECT_INTERNAL unsigned char par8, par4, par2, par1;
   /* Parity calculation results */

   DIRECT_INTERNAL unsigned char tmp_error_status;
   /* Temporary result of TC validity check */


   if (!TC_TIMER_OVERFLOW_FLAG)
   {
      /* TC is rejected. */

      telemetry_data.error_status |= TC_ERROR;
      /* Set TC Error bit. */

      return;
      /* Abort ISR. */
   }

   if ((TC_state == SC_TM_e) || (TC_state == memory_dump_e))
   {
      return;
      /* Abort ISR. */
   }

   STOP_TC_TIMER;
   INIT_TC_TIMER_MSB;
   INIT_TC_TIMER_LSB;
   CLEAR_TC_TIMER_OVERFLOW_FLAG;
   START_TC_TIMER;

   TC_address   = READ_TC_MSB;
   TC_code      = READ_TC_LSB;
   TC_word      = TC_address * 256 + TC_code;
   /* Get TC Address, TC Code and TC Word */

   if (TC_state == memory_patch_e)
   {
      Send_ISR_Mail(0, TC_word);
      return;
      /* This is not a normal telecommand, but word containing two bytes */
      /* of memory block to be written to data or code memory.           */
   }

   if (TC_state == register_TM_e)
   {
      TC_state = TC_handling_e;
      /* Register TM state is aborted */
 
      ResetInterruptMask(TM_ISR_MASK);
      /* Disable TM interrupt mask. Note that DisableInterrupt */
      /* cannot be called from the C51 ISR.                    */
   }


   par8 = TC_address ^ TC_code;
   par4 = (par8 & 0x0F) ^ (par8 >> 4);
   par2 = (par4 & 0x03) ^ (par4 >> 2);
   par1 = (par2 & 0x01) ^ (par2 >> 1);
   /* Calculate parity */

   TC_address >>= 1;

   tmp_error_status = 0;

   if (par1)
   {
      /* Parity error. */
   
      tmp_error_status |= PARITY_ERROR;
   }

   else
   {

      switch (TC_look_up[TC_address])
      {
         case ALL_INVALID:
         /* Invalid TC Address */
            tmp_error_status |= TC_ERROR;
            break;
 
         case ALL_VALID:
         /* All TC Codes are valid */
            Send_ISR_Mail(0, TC_word);
            break;
 
         case ONLY_EQUAL:
         /* TC Code should be equal to TC Address */
            if (TC_address != TC_code)
            {
               tmp_error_status |= TC_ERROR;
            }

            else
            {
               Send_ISR_Mail(0, TC_word);
            }
            break;
 
         case ON_OFF_TC:
         /* TC_Code must have ON , OFF or SELF_TEST value */
            if ((TC_code != ON_VALUE) && (TC_code != OFF_VALUE) && 
               (TC_code != SELF_TEST))
            {
               tmp_error_status |= TC_ERROR;
            }

            else
            {
               Send_ISR_Mail(0, TC_word);
            }
            break;
 
         case ONLY_EVEN:
         /* TC_Code must be even and not too big */
            if ((TC_code & 1) || (TC_code > LAST_EVEN))
            {
               tmp_error_status |= TC_ERROR;
            }

            else
            {
               Send_ISR_Mail(0, TC_word);
            }
            break;  
      }
   }

   if (((TC_address != SEND_STATUS_REGISTER) 
            || (tmp_error_status)) 
            && ((telemetry_data.error_status & TC_OR_PARITY_ERROR) == 0))
   {
      /* Condition 1 :                                        */
      /* (TC is not SEND STATUS REGISTER or TC is invalid).   */
      /* and condition 2:                                     */
      /* no invalid telecommands are recorded                 */
      /*                                                      */
      /* First condition checks that the Command Status and   */
      /* Command Time Tag registers should be updated.        */
      /* Second condition checks that the update can be done. */


      telemetry_data.TC_word = TC_word;
      COPY (telemetry_data.TC_time_tag, internal_time);
      /* Update TC registers in HK TM data area */
   }

   if (tmp_error_status)
   {
      /* TC is invalid. */

      telemetry_data.error_status |= tmp_error_status;
      WRITE_TM_MSB (telemetry_data.error_status);
      WRITE_TM_LSB (telemetry_data.mode_status);
      /* Send response to this TC to TM */

      return;
      /* Abort ISR because TC is invalid. */
   }


   if (TC_address == SEND_STATUS_REGISTER)
   {
      /* Send Status Register TC accepted */

      COPY(telemetry_data.time, internal_time);

      telemetry_pointer     = ((EXTERNAL unsigned char *)&telemetry_data) + TC_code;
      telemetry_end_pointer = ((EXTERNAL unsigned char *)&telemetry_data) + 
                              LAST_EVEN + 1;
      /* First TM register to be sent is given in TC_Code */

      CLEAR_TM_INTERRUPT_FLAG;

      WRITE_TM_MSB (*telemetry_pointer);
      telemetry_pointer++;
      WRITE_TM_LSB (*telemetry_pointer);
      telemetry_pointer++;

      TC_state = register_TM_e;

      SetInterruptMask(TM_ISR_MASK);
      /* Enable TM interrupt mask. Note that EnableInterrupt */
      /* cannot be called from the C51 ISR                   */

      if (telemetry_pointer > telemetry_end_pointer)
         telemetry_pointer = (EXTERNAL unsigned char *)&telemetry_data;      
   }

   else if (TC_address == SEND_SCIENCE_DATA_FILE)
   {
      /* Send Science Data File TC accepted. */

      if ((telemetry_data.mode_status & MODE_BITS_MASK) == DPU_SELF_TEST)
      {
         /* Wrong DEBIE mode. */

         telemetry_data.error_status |= TC_ERROR;
         WRITE_TM_MSB (telemetry_data.error_status);
         WRITE_TM_LSB (telemetry_data.mode_status);
         /* Send response to this TC to TM. */
      }

      else  
      {
         telemetry_pointer     = (EXTERNAL unsigned char *)&science_data;
         telemetry_end_pointer = ((EXTERNAL unsigned char *)
               &(science_data.event[free_slot_index])) - 1;
         /* Science telemetry stops to the end of the last used event */
         /* record of the Science Data memory.                        */

         science_data.length = (unsigned short int)
            (telemetry_end_pointer - telemetry_pointer + 1)/2;
         /* Store the current length of used science data. */  

         CLEAR_TM_INTERRUPT_FLAG;

         WRITE_TM_MSB (*telemetry_pointer);
         telemetry_pointer++;
         WRITE_TM_LSB (*telemetry_pointer);
         telemetry_pointer++;

         TC_state = SC_TM_e;

         SetInterruptMask(TM_ISR_MASK);
         /* Enable TM interrupt mask. Note that EnableInterrupt */
         /* cannot be called from a C51 ISR.                    */
      }
   }

   else if (TC_address == READ_DATA_MEMORY_LSB)
   {
      /* Read Data Memory LSB accepted. */

      if ( (TC_state != read_memory_e) ||
           ( ((unsigned int)address_MSB << 8) + TC_code
             > (END_SRAM3 - MEM_BUFFER_SIZE) + 1 ) )
      {
         /* Wrong TC state or wrong address range. */

         telemetry_data.error_status |= TC_ERROR;
         WRITE_TM_MSB (telemetry_data.error_status);
         WRITE_TM_LSB (telemetry_data.mode_status);
         /* Send response to this TC to TM. */

         TC_state = TC_handling_e;
      }

      else
      {
         telemetry_pointer = 
            DATA_POINTER((int)address_MSB * 256 + TC_code);
         telemetry_end_pointer = telemetry_pointer + MEM_BUFFER_SIZE;

         CLEAR_TM_INTERRUPT_FLAG;

         WRITE_TM_MSB (telemetry_data.error_status);
         WRITE_TM_LSB (telemetry_data.mode_status);
         /* First two bytes of Read Data Memory sequence. */

         read_memory_checksum = tmp_error_status ^ telemetry_data.mode_status;

         TC_state = memory_dump_e;

         SetInterruptMask(TM_ISR_MASK);
      }
   }

   else
   {
      /* Some other TC accepted. */

      WRITE_TM_MSB (telemetry_data.error_status);
      WRITE_TM_LSB (telemetry_data.mode_status);
      /* Send response to this TC to TM. */
   }

}


unsigned char PatchExecCommandOk (unsigned char execution_command)
{
   switch (execution_command)
   {
      case 0:
      case 0x09:
      case 0x37:
      case 0x5A:
         return 1;
   }
   return 0;
}


void MemoryPatch(telecommand_t EXTERNAL *command)
/* Purpose        : Handles received telecommand in memory patch state   */
/* Interface      : inputs  - command, received telecommand              */
/*                            address_MSB, MSB of patch area             */
/*                            address_LSB, LSB of patch area             */
/*                            memory_transfer_buffer, buffer for bytes   */
/*                               to be written to patch area             */
/*                            memory_buffer_index, index to above buffer */
/*                  outputs - memory_transfer_buffer, as above           */
/*                            memory_buffer_index, as above              */
/*                            code_not_patched, indicates if code      */
/*                               checksum is vali or not                 */
/*                            telemetry_data.mode_status, Mode Status    */
/*                               telemetry register                      */
/*                            TC_state, TC Execution task state          */
/* Preconditions  : TC_state is memory patch.                            */
/*                  Destination memory type (data/code) is memorized.    */
/* Postconditions : After last phase code or data memory patched if      */
/*                     operation succeeded.                              */
/* Algorithm      : - If memory_transfer_buffer not filled               */
/*                     - update write_checksum (XOR with MSB and LSB of  */
/*                          the telecommand word)                        */
/*                     - store TC word to memory_transfer_buffer         */
/*                  - else                                               */
/*                     - If checksum of TC block Ok                      */
/*                        - If data memory patch                         */
/*                           - copy memory_tranfer_buffer to data memory */
/*                        - else (code memory patch)                     */
/*                           - call PatchCode function                   */
/*                     - else                                            */
/*                        - set MEMORY_WRITE_ERROR bit in ModeStatus     */

{
   memory_patch_variables_t EXTERNAL patch_info;
   /* Parameters for PatchCode function. */

   data_address_t INDIRECT_INTERNAL address;
   /* Start address of the memory area to be patched. */

   uint_least8_t INDIRECT_INTERNAL i;
   /* Loop variable. */

   unsigned char INDIRECT_INTERNAL TC_msb;
   /* Most significant byte of the TC word. */

   TC_msb = (command -> TC_word) >> 8;
   write_checksum ^= TC_msb;

   if (memory_buffer_index < MEM_BUFFER_SIZE)
   {
      /* Filling the buffer bytes to be written to code or data */
      /* memory.                                                */

      write_checksum ^= command -> TC_code;

      memory_transfer_buffer[memory_buffer_index] = TC_msb;
      memory_buffer_index++;
      memory_transfer_buffer[memory_buffer_index] = command -> TC_code;
      memory_buffer_index++;

      TC_timeout = WRITE_MEMORY_TIMEOUT;
   }

   else
   {
      /* Now all bytes for memory area to be patched have been */
      /* received.                                             */

      if (write_checksum == command -> TC_code)
      {
         /* Checksum Ok. */

         address = ((unsigned int)address_MSB)*256 + address_LSB;

         switch (memory_type)
         {
            case data_e:
               /* Write to the data memory. */

               if (address <= (END_SRAM3 - MEM_BUFFER_SIZE + 1))
               {
                  for(i=0; i<MEM_BUFFER_SIZE; i++)
                  {
                     SET_DATA_BYTE(address + i, memory_transfer_buffer[i]);
                  }
               }
   
               else
               {
                  Set_TC_Error();
               }

               break;

            case code_e:
               /* Write to the code memory. */

               if ((address >= BEGIN_SRAM1) && 
                   (address <= END_SRAM1 - MEM_BUFFER_SIZE + 1) &&
                   (PatchExecCommandOk (TC_msb)))
    
               {
                  /* Destination section resides in SRAM1.   */
 
                  code_not_patched = 0;
                  /* Next code checksum not valid, because code memory */
                  /* will be patched.                                  */

                  patch_info.source            = memory_transfer_buffer,
                  patch_info.destination       = address,
                  patch_info.data_amount       = MEM_BUFFER_SIZE,
                  patch_info.execution_command = TC_msb;
                  /* Set parameters for the MemoryPatch function   */
                  /* (see definition of memory_patch_variables_t). */

                  PatchCode(&patch_info);
                  /* May or may not return here. */
               }

               else
               {
                  /* Destination section out side SRAM1.   */
                  Set_TC_Error();
               }

               break;
         }
      }

      else
      {
         /* Checksum failure. */

         SetModeStatusError(MEMORY_WRITE_ERROR);
         /* Set memory write error bit in Mode Status register. */
      }

      TC_state = TC_handling_e;
   }
}


void WriteMemory(telecommand_t EXTERNAL *command)
/* Purpose        : Handles received telecommand in the WriteMemory state    */
/* Interface      : inputs  - Parameter "command" containing received        */
/*                            telecommand or memory byte                     */
/*                            memory_type defining code or data memory       */
/*                            destination                                    */
/*                  outputs - address_LSB, LSB of the patch area             */
/*                            TC_state                                       */
/* Preconditions  : TC_state is write_memory                                 */
/*                  Destination memory type (data/code) is memorized         */
/* Postconditions : LSB of the patch area address is memorized.              */
/* Algorithm      : - update write_checksum (XOR with TC word MSB and LSB)   */
/*                  - if telecommand is LSB setting of patch memory address  */
/*                    of selected memory type                                */
/*                      - set TC state to memory_patch_e                     */
/*                      - memorize address LSB                               */
/*                      - clear memory_buffer_index                          */
/*                      - set TC timeout to WRITE_MEMORY_TIMEOUT             */
/*                  - else                                                   */
/*                      - set TC state to TC_handling_e                      */

{
   /* Expecting LSB of the start address for the memory area to be patched. */

   write_checksum ^= ((command -> TC_word) >> 8) ^ (command -> TC_code);

   if (   (   (command -> TC_address == WRITE_CODE_MEMORY_LSB)
           && (memory_type           == code_e))
       || (   (command -> TC_address == WRITE_DATA_MEMORY_LSB)
           && (memory_type           == data_e)))
   {
      TC_state = memory_patch_e;

      address_LSB = command -> TC_code;
      memory_buffer_index = 0;
      TC_timeout = WRITE_MEMORY_TIMEOUT;
   }

   else
   {
      TC_state = TC_handling_e;

      Set_TC_Error();
   }
}


void UpdateTarget(telecommand_t EXTERNAL *command)
/* Purpose         : Updates a HW register or some global variable according */
/*                   to the parameter "command"                              */
/* Interface       : inputs  - Parameter "command" containing received       */
/*                             telecommand                                   */
/*                   outputs - telemetry data                                */
/* Preconditions  : The parameter "command" contains a valid telecommand     */
/* Postconditions  : A HW register or global variable is updated (depend on  */
/*                   "command")                                              */
/* Algorithm       : - switch "command"                                      */
/*                     - case Set Coefficient:                               */
/*                          set given quality coefficient value in telemetry */
/*                          according to "command"                           */
/*                     - case Min/Max Time:                                  */
/*                          set Min/Max Time according to "command"          */
/*                     - case Classification Level:                          */
/*                          set classification level according to "command"  */
/*                     - case Error Status Clear:                            */
/*                          clear error indicating bits from telemetry       */
/*                     - case Set Time Byte:                                 */
/*                          set Debie time byte# according to "command"      */
/*                     - case Clear Watchdog/Checksum Failure counter        */
/*                          clear Watchdog/Checksum Failure counter          */
/*                     - case Switch SU # On/Off/SelfTest                    */
/*                          switch SU to On/Off/SelfTest according to        */
/*                          "command"                                        */
/*                     - case Set Threshold                                  */
/*                          set Threshold according to "command"             */
{
   EXTERNAL sensor_unit_t SU_setting;
   /* Holds parameters for "SetSensorUnit" operation                    */
   /* Must be in external memory, because the parameter to the function */
   /* is pointer to external memory                                     */

   EXTERNAL trigger_set_t new_threshold;

   dpu_time_t EXTERNAL new_time;

   sensor_index_t EXTERNAL SU_index;

   SU_index = ((command -> TC_address) >> 4) - 2;


   switch (command -> TC_address)
   {
      case SET_COEFFICIENT_1:
      case SET_COEFFICIENT_2:
      case SET_COEFFICIENT_3:
      case SET_COEFFICIENT_4:
      case SET_COEFFICIENT_5:
 
         telemetry_data.coefficient[(command -> TC_address)&0x07] =
            command -> TC_code;
         break;

      case SET_SU_1_PLASMA_1E_1I_MAX_TIME:
      case SET_SU_2_PLASMA_1E_1I_MAX_TIME:
      case SET_SU_3_PLASMA_1E_1I_MAX_TIME:
      case SET_SU_4_PLASMA_1E_1I_MAX_TIME:
 
          SU_config[SU_index] -> plasma_1_plus_to_minus_max_time =
             command -> TC_code;
          break;
 
      case SET_SU_1_PLASMA_1E_PZT_MIN_TIME:
      case SET_SU_2_PLASMA_1E_PZT_MIN_TIME:
      case SET_SU_3_PLASMA_1E_PZT_MIN_TIME:
      case SET_SU_4_PLASMA_1E_PZT_MIN_TIME:
 
          SU_config[SU_index] -> plasma_1_plus_to_piezo_min_time =
             command -> TC_code;
          break;
 
      case SET_SU_1_PLASMA_1E_PZT_MAX_TIME:
      case SET_SU_2_PLASMA_1E_PZT_MAX_TIME:
      case SET_SU_3_PLASMA_1E_PZT_MAX_TIME:
      case SET_SU_4_PLASMA_1E_PZT_MAX_TIME:
 
          SU_config[SU_index] -> plasma_1_plus_to_piezo_max_time =
             command -> TC_code;
          break;
 
      case SET_SU_1_PLASMA_1I_PZT_MIN_TIME:
      case SET_SU_2_PLASMA_1I_PZT_MIN_TIME:
      case SET_SU_3_PLASMA_1I_PZT_MIN_TIME:
      case SET_SU_4_PLASMA_1I_PZT_MIN_TIME:
 
          SU_config[SU_index] -> plasma_1_minus_to_piezo_min_time =
             command -> TC_code;
          break;
 
      case SET_SU_1_PLASMA_1I_PZT_MAX_TIME:
      case SET_SU_2_PLASMA_1I_PZT_MAX_TIME:
      case SET_SU_3_PLASMA_1I_PZT_MAX_TIME:
      case SET_SU_4_PLASMA_1I_PZT_MAX_TIME:
 
          SU_config[SU_index] -> plasma_1_minus_to_piezo_max_time =
             command -> TC_code;
          break;

      case SET_SU_1_PLASMA_1P_CLASS_LEVEL:
      case SET_SU_2_PLASMA_1P_CLASS_LEVEL:
      case SET_SU_3_PLASMA_1P_CLASS_LEVEL:
      case SET_SU_4_PLASMA_1P_CLASS_LEVEL:

         SU_config[SU_index] -> plasma_1_plus_classification = 
            command -> TC_code;
         break;

      case SET_SU_1_PLASMA_1M_CLASS_LEVEL:
      case SET_SU_2_PLASMA_1M_CLASS_LEVEL:
      case SET_SU_3_PLASMA_1M_CLASS_LEVEL:
      case SET_SU_4_PLASMA_1M_CLASS_LEVEL:

         SU_config[SU_index] -> plasma_1_minus_classification = 
            command -> TC_code;
         break;

      case SET_SU_1_PLASMA_2P_CLASS_LEVEL:
      case SET_SU_2_PLASMA_2P_CLASS_LEVEL:
      case SET_SU_3_PLASMA_2P_CLASS_LEVEL:
      case SET_SU_4_PLASMA_2P_CLASS_LEVEL:

         SU_config[SU_index] -> plasma_2_plus_classification = 
            command -> TC_code;
         break;

      case SET_SU_1_PIEZO_1_CLASS_LEVEL:
      case SET_SU_2_PIEZO_1_CLASS_LEVEL:
      case SET_SU_3_PIEZO_1_CLASS_LEVEL:
      case SET_SU_4_PIEZO_1_CLASS_LEVEL:

         SU_config[SU_index] -> piezo_1_classification = 
            command -> TC_code;
         break;

      case SET_SU_1_PIEZO_2_CLASS_LEVEL:
      case SET_SU_2_PIEZO_2_CLASS_LEVEL:
      case SET_SU_3_PIEZO_2_CLASS_LEVEL:
      case SET_SU_4_PIEZO_2_CLASS_LEVEL:
      
         SU_config[SU_index] -> piezo_2_classification = 
            command -> TC_code;
         break;

      case ERROR_STATUS_CLEAR:

         ClearErrorStatus();
         Clear_RTX_Errors();
         ClearSoftwareError();
         ClearModeStatusError();
         Clear_SU_Error();
         /* Clear Error Status register, RTX and software error indicating bits  */
         /* and Mode and SU Status registers.                                    */

         break;


      case SET_TIME_BYTE_3:

         new_time = ((dpu_time_t) command -> TC_code) << 24;
         COPY (internal_time, new_time);
         TC_timeout = SET_TIME_TC_TIMEOUT;

         break;

      case SET_TIME_BYTE_2:

         if (previous_TC.TC_address == SET_TIME_BYTE_3)
         {
            COPY (new_time, internal_time);
 
            new_time &= 0xFF000000;
            new_time |= 
               ((dpu_time_t) command -> TC_code) << 16;

            COPY (internal_time, new_time);

            TC_timeout = SET_TIME_TC_TIMEOUT;
         }

         else
         {
            Set_TC_Error();
         }

         break;

      case SET_TIME_BYTE_1:

         if (previous_TC.TC_address == SET_TIME_BYTE_2)
         {
            COPY (new_time, internal_time);
 
            new_time &= 0xFFFF0000;
            new_time |= 
               ((dpu_time_t) command -> TC_code) << 8;

            COPY (internal_time, new_time);

            TC_timeout = SET_TIME_TC_TIMEOUT;
         }

         else
         {
            Set_TC_Error();
         }

         break;

      case SET_TIME_BYTE_0:

         if (previous_TC.TC_address == SET_TIME_BYTE_1)
         {
            COPY (new_time, internal_time);
 
            new_time &= 0xFFFFFF00;
            new_time |= 
               ((dpu_time_t) command -> TC_code);

            COPY (internal_time, new_time);
         }

         else
         {
            Set_TC_Error();
         }

         break;

      case CLEAR_WATCHDOG_FAILURES:

         telemetry_data.watchdog_failures = 0;
         break;
         
      case CLEAR_CHECKSUM_FAILURES:

         telemetry_data.checksum_failures = 0;
         break;

      case SWITCH_SU_1:
      case SWITCH_SU_2:
      case SWITCH_SU_3:
      case SWITCH_SU_4:

         if (GetMode() != ACQUISITION)
         {
            SU_setting.SU_number = SU_index + SU_1;

            switch (command -> TC_code)
            {
               case ON_VALUE:
                  Start_SU_SwitchingOn(SU_index, &
                     (SU_setting.execution_result));
                  break;

               case OFF_VALUE:
                  SetSensorUnitOff(SU_index, &(SU_setting.execution_result));
                  break;

               case SELF_TEST:
                  SU_setting.SU_state              = self_test_mon_e;
                  SU_setting.expected_source_state = on_e;
                  Switch_SU_State (&SU_setting);
                  break;
            }

            if (SU_setting.execution_result == SU_STATE_TRANSITION_FAILED)
            {
                /* The requested SU state transition failed. */

                Set_TC_Error();
            }

         }
         
         else

         {
            Set_TC_Error();
         }

         break;

      case SET_SU_1_PLASMA_1P_THRESHOLD:

         new_threshold.sensor_unit = SU_1;
         new_threshold.channel     = PLASMA_1_PLUS;
         new_threshold.level       = command -> TC_code;
         SetTriggerLevel(&new_threshold);

         telemetry_data.sensor_unit_1.plasma_1_plus_threshold = 
            command -> TC_code;
         break;

      case SET_SU_2_PLASMA_1P_THRESHOLD:

         new_threshold.sensor_unit = SU_2;
         new_threshold.channel     = PLASMA_1_PLUS;
         new_threshold.level       = command -> TC_code;
         SetTriggerLevel(&new_threshold);

        telemetry_data.sensor_unit_2.plasma_1_plus_threshold = 
            command -> TC_code;
         break;

      case SET_SU_3_PLASMA_1P_THRESHOLD:

         new_threshold.sensor_unit = SU_3;
         new_threshold.channel     = PLASMA_1_PLUS;
         new_threshold.level       = command -> TC_code;
         SetTriggerLevel(&new_threshold);

         telemetry_data.sensor_unit_3.plasma_1_plus_threshold = 
            command -> TC_code;
         break;

      case SET_SU_4_PLASMA_1P_THRESHOLD:

         new_threshold.sensor_unit = SU_4;
         new_threshold.channel     = PLASMA_1_PLUS;
         new_threshold.level       = command -> TC_code;
         SetTriggerLevel(&new_threshold);

         telemetry_data.sensor_unit_4.plasma_1_plus_threshold = 
            command -> TC_code;
         break;

      case SET_SU_1_PLASMA_1M_THRESHOLD:

         new_threshold.sensor_unit = SU_1;
         new_threshold.channel     = PLASMA_1_MINUS;
         new_threshold.level       = command -> TC_code;
         SetTriggerLevel(&new_threshold);

         telemetry_data.sensor_unit_1.plasma_1_minus_threshold = 
            command -> TC_code;
         break;

      case SET_SU_2_PLASMA_1M_THRESHOLD:

         new_threshold.sensor_unit = SU_2;
         new_threshold.channel     = PLASMA_1_MINUS;
         new_threshold.level       = command -> TC_code;
         SetTriggerLevel(&new_threshold);

         telemetry_data.sensor_unit_2.plasma_1_minus_threshold = 
            command -> TC_code;
         break;

      case SET_SU_3_PLASMA_1M_THRESHOLD:

         new_threshold.sensor_unit = SU_3;
         new_threshold.channel     = PLASMA_1_MINUS;
         new_threshold.level       = command -> TC_code;
         SetTriggerLevel(&new_threshold);

         telemetry_data.sensor_unit_3.plasma_1_minus_threshold = 
            command -> TC_code;
         break;

      case SET_SU_4_PLASMA_1M_THRESHOLD:

         new_threshold.sensor_unit = SU_4;
         new_threshold.channel     = PLASMA_1_MINUS;
         new_threshold.level       = command -> TC_code;
         SetTriggerLevel(&new_threshold);

         telemetry_data.sensor_unit_4.plasma_1_minus_threshold = 
            command -> TC_code;
         break;

      case SET_SU_1_PIEZO_THRESHOLD:

         new_threshold.sensor_unit = SU_1;
         new_threshold.channel     = PZT_1_2;
         new_threshold.level       = command -> TC_code;
         SetTriggerLevel(&new_threshold);

         telemetry_data.sensor_unit_1.piezo_threshold = command -> TC_code;
         break;

      case SET_SU_2_PIEZO_THRESHOLD:

         new_threshold.sensor_unit = SU_2;
         new_threshold.channel     = PZT_1_2;
         new_threshold.level       = command -> TC_code;
         SetTriggerLevel(&new_threshold);

         telemetry_data.sensor_unit_2.piezo_threshold = command -> TC_code;
         break;

      case SET_SU_3_PIEZO_THRESHOLD:

         new_threshold.sensor_unit = SU_3;
         new_threshold.channel     = PZT_1_2;
         new_threshold.level       = command -> TC_code;
         SetTriggerLevel(&new_threshold);

         telemetry_data.sensor_unit_3.piezo_threshold = command -> TC_code;
         break;

      case SET_SU_4_PIEZO_THRESHOLD:

         new_threshold.sensor_unit = SU_4;
         new_threshold.channel     = PZT_1_2;
         new_threshold.level       = command -> TC_code;
         SetTriggerLevel(&new_threshold);

         telemetry_data.sensor_unit_4.piezo_threshold = command -> TC_code;
         break;

      default:
         /* Telecommands that will not be implemented in the Prototype SW */
         break;
   }
}


void ExecuteCommand(telecommand_t EXTERNAL *command)
/* Purpose        : Executes telecommand                                     */
/* Interface      : inputs      - Parameter "command" containing received    */
/*                                telecommand                                */
/*                  outputs     - TC_state                                   */
/*                                address_MSB                                */
/*                                memory_type                                */
/*                  subroutines - UpdateTarget                               */
/*                                StartAcquisition                           */
/*                                StopAcquisition                            */
/*                                SoftReset                                  */
/*                                Set_TC_Error                               */
/*                                Switch_SU_State                            */
/*                                Reboot                                     */
/*                                SetMode                                    */
/*                                UpdateTarget                               */
/* Preconditions  : The parameter "command" contains a valid telecommand     */
/*                  TC_state is TC_handling                                  */
/* Postconditions : Telecommand is executed                                  */
/*                  TC_state updated when appropriate (depend on "command")  */
/*                  HW registers modified when appropriate (depend on        */
/*                  "command")                                               */
/*                  Global variables modified when appropriate (depend on    */
/*                  "command")                                               */
/* Algorithm      : - switch "command"                                       */
/*                    - case Send Sciece Data File : set TC_state to SC_TM   */
/*                    - case Send Status Register  : set TC_state to         */
/*                         RegisterTM                                        */
/*                    - case Read Data Memory MSB  : memorize the address    */
/*                         MSB given in the TC_code and set TC_state to      */
/*                         read_memory                                       */
/*                    - case Write Code Memory MSB : memorize the address    */
/*                         MSB given in the TC_code, memorize code           */
/*                         destination selection and set TC_state to         */
/*                         write_memory                                      */
/*                    - case Write Data Memory MSB : memorize the address    */
/*                         MSB given in the TC_code, memorize data           */
/*                         destination selection and set TC_state to         */
/*                         write_memory                                      */
/*                    - case Read/Write Memory LSB : ignore telecommand      */
/*                    - case Soft Reset            : call SoftReset          */
/*                    - case Start Acquisition     : call StartAcquisition   */
/*                    - case Stop Acquisition      : call StopAcquisition    */
/*                    - default : call UpdateTarget                          */

{
   sensor_unit_t EXTERNAL SU_setting;
   unsigned char          error_flag;
   sensor_number_t        i;

   switch (command -> TC_address)
   {
      case SEND_SCIENCE_DATA_FILE:
         break;

      case SEND_STATUS_REGISTER:
         break;

      case READ_DATA_MEMORY_MSB:
         address_MSB = command -> TC_code;
         TC_state    = read_memory_e;
         break;

      case WRITE_CODE_MEMORY_MSB:
         if (GetMode() == STAND_BY)
         {
            address_MSB    = command -> TC_code;
            memory_type    = code_e;
            TC_timeout     = WRITE_MEMORY_TIMEOUT;
            write_checksum = 
               ((command -> TC_word) >> 8) ^ (command -> TC_code);
            TC_state       = write_memory_e;
         }

         else
         {
             Set_TC_Error();
         }

         break;

      case WRITE_DATA_MEMORY_MSB:
         if (GetMode() == STAND_BY)
         {
            address_MSB = command -> TC_code;
            memory_type = data_e;
            TC_timeout     = WRITE_MEMORY_TIMEOUT;
            write_checksum = 
               ((command -> TC_word) >> 8) ^ (command -> TC_code);
            TC_state    = write_memory_e;
         }
 
        else
         {
             Set_TC_Error();
         }

        break;

      case READ_DATA_MEMORY_LSB:
         break;

      case WRITE_CODE_MEMORY_LSB:
      case WRITE_DATA_MEMORY_LSB:
         if (TC_state != write_memory_e)
	 {
            Set_TC_Error();
	 }
         break;

      case SOFT_RESET:
         Reboot(soft_reset_e);
         /* Software is rebooted, no return to this point. */

         break;

      case START_ACQUISITION:
         error_flag = 0;
 
         for (i=SU_1; i<=SU_4; i++)
         {
             if ((ReadSensorUnit(i) == start_switching_e) ||
                 (ReadSensorUnit(i) == switching_e))
             {
                /* SU is being switched on. */
 
                error_flag = 1;
                /* StartAcquisition TC has to be rejected. */
             }
         }

         if ((GetMode() == STAND_BY) && (error_flag == 0))
         {
            SU_setting.SU_state              = acquisition_e;
            SU_setting.expected_source_state = on_e;

            SU_setting.SU_number = SU_1;
            Switch_SU_State (&SU_setting);
            /* Try to switch SU 1 to Acquisition state. */

            SU_setting.SU_number = SU_2;
            Switch_SU_State (&SU_setting);
            /* Try to switch SU 2 to Acquisition state. */

            SU_setting.SU_number = SU_3;
            Switch_SU_State (&SU_setting);
            /* Try to switch SU 3 to Acquisition state. */

            SU_setting.SU_number = SU_4;
            Switch_SU_State (&SU_setting);
            /* Try to switch SU 4 to Acquisition state. */

            CLEAR_HIT_TRIGGER_ISR_FLAG;
       
            ResetDelayCounters();
            /* Resets the SU logic that generates Hit Triggers.    */
            /* Brings T2EX to a high level, making a new falling   */
            /* edge possible.                                      */
            /* This statement must come after the above "clear",   */
            /* because a reversed order could create a deadlock    */
            /* situation.                                          */
  
            SetMode(ACQUISITION);
         }

         else
         {
            Set_TC_Error();
         }
         break;

      case STOP_ACQUISITION:
         if (GetMode() == ACQUISITION)
         {
            SU_setting.SU_state              = on_e;
            SU_setting.expected_source_state = acquisition_e;

            SU_setting.SU_number = SU_1;
            Switch_SU_State (&SU_setting);
            /* Try to switch SU 1 to On state. */

            SU_setting.SU_number = SU_2;
            Switch_SU_State (&SU_setting);
            /* Try to switch SU 2 to On state. */

            SU_setting.SU_number = SU_3;
            Switch_SU_State (&SU_setting);
            /* Try to switch SU 3 to On state. */

            SU_setting.SU_number = SU_4;
            Switch_SU_State (&SU_setting);
            /* Try to switch SU 4 to On state. */
         
            SetMode(STAND_BY);
         }

         else
         {
            Set_TC_Error();
         }
         break;

      default:
         UpdateTarget(command);
   }
}            


static EXTERNAL incoming_mail_t TC_mail;
/* Holds parameters for the mail waiting function.                        */
/* Must be in xdata memory because parameter of subrounite is pointer     */
/* to xdata area.                                                         */

static EXTERNAL telecommand_t received_command;
/* Telecommand read form the mailbox is stored to TC_word component.      */
/* Must be in external memory because parameters of subroutines (pointers */
/* to xdata area).                                                        */


void InitTelecommandTask (void)

/* Purpose        : Initialize the global state of Telecommand Execution     */
/* Interface      : inputs      - none                                       */
/*                  outputs     - TC state                                   */
/*                  subroutines - DisableInterrupt                           */
/*                                EnableInterrupt                            */
/* Preconditions  : none                                                     */
/* Postconditions : TelecommandExecutionTask is operational.                 */
/* Algorithm      : - initialize task variables.                             */

{
   InitTC_LookUp();
   
   TC_state            = TC_handling_e;
 
   TC_mail.mailbox_number = TCTM_MAILBOX;
   TC_mail.message        = &(received_command.TC_word);
   /* Parameters for mail waiting function.   */
   /* Time-out set separately.                */

   previous_TC.TC_word    = 0;
   previous_TC.TC_address = UNUSED_TC_ADDRESS;
   previous_TC.TC_code    = 0;

   DisableInterrupt(TM_ISR_SOURCE);
   EnableInterrupt(TC_ISR_SOURCE);
}


void HandleTelecommand (void)

/* Purpose        : Waits for and handles one Telecommand from the TC ISR    */
/* Interface      : inputs      - Telecommand Execution task mailbox         */
/*                                TC state                                   */
/*                  outputs     - TC state                                   */
/*                  subroutines - ReadMemory                                 */
/*                                WriteMemory                                */
/*                                ExecuteCommand                             */
/* Preconditions  : none                                                     */
/* Postconditions : one message removed from the TC mailbox                  */
/* Algorithm      : - wait for mail to Telecommand Execution task mailbox    */
/*                  - if mail is "TM_READY" and TC state is either           */
/*                    "SC_TM_e" or "memory_dump_e".                          */
/*                    - if TC state is "SC_TM_e"                             */
/*                       - call ClearEvents function                         */
/*                    - set TC state to TC handling                          */
/*                  - else switch TC state                                   */
/*                    - case ReadMemory_e  : Check received TC's address     */
/*                    - case WriteMemory_e : call WriteMemory function       */
/*                    - case MemoryPatch_e : call MemoryPatch function       */
/*                    - case TC_Handling : call ExecuteCommand               */
/*                NOTE:   case register_TM_e is left out because             */
/*                        operation of SEND_STATUS_REGISTER TC does not      */
/*                        require any functionalites of this task.           */  

{
   TC_mail.timeout = TC_timeout;

   WaitMail(&TC_mail);

   TC_timeout = 0;
   /* Default value */

   if (TC_mail.execution_result == TIMEOUT_OCCURRED)
   {
      previous_TC.TC_word    = 0;
      previous_TC.TC_address = UNUSED_TC_ADDRESS;
      previous_TC.TC_code    = 0;
      /* Forget previous telecommand. */

      if (TC_state != TC_handling_e)
      {
         /* Memory R/W time-out. */
         Set_TC_Error();
      }

      TC_state = TC_handling_e; 
   }

   else if (TC_mail.execution_result == MSG_RECEIVED)
   {
      received_command.TC_address = TC_ADDRESS (received_command.TC_word);
      received_command.TC_code    = TC_CODE    (received_command.TC_word);

      if (((TC_state == SC_TM_e) || (TC_state == memory_dump_e)) &&
          (received_command.TC_word == TM_READY))

      /* Note that in order to this condition to be sufficient, only */
      /* TM interrupt service should be allowed to send mail to this */
      /* task in the TC states mentioned.                            */

      {
         DisableInterrupt(TM_ISR_SOURCE);

         if (TC_state == SC_TM_e)
         {
           ClearEvents();
         }

         TC_state = TC_handling_e;
      }
      else
      {
         switch (TC_state)
         {

            case read_memory_e:

               if (received_command.TC_address != READ_DATA_MEMORY_LSB)
               {
                  Set_TC_Error();
                  TC_state = TC_handling_e;
               }

               break;

            case write_memory_e:
               WriteMemory (&received_command);
               break;

            case memory_patch_e:
               MemoryPatch (&received_command);
               break;

            case TC_handling_e:
               ExecuteCommand (&received_command);
               break;

         }

      }

      STRUCT_ASSIGN (previous_TC, received_command, telecommand_t);
   }

   else
   {
      /* Nothing is done if WaitMail returns an error message. */
   }
}


void TelecommandExecutionTask(void) TASK(TC_TM_INTERFACE_TASK) 
                                    PRIORITY(TC_TM_INTERFACE_PR)
/* Purpose        : Implements the highest level of Telecommand Execution    */
/*                  task                                                     */
/* Interface      : inputs      - Telecommand Execution task mailbox         */
/*                                TC state                                   */
/*                  outputs     - TC state                                   */
/*                  subroutines - InitTelecommandTask                        */
/*                                HandleTeleommand                           */
/* Preconditions  : none                                                     */
/* Postconditions : This function does not return.                           */
/* Algorithm      : - InitTelecommandTask                                    */
/*                  - loop forever:                                          */
/*                    - HandleTelecommand                                    */
                
{
   InitTelecommandTask ();

   while(1)
   {
      HandleTelecommand ();
   }
}


void Set_TC_Error(void)
/* Purpose        : This function will be called always when TC_ERROR bit in */
/*                  the ErrorStatus register is set.                         */
/* Interface      : inputs      - Error_status register                      */
/*                  outputs     - Error_status register                      */
/*                  subroutines - none                                       */
/* Preconditions  : none                                                     */
/* Postconditions : none                                                     */
/* Algorithm      : - Disable interrupts                                     */
/*                  - Write to Error Status register                         */
/*                  - Enable interrupts                                      */
{
   DISABLE_INTERRUPT_MASTER;

   telemetry_data.error_status |= TC_ERROR;

   ENABLE_INTERRUPT_MASTER;
}

/*Assign pointers to tasks*/
void (* EXTERNAL TC_task)(void) = TelecommandExecutionTask;
